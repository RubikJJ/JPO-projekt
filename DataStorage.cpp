#include "DataStorage.h"
#include <iostream>
#include <fstream>
#include <string>
#include <curl/curl.h>

using namespace std;

size_t DataStorage::WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

DataStorage::DataStorage(const string& path) : dataPath(path) {
    // SprawdŸ czy koñczy siê na "/"
    if (!dataPath.empty() && dataPath.back() != '/') {
        dataPath += '/';
    }
}

bool DataStorage::performCurlRequest(const string& url, string& response) {
    CURL* curl = curl_easy_init();
    if (!curl) return false;

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    return (res == CURLE_OK);
}

bool DataStorage::parseJsonResponse(const string& jsonResponse, Json::Value& parsedRoot) {
    Json::CharReaderBuilder builder;
    JSONCPP_STRING errs;

    const unique_ptr<Json::CharReader> reader(builder.newCharReader());
    return reader->parse(jsonResponse.c_str(), jsonResponse.c_str() + jsonResponse.size(), &parsedRoot, &errs);
}



Json::Value DataStorage::loadJsonData(const string& filename) {
    Json::Value root;
    string fullPath = dataPath + filename;
    ifstream file(fullPath);

    if (!file.is_open()) {
        cerr << "B³¹d otwierania pliku do odczytu: " << fullPath << endl;
        return root;
    }

    Json::CharReaderBuilder builder;
    JSONCPP_STRING errs;
    if (!Json::parseFromStream(builder, file, &root, &errs)) {
        cerr << "B³¹d parsowania pliku JSON: " << errs << endl;
    }

    file.close();
    return root;
}

bool DataStorage::dataExists(const string& filename) {
    string fullPath = dataPath + filename;
    ifstream file(fullPath);
    return file.good();
}

void DataStorage::saveJsonToFile(const Json::Value& root, const string& filename) {
    ofstream file(filename, ios::out | ios::trunc);
    if (!file.is_open()) {
        cerr << "B³¹d otwierania pliku do zapisu: " << filename << endl;
        return;
    }

    Json::StreamWriterBuilder writer;
    writer["emitUTF8"] = true;

    unique_ptr<Json::StreamWriter> jsonWriter(writer.newStreamWriter());
    jsonWriter->write(root, &file);

    file.close();
    cout << "Dane zapisane do " << filename << endl;
}

void DataStorage::getStationData() {
    string api_url = "https://api.gios.gov.pl/pjp-api/rest/station/findAll";
    string response;

    if (performCurlRequest(api_url, response)) {
        Json::Value root;
        if (parseJsonResponse(response, root)) {
            Json::Value stationData(Json::arrayValue); // JSON tablica

            for (const auto& station : root) {
                Json::Value stationEntry;
                stationEntry["id"] = station["id"];
                stationEntry["stationName"] = station["stationName"];
                stationEntry["gegrLat"] = station["gegrLat"];
                stationEntry["gegrLon"] = station["gegrLon"];

                if (!station["city"].isNull()) {
                    Json::Value cityData;
                    cityData["id"] = station["city"]["id"];
                    cityData["name"] = station["city"]["name"];

                    if (!station["city"]["commune"].isNull()) {
                        Json::Value communeData;
                        communeData["communeName"] = station["city"]["commune"]["communeName"];
                        communeData["districtName"] = station["city"]["commune"]["districtName"];
                        communeData["provinceName"] = station["city"]["commune"]["provinceName"];
                        cityData["commune"] = communeData;
                    }

                    stationEntry["city"] = cityData;
                }

                stationEntry["addressStreet"] = station["addressStreet"].isNull() ? "null" : station["addressStreet"];

                stationData.append(stationEntry);
            }

            saveJsonToFile(stationData, "stations.json");
        }
    }
}
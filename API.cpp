#include "API.h"
#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>
#include <locale>

size_t AirQualityAPI::WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

AirQualityAPI::AirQualityAPI() {
    // Ustawienie polskich znaków
    setlocale(LC_ALL, "Polish");
    std::locale::global(std::locale("pl_PL.UTF-8"));

    // Inicjalizacja biblioteki cURL
    curl_global_init(CURL_GLOBAL_DEFAULT);
}

AirQualityAPI::~AirQualityAPI() {
    // Zwolnienie zasobów cURL
    curl_global_cleanup();
}

bool AirQualityAPI::performCurlRequest(const std::string& url, std::string& response) {
    // Dodajemy ponowne próby w przypadku b³êdu
 
    int retries = 3;
    while (retries > 0) {
        CURL* curl = curl_easy_init();
        if (!curl) {
            std::cerr << "Nie uda³o siê zainicjowaæ biblioteki CURL" << std::endl;
            return false;
        }

        // Czyszczenie poprzedniej odpowiedzi
        response.clear();

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L); // 10 sekund timeout
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

        CURLcode res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        if (res == CURLE_OK) {
            return true;
        }

        std::cerr << "B³¹d podczas wykonywania ¿¹dania: " << curl_easy_strerror(res) << std::endl;
        retries--;

        if (retries > 0) {
            std::cerr << "Ponawiam próbê... (pozosta³o prób: " << retries << ")" << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(2)); // Czekaj 2 sekundy przed ponown¹ prób¹
        }
    }

    return false;
}

bool AirQualityAPI::parseJsonResponse(const std::string& jsonResponse, Json::Value& parsedRoot) {
    Json::CharReaderBuilder builder;
    JSONCPP_STRING errs;

    const std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
    return reader->parse(jsonResponse.c_str(), jsonResponse.c_str() + jsonResponse.size(), &parsedRoot, &errs);
}

Json::Value AirQualityAPI::getAllStations() {
    Json::Value result;
    std::string url = "https://api.gios.gov.pl/pjp-api/rest/station/findAll";
    std::string response;

    try {
        if (performCurlRequest(url, response)) {
            if (!parseJsonResponse(response, result)) {
                std::cerr << "Nie uda³o siê sparsowaæ danych stacji" << std::endl;
            }
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Wyj¹tek w metodzie getAllStations: " << e.what() << std::endl;
    }

    return result;
}

Json::Value AirQualityAPI::getStationsByCity(const std::string& cityName) {
    Json::Value allStations = getAllStations();
    Json::Value filteredStations(Json::arrayValue);

    try {
        for (const auto& station : allStations) {
            if (!station["city"].isNull() &&
                !station["city"]["name"].isNull() &&
                station["city"]["name"].asString().find(cityName) != std::string::npos) {
                filteredStations.append(station);
            }
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Wyj¹tek w metodzie getStationsByCity: " << e.what() << std::endl;
    }

    return filteredStations;
}

Json::Value AirQualityAPI::getSensorsByStation(int stationId) {
    Json::Value result;
    std::string url = "https://api.gios.gov.pl/pjp-api/rest/station/sensors/" + std::to_string(stationId);
    std::string response;

    try {
        if (performCurlRequest(url, response)) {
            if (!parseJsonResponse(response, result)) {
                std::cerr << "Nie uda³o siê sparsowaæ danych czujników" << std::endl;
            }
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Wyj¹tek w metodzie getSensorsByStation: " << e.what() << std::endl;
    }

    return result;
}

Json::Value AirQualityAPI::getMeasurementData(int sensorId) {
    Json::Value result;
    std::string url = "https://api.gios.gov.pl/pjp-api/rest/data/getData/" + std::to_string(sensorId);
    std::string response;

    try {
        if (performCurlRequest(url, response)) {
            if (!parseJsonResponse(response, result)) {
                std::cerr << "Nie uda³o siê sparsowaæ danych pomiarowych" << std::endl;
            }
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Wyj¹tek w metodzie getMeasurementData: " << e.what() << std::endl;
    }

    return result;
}

Json::Value AirQualityAPI::getAirQualityIndex(int stationId) {
    Json::Value result;
    std::string url = "https://api.gios.gov.pl/pjp-api/rest/aqindex/getIndex/" + std::to_string(stationId);
    std::string response;

    try {
        if (performCurlRequest(url, response)) {
            if (!parseJsonResponse(response, result)) {
                std::cerr << "Nie uda³o siê sparsowaæ indeksu jakoœci powietrza" << std::endl;
            }
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Wyj¹tek w metodzie getAirQualityIndex: " << e.what() << std::endl;
    }

    return result;
}

void AirQualityAPI::saveJsonToFile(const Json::Value& root, const std::string& filename) {
    std::ofstream file(filename, std::ios::out | std::ios::trunc);
    if (!file.is_open()) {
        std::cerr << "B³¹d otwierania pliku do zapisu: " << filename << std::endl;
        return;
    }

    Json::StreamWriterBuilder writer;
    writer["emitUTF8"] = true;

    std::unique_ptr<Json::StreamWriter> jsonWriter(writer.newStreamWriter());
    jsonWriter->write(root, &file);

    file.close();
    std::cout << "Dane zapisane do " << filename << std::endl;
}

Json::Value AirQualityAPI::readJsonFromFile(const std::string& filename) {
    Json::Value root;
    std::ifstream file(filename, std::ios::in);
    if (!file.is_open()) {
        std::cerr << "B³¹d otwierania pliku: " << filename << std::endl;
        return root;
    }

    Json::CharReaderBuilder builder;
    JSONCPP_STRING errs;

    if (!Json::parseFromStream(builder, file, &root, &errs)) {
        std::cerr << "B³¹d parsowania pliku JSON: " << errs << std::endl;
    }

    file.close();
    return root;
}


Json::Value AirQualityAPI::getAllSensors() {
    Json::Value result(Json::arrayValue);
    Json::Value stations = getAllStations();

    try {
        // Iteruj przez wszystkie stacje
        for (const auto& station : stations) {
            if (!station["id"].isNull()) {
                int stationId = station["id"].asInt();

                // Pobierz czujniki dla stacji
                Json::Value sensors = getSensorsByStation(stationId);

                // Dodaj ID stacji do ka¿dego czujnika dla ³atwiejszego filtrowania
                for (auto& sensor : sensors) {
                    sensor["stationId"] = stationId;
                    result.append(sensor);
                }
            }
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Wyj¹tek w metodzie getAllSensors: " << e.what() << std::endl;
    }

    return result;
}

Json::Value AirQualityAPI::getAllMeasurementData() {
    Json::Value result(Json::objectValue);
    Json::Value allSensors = getAllSensors();
    int processedSensors = 0;

    try {
        
        for (const auto& sensor : allSensors) {


            if (!sensor["id"].isNull()) {
                int sensorId = sensor["id"].asInt();

                // Pobierz dane pomiarowe dla danego czujnika
                Json::Value measurementData = getMeasurementData(sensorId);

                // Jeœli uda³o siê pobraæ dane i zawieraj¹ wartoœci, dodaj do wyników
                if (!measurementData.isNull() && !measurementData["values"].isNull() && measurementData["values"].size() > 0) {
                    // U¿ywamy ID czujnika jako klucza w obiekcie JSON
                    result[std::to_string(sensorId)] = measurementData;
                    processedSensors++;
                }
            }
        }

        // Zapisz zbiorcze dane do pliku

    }
    catch (const std::exception& e) {
        std::cerr << "Wyj¹tek w metodzie getAllMeasurementData: " << e.what() << std::endl;
    }

    return result;
}

#pragma once

#include <string>
#include <json/json.h>

/**
 * @brief Klasa obs�uguj�ca przechowywanie i odczyt danych w formacie JSON
 *
 * Klasa zapewnia metody do zapisu danych JSON do plik�w, odczytu danych z plik�w,
 * oraz pobierania informacji o stacjach pomiarowych z API.
 */
class DataStorage {
private:
    /**
     * @brief �cie�ka do katalogu z danymi
     */
    std::string dataPath;

    /**
     * @brief Funkcja zwrotna dla biblioteki cURL do zapisywania odpowiedzi
     *
     * @param contents Wska�nik do tre�ci otrzymanej odpowiedzi
     * @param size Rozmiar ka�dego elementu
     * @param nmemb Liczba element�w
     * @param userp Wska�nik do danych u�ytkownika (bufora odpowiedzi)
     * @return Liczba przetworzonych bajt�w
     */
    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp);

    /**
     * @brief Wykonuje ��danie HTTP za pomoc� cURL
     *
     * @param url Adres URL do zapytania
     * @param response Referencja do zmiennej przechowuj�cej odpowied�
     * @return true je�li ��danie zako�czy�o si� sukcesem, false w przeciwnym razie
     */
    bool performCurlRequest(const std::string& url, std::string& response);

    /**
     * @brief Parsuje odpowied� JSON
     *
     * @param jsonResponse Odpowied� w formacie JSON
     * @param parsedRoot Referencja do obiektu Json::Value, kt�ry b�dzie zawiera� sparsowan� odpowied�
     * @return true je�li parsowanie powiod�o si�, false w przeciwnym razie
     */
    bool parseJsonResponse(const std::string& jsonResponse, Json::Value& parsedRoot);

public:
    /**
     * @brief Konstruktor klasy DataStorage
     *
     * @param path �cie�ka do katalogu, gdzie maj� by� przechowywane dane (domy�lnie "data/")
     */
    DataStorage(const std::string& path = "data/");

    /**
     * @brief Zapisuje dane JSON do pliku
     *
     * @param data Dane w formacie Json::Value do zapisania
     * @param filename Nazwa pliku (bez �cie�ki)
     * @return true je�li zapis powi�d� si�, false w przeciwnym razie
     */

    Json::Value loadJsonData(const std::string& filename);

    /**
     * @brief Sprawdza czy dane istniej� w pliku
     *
     * @param filename Nazwa pliku (bez �cie�ki)
     * @return true je�li plik istnieje, false w przeciwnym razie
     */
    bool dataExists(const std::string& filename);

    /**
     * @brief Zapisuje dane JSON do pliku
     *
     * @param root Dane w formacie Json::Value do zapisania
     * @param filename Nazwa pliku (pe�na �cie�ka)
     */
    void saveJsonToFile(const Json::Value& root, const std::string& filename);

    /**
     * @brief Pobiera dane o wszystkich stacjach pomiarowych i zapisuje do pliku
     */
    void getStationData();
};
#pragma once

#include <string>
#include <curl/curl.h>
#include <json/json.h>
#include <memory>
#include <stdexcept>

/**
 * @brief Klasa do obsługi API GIOŚ - Głównego Inspektoratu Ochrony Środowiska
 *
 * Klasa zapewnia metody do pobierania danych o jakości powietrza z API GIOŚ,
 * w tym stacje pomiarowe, czujniki, pomiary oraz indeksy jakości powietrza.
 */
class AirQualityAPI {
private:
    /**
     * @brief Funkcja zwrotna do obsługi odpowiedzi cURL
     *
     * @param contents Wskaźnik do treści otrzymanej odpowiedzi
     * @param size Rozmiar każdego elementu
     * @param nmemb Liczba elementów
     * @param userp Wskaźnik do danych użytkownika (bufora odpowiedzi)
     * @return Liczba przetworzonych bajtów
     */
    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp);

    /**
     * @brief Wykonuje żądanie HTTP za pomocą cURL
     *
     * @param url Adres URL do zapytania
     * @param response Referencja do zmiennej przechowującej odpowiedź
     * @return true jeśli żądanie zakończyło się sukcesem, false w przeciwnym razie
     */
    bool performCurlRequest(const std::string& url, std::string& response);

    /**
     * @brief Parsuje odpowiedź JSON
     *
     * @param jsonResponse Odpowiedź w formacie JSON
     * @param parsedRoot Referencja do obiektu Json::Value, który będzie zawierać sparsowaną odpowiedź
     * @return true jeśli parsowanie powiodło się, false w przeciwnym razie
     */
    bool parseJsonResponse(const std::string& jsonResponse, Json::Value& parsedRoot);

public:
    /**
     * @brief Konstruktor, inicjalizuje bibliotekę cURL
     */
    AirQualityAPI();

    /**
     * @brief Destruktor, zwalnia zasoby biblioteki cURL
     */
    ~AirQualityAPI();

    /**
     * @brief Pobiera wszystkie stacje pomiarowe
     *
     * @return Obiekt Json::Value zawierający dane wszystkich stacji
     */
    Json::Value getAllStations();

    /**
     * @brief Pobiera wszystkie czujniki dla wszystkich stacji
     *
     * @return Obiekt Json::Value zawierający dane wszystkich czujników
     */
    Json::Value getAllSensors();

    /**
     * @brief Pobiera stacje pomiarowe dla podanego miasta
     *
     * @param cityName Nazwa miasta
     * @return Obiekt Json::Value zawierający dane stacji w danym mieście
     */
    Json::Value getStationsByCity(const std::string& cityName);

    /**
     * @brief Pobiera czujniki dla danej stacji pomiarowej
     *
     * @param stationId Identyfikator stacji pomiarowej
     * @return Obiekt Json::Value zawierający dane czujników
     */
    Json::Value getSensorsByStation(int stationId);

    /**
     * @brief Pobiera dane pomiarowe dla danego czujnika
     *
     * @param sensorId Identyfikator czujnika
     * @return Obiekt Json::Value zawierający dane pomiarowe
     */
    Json::Value getMeasurementData(int sensorId);

    /**
     * @brief Pobiera indeks jakości powietrza dla danej stacji
     *
     * @param stationId Identyfikator stacji pomiarowej
     * @return Obiekt Json::Value zawierający indeks jakości powietrza
     */
    Json::Value getAirQualityIndex(int stationId);

    /**
     * @brief Zapisuje dane JSON do pliku
     *
     * @param root Dane w formacie Json::Value do zapisania
     * @param filename Nazwa pliku do zapisu
     */
    void saveJsonToFile(const Json::Value& root, const std::string& filename);


    /**
     * @brief Pobiera dane pomiarowe dla wszystkich czujników
     *
     * @param maxSensors Maksymalna liczba czujników, dla których pobierane są dane (domyślnie 20)
     * @return Obiekt Json::Value zawierający dane pomiarowe dla wszystkich czujników
     */
    Json::Value getAllMeasurementData();


    /**
     * @brief Wczytuje dane JSON z pliku
     *
     * @param filename Nazwa pliku do wczytania
     * @return Obiekt Json::Value zawierający wczytane dane
     */
    Json::Value readJsonFromFile(const std::string& filename);
};


#pragma once

#include <string>
#include <json/json.h>

/**
 * @brief Klasa obs³uguj¹ca przechowywanie i odczyt danych w formacie JSON
 *
 * Klasa zapewnia metody do zapisu danych JSON do plików, odczytu danych z plików,
 * oraz pobierania informacji o stacjach pomiarowych z API.
 */
class DataStorage {
private:
    /**
     * @brief Œcie¿ka do katalogu z danymi
     */
    std::string dataPath;

    /**
     * @brief Funkcja zwrotna dla biblioteki cURL do zapisywania odpowiedzi
     *
     * @param contents WskaŸnik do treœci otrzymanej odpowiedzi
     * @param size Rozmiar ka¿dego elementu
     * @param nmemb Liczba elementów
     * @param userp WskaŸnik do danych u¿ytkownika (bufora odpowiedzi)
     * @return Liczba przetworzonych bajtów
     */
    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp);

    /**
     * @brief Wykonuje ¿¹danie HTTP za pomoc¹ cURL
     *
     * @param url Adres URL do zapytania
     * @param response Referencja do zmiennej przechowuj¹cej odpowiedŸ
     * @return true jeœli ¿¹danie zakoñczy³o siê sukcesem, false w przeciwnym razie
     */
    bool performCurlRequest(const std::string& url, std::string& response);

    /**
     * @brief Parsuje odpowiedŸ JSON
     *
     * @param jsonResponse OdpowiedŸ w formacie JSON
     * @param parsedRoot Referencja do obiektu Json::Value, który bêdzie zawieraæ sparsowan¹ odpowiedŸ
     * @return true jeœli parsowanie powiod³o siê, false w przeciwnym razie
     */
    bool parseJsonResponse(const std::string& jsonResponse, Json::Value& parsedRoot);

public:
    /**
     * @brief Konstruktor klasy DataStorage
     *
     * @param path Œcie¿ka do katalogu, gdzie maj¹ byæ przechowywane dane (domyœlnie "data/")
     */
    DataStorage(const std::string& path = "data/");

    /**
     * @brief Zapisuje dane JSON do pliku
     *
     * @param data Dane w formacie Json::Value do zapisania
     * @param filename Nazwa pliku (bez œcie¿ki)
     * @return true jeœli zapis powiód³ siê, false w przeciwnym razie
     */

    Json::Value loadJsonData(const std::string& filename);

    /**
     * @brief Sprawdza czy dane istniej¹ w pliku
     *
     * @param filename Nazwa pliku (bez œcie¿ki)
     * @return true jeœli plik istnieje, false w przeciwnym razie
     */
    bool dataExists(const std::string& filename);

    /**
     * @brief Zapisuje dane JSON do pliku
     *
     * @param root Dane w formacie Json::Value do zapisania
     * @param filename Nazwa pliku (pe³na œcie¿ka)
     */
    void saveJsonToFile(const Json::Value& root, const std::string& filename);

    /**
     * @brief Pobiera dane o wszystkich stacjach pomiarowych i zapisuje do pliku
     */
    void getStationData();
};
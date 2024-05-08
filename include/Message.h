#pragma once

#include <memory>
#include <string>

/// @brief Typy wiadomości
enum class MessageType : uint32_t {
    error = 0,
    serverPort = 1,
    clientList = 2,
    fileListRequest = 3,
    fileListResponse = 4,
    fileRequest = 5,
    fileResponse = 6,
};

/// @brief Klasa reprezentująca wiadomość
class Message {

public:

    /// @brief Długość nagłówka
    static const uint32_t HeaderLength = 16;

    /// @brief Maksymalna długość wiadomości
    static const uint32_t MaxLength = 1024;

    /// @brief Maksymalna długość danych
    static const uint32_t MaxDataLength = MaxLength - HeaderLength;

    /// @brief Pozycja długości danych
    static const uint32_t DataLengthOffset = 0;

    /// @brief Pozycja numeru segmentu
    static const uint32_t SegmentIndexOffset = 4;

    /// @brief Pozycja ilości segmentów
    static const uint32_t SegmentTotalOffset = 8;

    /// @brief Pozycja typu wiadomości
    static const uint32_t TypeOffset = 12;

    /// @brief Pozycja danych
    static const uint32_t DataOffset = 16;


    /// @brief Konstruktor
    /// @param segmentIndex index segmentu
    /// @param segmentTotal ilość segmentów
    /// @param type typ wiadomości
    /// @param data dane
    /// @param dataLength długość danych
    Message(uint32_t segmentIndex, uint32_t segmentTotal, uint32_t type, char* data, uint32_t dataLength);

    /// @brief Konstruktor
    /// @param buffer bufor

    Message(uint32_t segmentIndex, uint32_t segmentTotal, uint32_t type, std::string data);

    /// @brief Konstruktor
    /// @param buffer bufor
    Message(char* buffer);

    /// @brief Destruktor
    virtual ~Message();
    

    /// @brief Zwraca numer segmentu
    /// @return numer segmentu
    inline uint32_t GetSegmentIndex() const { return m_segmentIndex; }

    /// @brief Zwraca ilość segmentów
    /// @return ilość segmentów
    inline uint32_t GetSegmentTotal() const { return m_segmentTotal; }

    /// @brief Zwraca typ wiadomości
    /// @return typ wiadomości
    inline uint32_t GetType() const { return m_type; }

    /// @brief Zwraca dane
    /// @return dane
    inline char* GetData() const { return m_data; }

    /// @brief Zwraca długość danych
    /// @return długość danych
    inline uint32_t GetDataLength() const { return m_dataLength; }

    /// @brief Zwraca długość wiadomości
    /// @return długość wiadomości
    inline uint32_t GetLength() const { return HeaderLength + m_dataLength; }


    /// @brief Zwraca wiadomość jako ciąg bajtów
    /// @return ciąg bajtów
    char* ToBytes();

    /// @brief Konwertuje uint32_t na ciąg bajtów
    /// @param value wartość
    /// @return ciąg 4 bajtów
    static char* Uint32ToBytes(uint32_t value);

    /// @brief Konwertuje 4 bajty na uint32_t
    /// @param buffer bufor
    /// @return wartość uint32_t
    static uint32_t BytesToUint32(char* buffer);

private:
    /// @brief Długość danych
    uint32_t m_dataLength;

    /// @brief Numer segmentu
    uint32_t m_segmentIndex;

    /// @brief Ilość segmentów
    uint32_t m_segmentTotal;

    /// @brief Typ wiadomości
    uint32_t m_type;

    /// @brief Dane
    char* m_data;

};

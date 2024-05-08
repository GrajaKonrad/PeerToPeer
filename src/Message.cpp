#include "../include/Message.h"

#include <iostream>

Message::Message(uint32_t segmentIndex, uint32_t segmentTotal, uint32_t type, char* data, uint32_t dataLength) :
    m_segmentIndex(segmentIndex),
    m_segmentTotal(segmentTotal),
    m_type(type),
    m_dataLength(dataLength),
    m_data(data) {}

Message::Message(uint32_t segmentIndex, uint32_t segmentTotal, uint32_t type, std::string data) :
    m_segmentIndex(segmentIndex),
    m_segmentTotal(segmentTotal),
    m_type(type) {
        m_dataLength = data.length();
        m_data = new char[m_dataLength];
        for (int i = 0; i < data.length(); i++) {
            m_data[i] = data[i];
        }
    }

Message::Message(char* buffer) {
    m_dataLength = BytesToUint32(buffer + DataLengthOffset);
    m_segmentIndex = BytesToUint32(buffer + SegmentIndexOffset);
    m_segmentTotal = BytesToUint32(buffer + SegmentTotalOffset);
    m_type = BytesToUint32(buffer + TypeOffset);
    m_data = new char[m_dataLength];
    for (int i = 0; i < m_dataLength; i++) {
        m_data[i] = buffer[HeaderLength + i];
    }
}

Message::~Message() {
    // delete[] m_data;
}

char *Message::ToBytes()
{
    char* buffer = new char[HeaderLength + m_dataLength];
    char* lengthBytes = Uint32ToBytes(m_dataLength);
    char* segmentNumberBytes = Uint32ToBytes(m_segmentIndex);
    char* segmentTotalBytes = Uint32ToBytes(m_segmentTotal);
    char* typeBytes = Uint32ToBytes(m_type);
    for (int i = 0; i < 4; i++) {
        buffer[DataLengthOffset + i] = lengthBytes[i];
    }
    for (int i = 0; i < 4; i++) {
        buffer[SegmentIndexOffset + i] = segmentNumberBytes[i];
    }
    for (int i = 0; i < 4; i++) {
        buffer[SegmentTotalOffset + i] = segmentTotalBytes[i];
    }
    for (int i = 0; i < 4; i++) {
        buffer[TypeOffset + i] = typeBytes[i];
    }
    for (int i = 0; i < m_dataLength; i++) {
        buffer[HeaderLength + i] = m_data[i];
    }
    return buffer;
}

char* Message::Uint32ToBytes(uint32_t value) {
    char* buffer = new char[4];
    buffer[0] = (value >> 24) & 0xFF;
    buffer[1] = (value >> 16) & 0xFF;
    buffer[2] = (value >> 8) & 0xFF;
    buffer[3] = value & 0xFF;
    return buffer;
}

uint32_t Message::BytesToUint32(char* buffer) {
    return ((buffer[0] & 0xFF) << 24) | ((buffer[1] & 0xFF) << 16) | ((buffer[2] & 0xFF) << 8) | (buffer[3] & 0xFF);
}
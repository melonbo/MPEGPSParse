#include "PSStreamReader.h"


PSStreamReader::PSStreamReader(const std::string& filePath) : file(filePath, std::ios::binary) {
    if (!file.is_open()) {
        throw std::runtime_error("Unable to open file");
    }
}

PSHeader PSStreamReader::readPackHeader() {
    PSHeader header;

    // 读取 Pack Start Code
    header.pack_start_code = readBits(32);
    if (header.pack_start_code != 0x000001BA) {
        throw std::invalid_argument("Invalid Pack Start Code");
    }

    // 读取 2 位
    readBits(2);

    // 读取 System Clock Reference Base (SCR)
    header.system_clock_reference_base = readBits(30); // 30 bits
    readBits(1); // Marker bit
    header.system_clock_reference_base = (header.system_clock_reference_base << 15) | readBits(15); // 15 bits
    readBits(1); // Marker bit
    header.system_clock_reference_base = (header.system_clock_reference_base << 15) | readBits(15); // 15 bits

    // 读取 System Clock Reference Extension
    readBits(1); // Marker bit
    header.system_clock_reference_extension = readBits(9); // 9 bits

    // 读取 Program Mux Rate
    readBits(1); // Marker bit
    header.program_mux_rate = readBits(22); // 22 bits

    // 读取 Pack Stuffing Length
    readBits(1); // Marker bit
    readBits(1); // Marker bit
    readBits(5); // Reserved
    header.pack_stuffing_length = readBits(3); // 3 bits

    // 读取 Stuffing Bytes
    for (uint8_t i = 0; i < header.pack_stuffing_length; ++i) {
        header.stuffing_bytes.push_back(readBits(8)); // 8 bits
    }

    return header;
}

uint32_t PSStreamReader::readBits(int numBits) {
    uint32_t value = 0;
    int bitsRead = 0;

    while (bitsRead < numBits) {
        // 确保还有字节可以读取
        if (file.eof()) {
            throw std::runtime_error("End of file reached");
        }

        // 读取字节
        unsigned char byte;
        file.read(reinterpret_cast<char*>(&byte), 1);
        if (!file) {
            throw std::runtime_error("Failed to read byte");
        }

        // 将读取的字节中的位存入 value
        for (int bit = 7; bit >= 0 && bitsRead < numBits; --bit, ++bitsRead) {
            value = (value << 1) | ((byte >> bit) & 0x01);
        }
    }

    return value;
}

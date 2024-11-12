#include <iostream>
#include <fstream>
#include <vector>
#include <stdexcept>

class PSHeader {
public:
    uint32_t pack_start_code;
    uint32_t system_clock_reference_base;
    uint32_t system_clock_reference_extension;
    uint32_t program_mux_rate;
    uint8_t pack_stuffing_length;
    std::vector<uint8_t> stuffing_bytes;

    void print() const {
        std::cout << "Pack Start Code: " << std::hex << pack_start_code << std::dec << "\n"
                  << "System Clock Reference Base: " << system_clock_reference_base << "\n"
                  << "System Clock Reference Extension: " << system_clock_reference_extension << "\n"
                  << "Program Mux Rate: " << program_mux_rate << "\n"
                  << "Pack Stuffing Length: " << static_cast<int>(pack_stuffing_length) << "\n";
        if (!stuffing_bytes.empty()) {
            std::cout << "Stuffing Bytes: ";
            for (const auto &byte : stuffing_bytes) {
                std::cout << std::hex << static_cast<int>(byte) << " ";
            }
            std::cout << std::dec << "\n";
        }
    }
};

class PSStreamReader {
public:
    PSStreamReader(const std::string& filePath);

    PSHeader readPackHeader();

private:
    std::ifstream file;

    uint32_t readBits(int numBits);
};

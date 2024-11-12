#ifndef PSSTREAM_H
#define PSSTREAM_H
#include <QByteArray>
#include <QDataStream>
#include <iostream>
#include <vector>
#include <iomanip>
#define VNAME(name) (#name)

struct PSHeader {
    uint32_t pack_start_code;

    uint32_t reserved_01 : 2;//01
    uint32_t system_clock_reference_base_32_30 : 3;
    uint32_t marker_bit_01 : 1;
    uint32_t system_clock_reference_base_29_15 : 15;
    uint32_t marker_bit_02 : 1;
    uint32_t system_clock_reference_base_14_0  : 15;
    uint32_t marker_bit_03 : 1;
    uint32_t system_clock_reference_extension : 9;
    uint32_t marker_bit_04 : 1;

    uint32_t program_mux_rate : 22;
    uint32_t marker_bit_05 : 1;
    uint32_t marker_bit_06 : 1;
    uint32_t reserved_02 : 5;
    uint32_t pack_stuffing_length;

    std::vector<uint8_t> stuffing_bytes;

    void print() const {
        std::cout << "Pack Start Code: " << std::setw(8) << std::setfill('0') << std::hex << pack_start_code << std::dec << "\n"
                  << "reserved_01: " << std::hex << reserved_01 << "\n"
                  << "System Clock Reference Base [32..30]: " << std::hex << system_clock_reference_base_32_30 << "\n"
                  << "marker_bit_01: " << std::hex << marker_bit_01 << "\n"
                  << "System Clock Reference Base [29..15]: " << std::hex << system_clock_reference_base_29_15 << "\n"
                  << "marker_bit_02: " << std::hex << marker_bit_02 << "\n"
                  << "System Clock Reference Base [14..0]: " << std::hex << system_clock_reference_base_14_0 << "\n"
                  << "marker_bit_03: " << std::hex << marker_bit_03 << "\n"
                  << "System Clock Reference Extension: " << std::hex << system_clock_reference_extension << "\n"
                  << "marker_bit_04: " << std::hex << marker_bit_04 << "\n"
                  << "Program Mux Rate: " << std::hex << program_mux_rate << "\n"
                  << "marker_bit_05: " << std::hex << marker_bit_05 << "\n"
                  << "marker_bit_06: " << std::hex << marker_bit_06 << "\n"
                  << "reserved_02: " << std::hex << reserved_02 << "\n"
                  << "Pack Stuffing Length: " << static_cast<int>(pack_stuffing_length) << "\n";
        if (!stuffing_bytes.empty()) {
            std::cout << "Stuffing Bytes: ";
            for (const auto &byte : stuffing_bytes) {
                std::cout << std::setw(2) << std::setfill('0') << std::hex << static_cast<int>(byte) << " ";
            }
            std::cout << std::dec << "\n";
        }

        std::cout << "\n";
    }
};

struct StreamInfo {
    uint8_t stream_id;
    uint8_t reserved;
    uint8_t P_STD_buffer_bound_scale;
    uint16_t P_STD_buffer_size_bound;

    void print() const {
        std::cout << "Stream ID: " << static_cast<int>(stream_id) << "\n"
                  << "P-STD Buffer Bound Scale: " << static_cast<int>(P_STD_buffer_bound_scale) << "\n"
                  << "P-STD Buffer Size Bound: " << P_STD_buffer_size_bound << "\n";
    }
};

struct SYSHeader {
    uint32_t  system_header_start_code;
    uint32_t  header_length;
    uint32_t marker_bit_01 : 1;
    uint32_t rate_bound : 22;
    uint32_t marker_bit_02 : 1;
    uint32_t audio_bound : 6;
    uint32_t fixed_flag : 1;
    uint32_t CSPS_flag : 1;
    uint32_t system_audio_lock_flag : 1;
    uint32_t system_video_lock_flag : 1;
    uint32_t marker_bit_03 : 1;
    uint32_t video_bound : 5;
    uint32_t packet_rate_restriction_flag : 1;
    uint32_t reserved_bits : 7;
    std::vector<StreamInfo> streams;

    void print() const {
        std::cout << "System Header Start Code: " << std::setw(8) << std::setfill('0') << std::hex << system_header_start_code << std::dec << "\n"
                  << "Header Length: " << header_length << "\n"
                  << "Rate Bound: " << rate_bound << "\n"
                  << "Audio Bound: " << static_cast<int>(audio_bound) << "\n"
                  << "Fixed Flag: " << fixed_flag << "\n"
                  << "CSPS Flag: " << CSPS_flag << "\n"
                  << "System Audio Lock Flag: " << system_audio_lock_flag << "\n"
                  << "System Video Lock Flag: " << system_video_lock_flag << "\n"
                  << "Video Bound: " << static_cast<int>(video_bound) << "\n"
                  << "Packet Rate Restriction Flag: " << packet_rate_restriction_flag << "\n"
                  << "Reserved Bits: " << static_cast<int>(reserved_bits) << "\n";

        for (const auto& stream : streams) {
            stream.print();
        }

        std::cout << "\n";
    }
};

struct Descriptor {
    // Placeholder for descriptor fields
    // Depending on your descriptor structure, you can define appropriate fields
    uint8_t descriptor_tag;
    uint8_t descriptor_length;

    void print() const {
        std::cout << "Descriptor Tag: " << static_cast<int>(descriptor_tag)
                  << ", Descriptor Length: " << static_cast<int>(descriptor_length) << "\n";
    }
};

struct ElementaryStream {
    uint8_t stream_type;
    uint8_t elementary_stream_id;
    uint16_t elementary_stream_info_length;
    std::vector<Descriptor> descriptors;

    void print() const {
        std::cout << "Stream Type: " << static_cast<int>(stream_type) << "\n"
                  << "Elementary Stream ID: " << static_cast<int>(elementary_stream_id) << "\n"
                  << "Elementary Stream Info Length: " << elementary_stream_info_length << "\n";
        for (const auto& desc : descriptors) {
            desc.print();
        }
    }
};

struct PSMHeader {
    uint32_t packet_start_code_prefix;
    uint8_t map_stream_id;
    uint16_t program_stream_map_length;
    bool current_next_indicator;
    uint8_t reserved_01 : 2;
    uint8_t program_stream_map_version : 5;
    uint8_t reserved_02 : 7;
    bool marker_bit;
    uint16_t program_stream_info_length;

    std::vector<Descriptor> program_descriptors;

    uint16_t elementary_stream_map_length;

    std::vector<ElementaryStream> elementary_streams;
    uint32_t crc;

    void print() const {
        std::cout << "Packet Start Code Prefix: " << std::setw(6) << std::setfill('0') << std::hex << packet_start_code_prefix << "\n"
                  << "Map Stream ID: " << std::hex << static_cast<int>(map_stream_id) << "\n"
                  << "Program Stream Map Length: " << program_stream_map_length << "\n"
                  << "Current Next Indicator: " << current_next_indicator << "\n"
                  << "Reserved: " << static_cast<int>(reserved_01) << "\n"
                  << "Program Stream Map Version: " << static_cast<int>(program_stream_map_version) << "\n"
                  << "Reserved: " << static_cast<int>(reserved_02) << "\n"
                  << "Marker Bit: " << static_cast<int>(marker_bit) << "\n"
                  << "Program Stream Info Length: " << program_stream_info_length << "\n";

        for (const auto& desc : program_descriptors) {
            desc.print();
        }

        std::cout << "Elementary Stream Map Length: " << elementary_stream_map_length << "\n";

        for (const auto& stream : elementary_streams) {
            stream.print();
        }

        std::cout << "CRC: " << std::hex << crc << std::dec << "\n\n";
    }
};

struct PESHeader {
    /* Common */
    uint32_t packet_start_code_prefix:24;
    uint8_t stream_id:8;
    uint16_t PES_packet_length:16;
    uint8_t PES_scrambling_control:2;
    uint8_t PES_priority:1;
    uint8_t data_alignment_indicator:1;
    uint8_t copyright:1;
    uint8_t original_or_copy:1;
    uint8_t PTS_DTS_flags:2;
    uint8_t ESCR_flag:1;
    uint8_t ES_rate_flag:1;
    uint8_t DSM_trick_mode_flag:1;
    uint8_t additional_copy_info_flag:1;
    uint8_t PES_CRC_flag:1;
    uint8_t PES_extension_flag:1;
    uint8_t PES_header_data_length:8;
    uint8_t PTS_32_30:3;
    uint16_t PTS_29_15:15;
    uint16_t PTS_14_0:15;
    uint8_t DTS_32_30:3;
    uint16_t DTS_29_15:15;
    uint16_t DTS_14_0:15;
    /* Timestamps */
    uint64_t PTS;
    uint64_t DTS;
    /* ESCR */
    uint64_t ESCR_base;
    uint16_t ESCR_ext;
    uint32_t ES_rate:22;
    /* Trick control */
    uint8_t trick_mode_control:3;
    uint8_t field_id:2;
    uint8_t intra_slice_refresh:1;
    uint8_t frequency_truncation:2;
    uint8_t rep_cntrl:5;
    /* Additional */
    uint8_t additional_copy_info:7;
    uint16_t previous_PES_packet_CRC:16;
    uint8_t PES_private_data_flag:1;
    uint8_t pack_header_field_flag:1;
    uint8_t program_packet_sequence_counter_flag:1;
    uint8_t P_STD_buffer_flag:1;
    uint8_t PES_extension_flag_2:1;
    uint8_t private_data[16];
    uint8_t pack_field_length;
    uint8_t pack_field[256];
    uint8_t program_packet_sequence_counter:7;
    uint8_t MPEG1_MPEG2_identifier:1;
    uint8_t original_stuff_length:6;
    uint8_t P_STD_buffer_scale:1;
    uint16_t P_STD_buffer_size:13;
    uint8_t PES_extension_field_length:7;
    uint8_t PES_extension_field[128];
    /* Data and size */
    const char* data;
    uint32_t data_size;


    // Optional fields
    bool has_pts;
    uint64_t pts;

    // Other fields as needed...
    // Here we can expand to include the other optional fields defined in the PES structure

    void print() const {
        std::cout << "Packet Start Code Prefix: " << std::hex << packet_start_code_prefix << std::dec << "\n"
                  << "Stream ID: " << std::setw(2) << std::hex << static_cast<int>(stream_id) << "\n"
                  << "PES Packet Length: " << std::dec << PES_packet_length << "\n"
                  << "PES_scrambling_control: " << PES_scrambling_control <<"\n"
                  << "PES_priority: " << PES_priority << "\n"
                  << "data_alignment_indicator: " << data_alignment_indicator << "\n"
                  << "copyright: " << copyright << "\n"
                  << "original_or_copy: " << original_or_copy << "\n"
                  << "PTS_DTS_flags: " << std::dec << static_cast<int>(PTS_DTS_flags) << "\n"
                  << "ESCR_flag: " << ESCR_flag << "\n"
                  << "ES_rate_flag: " << ES_rate_flag << "\n"
                  << "DSM_trick_mode_flag: " << DSM_trick_mode_flag << "\n"
                  << "additional_copy_info_flag: " << additional_copy_info_flag << "\n"
                  << "PES_CRC_flag: " << PES_CRC_flag << "\n"
                  << "PES_extension_flag: " << PES_extension_flag << "\n"
                  << "PES_header_data_length: " << PES_header_data_length << "\n";

        if (PTS_DTS_flags == 2)
            std::cout << "PTS: " << PTS << "\n";
        else if (PTS_DTS_flags == 3){
            std::cout << "PTS: " << PTS << "\n";
            std::cout << "DTS: " << DTS << "\n";
        }

        if (ESCR_flag == 1){
            std::cout << "ESCR_ext: " << ESCR_ext << "\n";
            std::cout << "ESCR_base: " << ESCR_base << "\n";
        }

        if (ES_rate_flag == 1){
            std::cout << "ES_rate: " << ES_rate << "\n";
        }

        if (DSM_trick_mode_flag == 1)
        {
            std::cout << "trick_mode_control: " << trick_mode_control << "\n";

            if (trick_mode_control == 0)
            {
                std::cout << "field_id: " << field_id << "\n";
                std::cout << "intra_slice_refresh: " << intra_slice_refresh << "\n";
                std::cout << "frequency_truncation: " << frequency_truncation << "\n";
            }
            else if (trick_mode_control == 1)
            {
                std::cout << "rep_cntrl: " << rep_cntrl << "\n";
            }
            else if (trick_mode_control == 2)
            {
                std::cout << "field_id: " << field_id << "\n";
            }
            else if (trick_mode_control == 3)
            {
                std::cout << "field_id: " << field_id << "\n";
                std::cout << "intra_slice_refresh: " << intra_slice_refresh << "\n";
                std::cout << "frequency_truncation: " << frequency_truncation << "\n";
            }
            else if (trick_mode_control == 4)
            {
                std::cout << "rep_cntrl: " << rep_cntrl << "\n";
            }
        }

        if (additional_copy_info_flag == 1)
        {
            std::cout << "additional_copy_info: " << additional_copy_info << "\n";
        }

        if (PES_CRC_flag == 1)
        {
            std::cout << "previous_PES_packet_CRC: " << previous_PES_packet_CRC << "\n";
        }

        if (PES_extension_flag == 1)
        {
            std::cout << "PES_private_data_flag: " << PES_private_data_flag << "\n";
            std::cout << "pack_header_field_flag: " << pack_header_field_flag << "\n";
            std::cout << "program_packet_sequence_counter_flag: " << program_packet_sequence_counter_flag << "\n";
            std::cout << "P_STD_buffer_flag: " << P_STD_buffer_flag << "\n";
            std::cout << "PES_extension_flag_2: " << PES_extension_flag_2 << "\n";

            if (PES_private_data_flag == 1)
            {
                std::cout << "private_data: ";
                for (int i = 0; i < 16; i++)
                {
                    std::cout << std::setw(2) << std::hex << private_data[i] << " ";
                }
                std::cout << "\n";
            }

            if (pack_header_field_flag == 1)
            {
                std::cout << "pack_field_length: " << pack_field_length << "\n";
                std::cout << "pack_field: ";

                for (int i = 0; i < pack_field_length; i++)
                {
                    std::cout << std::setw(2) << std::hex << pack_field[i] << " ";
                }
                std::cout << "\n";
            }

            if (program_packet_sequence_counter_flag == 1)
            {
                std::cout << "program_packet_sequence_counter: " << program_packet_sequence_counter << "\n";
                std::cout << "MPEG1_MPEG2_identifier: " << MPEG1_MPEG2_identifier << "\n";
                std::cout << "original_stuff_length: " << original_stuff_length << "\n";
            }

            if (P_STD_buffer_flag == 1)
            {
                std::cout << "P_STD_buffer_scale: " << P_STD_buffer_scale << "\n";
                std::cout << "P_STD_buffer_size: " << P_STD_buffer_size << "\n";
            }

            if (PES_extension_flag_2 == 1)
            {
                std::cout << "PES_extension_field_length: " << PES_extension_field_length << "\n";
                std::cout << "PES_extension_field: ";

                for (int i = 0; i < PES_extension_field_length; i++)
                {
                    std::cout << std::setw(2) << std::hex << PES_extension_field[i] << " ";
                }
                std::cout << "\n";
            }
        }
    }
};

class PSStream {
public:
    PSStream(QByteArray byteArray) : data(byteArray), bitPosition(0) {}
    PSHeader readPSHeader();
    SYSHeader readSYSHeader();
    PSMHeader readPSMHeader();
    PESHeader readPESHeader();

    int ps_header_len = 0;
    int sys_header_len = 0;
    int psm_header_len = 0;
    int pes_header_len = 0;

private:
    QByteArray data;
    int bitPosition;

    uint32_t readBits(int bits);
    int nextBits();
    Descriptor readDescriptor();
};

#endif // PSSTREAM_H

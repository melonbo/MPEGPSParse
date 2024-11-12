#include "PSStream.h"
#include <QDebug>

PSHeader PSStream::readPSHeader() {
    PSHeader header;
    header.pack_start_code = readBits(32);
    if (header.pack_start_code != 0x000001BA) {
        bitPosition -= 32;
        throw std::runtime_error("Invalid Pack Start Code");
    }

    header.reserved_01 = readBits(2); // 读取 '01' 2bslbf

    // 读取 system_clock_reference_base
    header.system_clock_reference_base_32_30 = readBits(3);
    header.marker_bit_01 = readBits(1); // marker_bit
    header.system_clock_reference_base_29_15 = readBits(15);
    header.marker_bit_02 = readBits(1); // marker_bit
    header.system_clock_reference_base_14_0 = readBits(15);
    header.marker_bit_03 = readBits(1); // marker_bit

    // 读取 system_clock_reference_extension
    header.system_clock_reference_extension = readBits(9);

    header.marker_bit_04 = readBits(1); // marker_bit
    // 读取 program_mux_rate
    header.program_mux_rate = readBits(22);
    header.marker_bit_05 = readBits(1); // marker_bit
    header.marker_bit_06 = readBits(1); // marker_bit
    header.reserved_02 = readBits(5); // reserved

    // 读取 pack_stuffing_length
    header.pack_stuffing_length = readBits(3);

    // 读取 stuffing bytes
    for (uint8_t i = 0; i < header.pack_stuffing_length; ++i) {
        header.stuffing_bytes.push_back(readBits(8)); // 读取 8 bits
    }

    ps_header_len = 14 + header.pack_stuffing_length;
    return header;
}

SYSHeader PSStream::readSYSHeader()
{
    SYSHeader header;
    header.system_header_start_code = readBits(32);
    if (header.system_header_start_code != 0x000001BB) {
        bitPosition -= 32;
        std::cout << "do not have system header " << std::setw(8) << std::setfill('0') << std::hex << header.system_header_start_code << std::endl;
        return header;
    }

    header.header_length = readBits(16);
    header.marker_bit_01 = readBits(1); // marker_bit
    header.rate_bound = readBits(22);
    header.marker_bit_02 = readBits(1); // marker_bit
    header.audio_bound = readBits(6);
    header.fixed_flag = readBits(1);
    header.CSPS_flag = readBits(1);
    header.system_audio_lock_flag = readBits(1);
    header.system_video_lock_flag = readBits(1);
    header.marker_bit_03 = readBits(1); // marker_bit
    header.video_bound = readBits(5);
    header.packet_rate_restriction_flag = readBits(1);
    header.reserved_bits = readBits(7);

    // 处理 stream 信息
    while (nextBits() == 1) {
        StreamInfo stream;
        stream.stream_id = readBits(8);
        stream.reserved = readBits(2); // '11'
        stream.P_STD_buffer_bound_scale = readBits(1);
        stream.P_STD_buffer_size_bound = readBits(13);
        header.streams.push_back(stream);
    }

    sys_header_len = 12+3*header.streams.size();
    return header;
}

PSMHeader PSStream::readPSMHeader() {
    PSMHeader header;

    header.packet_start_code_prefix = readBits(24);
    header.map_stream_id = readBits(8);
    if(header.packet_start_code_prefix != 1 || header.map_stream_id != 0xBC){
        bitPosition -= 32;
        std::cout << "do not have psm header " << std::setw(8) << std::setfill('0') << std::hex << header.packet_start_code_prefix << std::endl;
        return header;
    }

    header.program_stream_map_length = readBits(16);
    header.current_next_indicator = readBits(1);
    header.reserved_01 = readBits(2); // 2 bits reserved
    header.program_stream_map_version = readBits(5);
    header.reserved_02 = readBits(7); // 7 bits reserved
    header.marker_bit = readBits(1);
    header.program_stream_info_length = readBits(16);

    psm_header_len += 10;

    // Read program stream descriptors
    for (int i = 0; i < header.program_stream_info_length/sizeof(Descriptor); ++i) {
        Descriptor desc = readDescriptor();
        psm_header_len += sizeof(Descriptor);
        header.program_descriptors.push_back(desc);
    }

    header.elementary_stream_map_length = readBits(16);
    psm_header_len += 2;

    // Read elementary streams
    int len = header.elementary_stream_map_length;
    while(len){
        ElementaryStream stream;
        stream.stream_type = readBits(8);len -= 1;
        stream.elementary_stream_id = readBits(8);len -= 1;
        stream.elementary_stream_info_length = readBits(16);len -= 2;
        for(int j=0; j<stream.elementary_stream_info_length/sizeof(Descriptor); j++)
        {
            Descriptor desc = readDescriptor();
            stream.descriptors.push_back(desc);
            len -= sizeof(Descriptor);
        }
        header.elementary_streams.push_back(stream);
    }

    header.crc = readBits(32); // Read CRC
    psm_header_len += 4;

    return header;
}

PESHeader PSStream::readPESHeader() {
    PESHeader header;

    header.packet_start_code_prefix = readBits(24);

    if(header.packet_start_code_prefix != 1){
        bitPosition -= 24;
        std::cout << "do not have pes header " << std::setw(8) << std::setfill('0') << std::hex << header.packet_start_code_prefix << std::endl;
        return header;
    }

    header.stream_id = (uint8_t)readBits(8);
    header.PES_packet_length = (uint16_t)readBits(16);

    readBits(2);
    header.PES_scrambling_control = (uint8_t)readBits(2);
    header.PES_priority = (uint8_t)readBits(1);
    header.data_alignment_indicator = (uint8_t)readBits(1);
    header.copyright = (uint8_t)readBits(1);
    header.original_or_copy = (uint8_t)readBits(1);
    header.PTS_DTS_flags = (uint8_t)readBits(2);
    header.ESCR_flag = (uint8_t)readBits(1);
    header.ES_rate_flag = (uint8_t)readBits(1);
    header.DSM_trick_mode_flag = (uint8_t)readBits(1);
    header.additional_copy_info_flag = (uint8_t)readBits(1);
    header.PES_CRC_flag = (uint8_t)readBits(1);
    header.PES_extension_flag = (uint8_t)readBits(1);
    header.PES_header_data_length = (uint8_t)readBits(8);

    pes_header_len += 9;
    //uint8_t* pExtPtr = &parser.pData[parser.nOffset];
    uint64_t nBuff[3];
    unsigned int i;

    if (header.PTS_DTS_flags == 2)
    {
        nBuff[2] = nBuff[1] = nBuff[0] = 0;
        readBits(4);
        header.PTS_32_30 = nBuff[2] = (uint8_t)readBits(3);
        readBits(1);
        header.PTS_29_15 = nBuff[1] = (uint16_t)readBits(15);
        readBits(1);
        header.PTS_14_0 = nBuff[0] = (uint16_t)readBits(15);
        readBits(1);
        header.PTS = nBuff[0] | (nBuff[1] << 15) | (nBuff[2] << 30);
        pes_header_len += 5;
    }
    else if (header.PTS_DTS_flags == 3)
    {
        nBuff[2] = nBuff[1] = nBuff[0] = 0;
        readBits(4);
        header.PTS_32_30 = nBuff[2] = (uint8_t)readBits(3);
        readBits(1);
        header.PTS_29_15 = nBuff[1] = (uint16_t)readBits(15);
        readBits(1);
        header.PTS_14_0 = nBuff[0] = (uint16_t)readBits(15);
        readBits(1);
        header.PTS = nBuff[0] | (nBuff[1] << 15) | (nBuff[2] << 30);

        nBuff[2] = nBuff[1] = nBuff[0] = 0;
        readBits(4);
        header.DTS_32_30 = nBuff[2] = (uint8_t)readBits(3);
        readBits(1);
        header.DTS_29_15 = nBuff[1] = (uint16_t)readBits(15);
        readBits(1);
        header.DTS_14_0 = nBuff[0] = (uint16_t)readBits(15);
        readBits(1);
        header.DTS = nBuff[0] | (nBuff[1] << 15) | (nBuff[2] << 30);

        pes_header_len += 10;
    }

    if (header.ESCR_flag == 1)
    {
        nBuff[2] = nBuff[1] = nBuff[0] = 0;
        readBits(2);
        nBuff[2] = (uint8_t)readBits(3);
        readBits(1);
        nBuff[1] = (uint16_t)readBits(15);
        readBits(1);
        nBuff[0] = (uint16_t)readBits(15);
        readBits(1);
        header.ESCR_ext = (uint16_t)readBits(9);
        readBits(1);
        header.ESCR_base = nBuff[0] | (nBuff[1] << 15) | (nBuff[2] << 30);

        pes_header_len += 6;
    }

    if (header.ES_rate_flag == 1)
    {
        readBits(1);
        header.ES_rate = (uint32_t)readBits(22);
        readBits(1);

        pes_header_len += 3;
    }

    if (header.DSM_trick_mode_flag == 1)
    {
        header.trick_mode_control = (uint8_t)readBits(3);
        if (header.trick_mode_control == 0)
        {
            header.field_id = (uint8_t)readBits(2);
            header.intra_slice_refresh = (uint8_t)readBits(1);
            header.frequency_truncation = (uint8_t)readBits(2);
        }
        else if (header.trick_mode_control == 1)
        {
            header.rep_cntrl = (uint8_t)readBits(5);
        }
        else if (header.trick_mode_control == 2)
        {
            header.field_id = (uint8_t)readBits(2);
            readBits(3);
        }
        else if (header.trick_mode_control == 3)
        {
            header.field_id = (uint8_t)readBits(2);
            header.intra_slice_refresh = (uint8_t)readBits(1);
            header.frequency_truncation = (uint8_t)readBits(2);
        }
        else if (header.trick_mode_control == 4)
        {
            header.rep_cntrl = (uint8_t)readBits(5);
        }
        else
        {
            readBits(5);
        }

        pes_header_len += 1;
    }

    if (header.additional_copy_info_flag == 1)
    {
        readBits(1);
        header.additional_copy_info = (uint8_t)readBits(7);

        pes_header_len += 2;
    }

    if (header.PES_CRC_flag == 1)
    {
        header.previous_PES_packet_CRC = (uint16_t)readBits(16);

        pes_header_len += 2;
    }

    if (header.PES_extension_flag == 1)
    {
        header.PES_private_data_flag = (uint8_t)readBits(1);
        header.pack_header_field_flag = (uint8_t)readBits(1);
        header.program_packet_sequence_counter_flag = (uint8_t)readBits(1);
        header.P_STD_buffer_flag = (uint8_t)readBits(1);
        readBits(3);
        header.PES_extension_flag_2 = (uint8_t)readBits(1);
        pes_header_len += 1;

        if (header.PES_private_data_flag == 1)
        {
            for (i = 0; i < 16; i++)
            {
                header.private_data[i] = (uint8_t)readBits(8);
            }
            pes_header_len += 16;
        }

        if (header.pack_header_field_flag == 1)
        {
            header.pack_field_length = (uint8_t)readBits(8);
            for (i = 0; i < header.pack_field_length; i++)
            {
                header.pack_field[i] = (uint8_t)readBits(8);
            }
            pes_header_len += 1;
            pes_header_len += header.pack_field_length;
        }

        if (header.program_packet_sequence_counter_flag == 1)
        {
            readBits(1);
            header.program_packet_sequence_counter = (uint8_t)readBits(7);
            readBits(1);
            header.MPEG1_MPEG2_identifier = (uint8_t)readBits(1);
            header.original_stuff_length = (uint8_t)readBits(6);
            pes_header_len += 2;
        }

        if (header.P_STD_buffer_flag == 1)
        {
            readBits(2);
            header.P_STD_buffer_scale = (uint8_t)readBits(1);
            header.P_STD_buffer_size = (uint16_t)readBits(13);
            pes_header_len += 2;
        }

        if (header.PES_extension_flag_2 == 1)
        {
            readBits(1);
            header.PES_extension_field_length = (uint8_t)readBits(7);
            for (i = 0; i < header.PES_extension_field_length; i++)
            {
              header.PES_extension_field[i] = (uint8_t)readBits(8);
            }
            pes_header_len += 1;
            pes_header_len += header.PES_extension_field_length;
        }
    }

    //stuffing_byte
    if( header.PES_packet_length == 0 ){
        header.data_size = data.size()-pes_header_len;
    }else{
        header.data_size = header.PES_packet_length-(pes_header_len-6);
    }

    header.data = data.constData()+pes_header_len;

    // Check if the stream ID indicates that optional fields are present
    if (header.stream_id != 0xBC && header.stream_id != 0xBE && // program_stream_map, padding_stream
        header.stream_id != 0xBF && header.stream_id != 0xF0 && // private_stream_2, ECM
        header.stream_id != 0xF1 && header.stream_id != 0xF2 && // EMM, program_stream_directory
        header.stream_id != 0xF3 && header.stream_id != 0xF4) { // DSMCC_stream, ITU-T Rec.H.222.1

        // Read optional PES header fields
            //readPESOptionalFields(header);
    }

    return header;
}

uint32_t PSStream::readBits(int bits) {
    uint32_t value = 0;
    for (int i = 0; i < bits; ++i) {
        int bytePos = bitPosition / 8;
        int bitOffset = 7 - (bitPosition % 8);

        // 检查是否超出 QByteArray 的有效范围
        if (bytePos >= data.size()) {
            throw std::runtime_error("Read beyond end of QByteArray");
        }

        // 获取当前位
        bool bit = (data[bytePos] >> bitOffset) & 0x01;
        value = (value << 1) | bit;
        bitPosition++;
    }
    return value;
}

int PSStream::nextBits() {
    int bytePos = bitPosition / 8;
    int bitOffset = 7 - (bitPosition % 8);
    if (bytePos >= data.size()) {
        return 0; // 结束，超出范围
    }
    return (data[bytePos] >> bitOffset) & 0x01;
}

Descriptor PSStream::readDescriptor() {
    Descriptor desc;
    desc.descriptor_tag = readBits(8);
    desc.descriptor_length = readBits(8);
    // Read additional descriptor data as needed based on descriptor_length
    return desc;
}

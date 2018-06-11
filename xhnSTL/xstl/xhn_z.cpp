#include "xhn_z.hpp"

void xhn::OutputCommand::Print()
{
    if (type == EmptyCommand) {
        printf("Emtpy:\n");
    } else if (type == CompressCommand) {
        unsigned char* pattern = nullptr;
        dict->FindLine(compressHeader, compressId, &pattern);
        
        unsigned char com0 = pattern[0];
        unsigned char com1 = pattern[1];
        unsigned char com2 = pattern[2];
        unsigned char com3 = pattern[3];
        unsigned char com4 = pattern[4];
        unsigned char com5 = pattern[5];
        unsigned char com6 = pattern[6];
        unsigned char com7 = pattern[7];
        
        unsigned char com8 = pattern[8];
        unsigned char com9 = pattern[9];
        unsigned char com10 = pattern[10];
        unsigned char com11 = pattern[11];
        unsigned char com12 = pattern[12];
        unsigned char com13 = pattern[13];
        unsigned char com14 = pattern[14];
        unsigned char com15 = pattern[15];
        
        switch (compressLength)
        {
            case 1:
                printf("COMP:%c%c,%c|%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c\n",
                       (unsigned char)(compressHeader >> 8), (unsigned char)compressHeader,
                       com0, com1, com2, com3, com4, com5, com6, com7, com8, com9, com10, com11, com12, com13, com14, com15);
                break;
            case 2:
                printf("COMP:%c%c,%c%c|%c%c%c%c%c%c%c%c%c%c%c%c%c%c\n",
                       (unsigned char)(compressHeader >> 8), (unsigned char)compressHeader,
                       com0, com1, com2, com3, com4, com5, com6, com7, com8, com9, com10, com11, com12, com13, com14, com15);
                break;
            case 3:
                printf("COMP:%c%c,%c%c%c|%c%c%c%c%c%c%c%c%c%c%c%c%c\n",
                       (unsigned char)(compressHeader >> 8), (unsigned char)compressHeader,
                       com0, com1, com2, com3, com4, com5, com6, com7, com8, com9, com10, com11, com12, com13, com14, com15);
                break;
            case 4:
                printf("COMP:%c%c,%c%c%c%c|%c%c%c%c%c%c%c%c%c%c%c%c\n",
                       (unsigned char)(compressHeader >> 8), (unsigned char)compressHeader,
                       com0, com1, com2, com3, com4, com5, com6, com7, com8, com9, com10, com11, com12, com13, com14, com15);
                break;
            case 5:
                printf("COMP:%c%c,%c%c%c%c%c|%c%c%c%c%c%c%c%c%c%c%c\n",
                       (unsigned char)(compressHeader >> 8), (unsigned char)compressHeader,
                       com0, com1, com2, com3, com4, com5, com6, com7, com8, com9, com10, com11, com12, com13, com14, com15);
                break;
            case 6:
                printf("COMP:%c%c,%c%c%c%c%c%c|%c%c%c%c%c%c%c%c%c%c\n",
                       (unsigned char)(compressHeader >> 8), (unsigned char)compressHeader,
                       com0, com1, com2, com3, com4, com5, com6, com7, com8, com9, com10, com11, com12, com13, com14, com15);
                break;
            case 7:
                printf("COMP:%c%c,%c%c%c%c%c%c%c|%c%c%c%c%c%c%c%c%c\n",
                       (unsigned char)(compressHeader >> 8), (unsigned char)compressHeader,
                       com0, com1, com2, com3, com4, com5, com6, com7, com8, com9, com10, com11, com12, com13, com14, com15);
                break;
            case 8:
                printf("COMP:%c%c,%c%c%c%c%c%c%c%c|%c%c%c%c%c%c%c%c\n",
                       (unsigned char)(compressHeader >> 8), (unsigned char)compressHeader,
                       com0, com1, com2, com3, com4, com5, com6, com7, com8, com9, com10, com11, com12, com13, com14, com15);
                break;
            case 9:
                printf("COMP:%c%c,%c%c%c%c%c%c%c%c%c|%c%c%c%c%c%c%c\n",
                       (unsigned char)(compressHeader >> 8), (unsigned char)compressHeader,
                       com0, com1, com2, com3, com4, com5, com6, com7, com8, com9, com10, com11, com12, com13, com14, com15);
                break;
            case 10:
                printf("COMP:%c%c,%c%c%c%c%c%c%c%c%c%c|%c%c%c%c%c%c\n",
                       (unsigned char)(compressHeader >> 8), (unsigned char)compressHeader,
                       com0, com1, com2, com3, com4, com5, com6, com7, com8, com9, com10, com11, com12, com13, com14, com15);
                break;
            case 11:
                printf("COMP:%c%c,%c%c%c%c%c%c%c%c%c%c%c|%c%c%c%c%c\n",
                       (unsigned char)(compressHeader >> 8), (unsigned char)compressHeader,
                       com0, com1, com2, com3, com4, com5, com6, com7, com8, com9, com10, com11, com12, com13, com14, com15);
                break;
            case 12:
                printf("COMP:%c%c,%c%c%c%c%c%c%c%c%c%c%c%c|%c%c%c%c\n",
                       (unsigned char)(compressHeader >> 8), (unsigned char)compressHeader,
                       com0, com1, com2, com3, com4, com5, com6, com7, com8, com9, com10, com11, com12, com13, com14, com15);
                break;
            case 13:
                printf("COMP:%c%c,%c%c%c%c%c%c%c%c%c%c%c%c%c|%c%c%c\n",
                       (unsigned char)(compressHeader >> 8), (unsigned char)compressHeader,
                       com0, com1, com2, com3, com4, com5, com6, com7, com8, com9, com10, com11, com12, com13, com14, com15);
                break;
            case 14:
                printf("COMP:%c%c,%c%c%c%c%c%c%c%c%c%c%c%c%c%c|%c%c\n",
                       (unsigned char)(compressHeader >> 8), (unsigned char)compressHeader,
                       com0, com1, com2, com3, com4, com5, com6, com7, com8, com9, com10, com11, com12, com13, com14, com15);
                break;
            case 15:
                printf("COMP:%c%c,%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c|%c\n",
                       (unsigned char)(compressHeader >> 8), (unsigned char)compressHeader,
                       com0, com1, com2, com3, com4, com5, com6, com7, com8, com9, com10, com11, com12, com13, com14, com15);
                break;
            case 16:
                printf("COMP:%c%c,%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c|\n",
                       (unsigned char)(compressHeader >> 8), (unsigned char)compressHeader,
                       com0, com1, com2, com3, com4, com5, com6, com7, com8, com9, com10, com11, com12, com13, com14, com15);
                break;
            default:
                break;
        }
        
    } else {
        printf("BYTE:%c\n", byteData);
    }
}

void xhn::OutputCommand::TextPrint()
{
    if (type == EmptyCommand) {
    } else if (type == CompressCommand) {
        unsigned char* pattern = nullptr;
        dict->FindLine(compressHeader, compressId, &pattern);
        
        
        unsigned char com0 = pattern[0];
        unsigned char com1 = pattern[1];
        unsigned char com2 = pattern[2];
        unsigned char com3 = pattern[3];
        unsigned char com4 = pattern[4];
        unsigned char com5 = pattern[5];
        unsigned char com6 = pattern[6];
        unsigned char com7 = pattern[7];
        
        unsigned char com8 = pattern[8];
        unsigned char com9 = pattern[9];
        unsigned char com10 = pattern[10];
        unsigned char com11 = pattern[11];
        unsigned char com12 = pattern[12];
        unsigned char com13 = pattern[13];
        unsigned char com14 = pattern[14];
        unsigned char com15 = pattern[15];
        
        switch (compressLength)
        {
            case 1:
                printf("%c%c%c",
                       (unsigned char)(compressHeader >> 8), (unsigned char)compressHeader,
                       com0);
                break;
            case 2:
                printf("%c%c%c%c",
                       (unsigned char)(compressHeader >> 8), (unsigned char)compressHeader,
                       com0, com1);
                break;
            case 3:
                printf("%c%c%c%c%c",
                       (unsigned char)(compressHeader >> 8), (unsigned char)compressHeader,
                       com0, com1, com2);
                break;
            case 4:
                printf("%c%c%c%c%c%c",
                       (unsigned char)(compressHeader >> 8), (unsigned char)compressHeader,
                       com0, com1, com2, com3);
                break;
            case 5:
                printf("%c%c%c%c%c%c%c",
                       (unsigned char)(compressHeader >> 8), (unsigned char)compressHeader,
                       com0, com1, com2, com3, com4);
                break;
            case 6:
                printf("%c%c%c%c%c%c%c%c",
                       (unsigned char)(compressHeader >> 8), (unsigned char)compressHeader,
                       com0, com1, com2, com3, com4, com5);
                break;
            case 7:
                printf("%c%c%c%c%c%c%c%c%c",
                       (unsigned char)(compressHeader >> 8), (unsigned char)compressHeader,
                       com0, com1, com2, com3, com4, com5, com6);
                break;
            case 8:
                printf("%c%c%c%c%c%c%c%c%c%c",
                       (unsigned char)(compressHeader >> 8), (unsigned char)compressHeader,
                       com0, com1, com2, com3, com4, com5, com6, com7);
                break;
            case 9:
                printf("%c%c%c%c%c%c%c%c%c%c%c",
                       (unsigned char)(compressHeader >> 8), (unsigned char)compressHeader,
                       com0, com1, com2, com3, com4, com5, com6, com7, com8);
                break;
            case 10:
                printf("%c%c%c%c%c%c%c%c%c%c%c%c",
                       (unsigned char)(compressHeader >> 8), (unsigned char)compressHeader,
                       com0, com1, com2, com3, com4, com5, com6, com7, com8, com9);
                break;
            case 11:
                printf("%c%c%c%c%c%c%c%c%c%c%c%c%c",
                       (unsigned char)(compressHeader >> 8), (unsigned char)compressHeader,
                       com0, com1, com2, com3, com4, com5, com6, com7, com8, com9, com10);
                break;
            case 12:
                printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c",
                       (unsigned char)(compressHeader >> 8), (unsigned char)compressHeader,
                       com0, com1, com2, com3, com4, com5, com6, com7, com8, com9, com10, com11);
                break;
            case 13:
                printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",
                       (unsigned char)(compressHeader >> 8), (unsigned char)compressHeader,
                       com0, com1, com2, com3, com4, com5, com6, com7, com8, com9, com10, com11, com12);
                break;
            case 14:
                printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",
                       (unsigned char)(compressHeader >> 8), (unsigned char)compressHeader,
                       com0, com1, com2, com3, com4, com5, com6, com7, com8, com9, com10, com11, com12, com13);
                break;
            case 15:
                printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",
                       (unsigned char)(compressHeader >> 8), (unsigned char)compressHeader,
                       com0, com1, com2, com3, com4, com5, com6, com7, com8, com9, com10, com11, com12, com13, com14);
                break;
            case 16:
                printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",
                       (unsigned char)(compressHeader >> 8), (unsigned char)compressHeader,
                       com0, com1, com2, com3, com4, com5, com6, com7, com8, com9, com10, com11, com12, com13, com14, com15);
                break;
            default:
                break;
        }
        
        
    } else {
        printf("%c", byteData);
    }
}

inline void PushAByteImpl(unsigned short& m_header, unsigned char*& m_pattern)
{
    unsigned char tmp0 = *m_pattern;
    m_pattern++;
    
    m_header = m_header << 8;
    m_header |= (unsigned short)tmp0;
}

xhn::OutputCommand xhn::MatchBuffer::PushAByte()
{
    m_positionCount++;
    OutputCommand ret;
    if (m_numberOfBytesOnPipeline < PIPELINE_LENGTH) {
        ret.type = EmptyCommand;
        
        PushAByteImpl(m_header, m_pattern);
        
        m_numberOfBytesOnPipeline++;
    } else {
        unsigned int matchLength = 0;
        unsigned int matchId = 0;
        if (m_dictionary->FindLine(m_header, m_pattern, m_positionCount, &matchLength, &matchId)) {
            ret.type = CompressCommand;
            ret.compressHeader = m_header;
            ret.compressLength = (unsigned char)matchLength;
            ret.compressId = (unsigned char)matchId;
            ret.dict = m_dictionary;
            m_numberOfBytesOnPipeline -= (matchLength + 2);
            
            PushAByteImpl(m_header, m_pattern);
            
            m_numberOfBytesOnPipeline++;
        } else {
            unsigned char retByte = (unsigned char)(m_header >> 8);
            
            PushAByteImpl(m_header, m_pattern);
            
            m_dictionary->PushNewLine(m_header, m_pattern, m_positionCount);
            
            ret.type = ByteCommand;
            ret.byteData = retByte;
        }
    }
    return ret;
}

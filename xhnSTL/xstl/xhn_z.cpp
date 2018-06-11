#include "xhn_z.hpp"

void xhn::OutputCommand::Print()
{
    if (type == EmptyCommand) {
        printf("Emtpy:\n");
    } else if (type == CompressCommand) {
        unsigned long long pattern = 0;
        dict->FindLine(compressHeader, compressId, &pattern);
        
        unsigned char com0 = (unsigned char)(pattern >> 56);
        unsigned char com1 = (unsigned char)(pattern >> 48);
        unsigned char com2 = (unsigned char)(pattern >> 40);
        unsigned char com3 = (unsigned char)(pattern >> 32);
        unsigned char com4 = (unsigned char)(pattern >> 24);
        unsigned char com5 = (unsigned char)(pattern >> 16);
        unsigned char com6 = (unsigned char)(pattern >> 8);
        unsigned char com7 = (unsigned char)(pattern >> 0);
        
        switch (compressLength)
        {
            case 1:
                printf("COMP:%c%c,%c|%c%c%c%c%c%c%c\n",
                       (unsigned char)(compressHeader >> 8), (unsigned char)compressHeader,
                       com0, com1, com2, com3, com4, com5, com6, com7);
                break;
            case 2:
                printf("COMP:%c%c,%c%c|%c%c%c%c%c%c\n",
                       (unsigned char)(compressHeader >> 8), (unsigned char)compressHeader,
                       com0, com1, com2, com3, com4, com5, com6, com7);
                break;
            case 3:
                printf("COMP:%c%c,%c%c%c|%c%c%c%c%c\n",
                       (unsigned char)(compressHeader >> 8), (unsigned char)compressHeader,
                       com0, com1, com2, com3, com4, com5, com6, com7);
                break;
            case 4:
                printf("COMP:%c%c,%c%c%c%c|%c%c%c%c\n",
                       (unsigned char)(compressHeader >> 8), (unsigned char)compressHeader,
                       com0, com1, com2, com3, com4, com5, com6, com7);
                break;
            case 5:
                printf("COMP:%c%c,%c%c%c%c%c|%c%c%c\n",
                       (unsigned char)(compressHeader >> 8), (unsigned char)compressHeader,
                       com0, com1, com2, com3, com4, com5, com6, com7);
                break;
            case 6:
                printf("COMP:%c%c,%c%c%c%c%c%c|%c%c\n",
                       (unsigned char)(compressHeader >> 8), (unsigned char)compressHeader,
                       com0, com1, com2, com3, com4, com5, com6, com7);
                break;
            case 7:
                printf("COMP:%c%c,%c%c%c%c%c%c%c|%c\n",
                       (unsigned char)(compressHeader >> 8), (unsigned char)compressHeader,
                       com0, com1, com2, com3, com4, com5, com6, com7);
                break;
            case 8:
                printf("COMP:%c%c,%c%c%c%c%c%c%c%c|\n",
                       (unsigned char)(compressHeader >> 8), (unsigned char)compressHeader,
                       com0, com1, com2, com3, com4, com5, com6, com7);
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
        unsigned long long pattern = 0;
        dict->FindLine(compressHeader, compressId, &pattern);
        
        unsigned char com0 = (unsigned char)(pattern >> 56);
        unsigned char com1 = (unsigned char)(pattern >> 48);
        unsigned char com2 = (unsigned char)(pattern >> 40);
        unsigned char com3 = (unsigned char)(pattern >> 32);
        unsigned char com4 = (unsigned char)(pattern >> 24);
        unsigned char com5 = (unsigned char)(pattern >> 16);
        unsigned char com6 = (unsigned char)(pattern >> 8);
        unsigned char com7 = (unsigned char)(pattern >> 0);
        
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
            default:
                break;
        }
        
    } else {
        printf("%c", byteData);
    }
}

xhn::OutputCommand xhn::MatchBuffer::PushAByte(unsigned char byte)
{
    m_positionCount++;
    OutputCommand ret;
    if (m_numberOfBytesOnPipeline < 10) {
        ret.type = EmptyCommand;
        if (m_numberOfBytesOnPipeline < 8) {
            m_pattern = m_pattern << 8;
            m_pattern |= (unsigned long long)byte;
        } else {
            m_header = m_header << 8;
            unsigned char tmp = (unsigned char)(m_pattern >> 56);
            m_header = m_header | (unsigned short)tmp;
            m_pattern = m_pattern << 8;
            m_pattern |= (unsigned long long)byte;
        }
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
            
            if (m_numberOfBytesOnPipeline < 8) {
                m_pattern = m_pattern << 8;
                m_pattern |= (unsigned long long)byte;
            } else {
                m_header = m_header << 8;
                unsigned char tmp = (unsigned char)(m_pattern >> 56);
                m_header = m_header | (unsigned short)tmp;
                m_pattern = m_pattern << 8;
                m_pattern |= (unsigned long long)byte;
            }
            m_numberOfBytesOnPipeline++;
        } else {
            unsigned char retByte = (unsigned char)(m_header >> 8);
            unsigned char tmp = (unsigned char)(m_pattern >> 56);
            m_header = ((m_header << 8) | (unsigned short)tmp);
            m_pattern = ((m_pattern << 8) | (unsigned long long)byte);
            
            m_dictionary->PushNewLine(m_header, m_pattern, m_positionCount);
            
            ret.type = ByteCommand;
            ret.byteData = retByte;
        }
    }
    return ret;
}

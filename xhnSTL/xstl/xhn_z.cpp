#include "xhn_z.hpp"

void xhn::OutputCommand::Print()
{
    if (type == EmptyCommand) {
        printf("Emtpy:\n");
    } else if (type == CompressCommand) {
        unsigned long long pattern0 = 0;
        unsigned long long pattern1 = 0;
        dict->FindLine(compressHeader, compressId, &pattern0, &pattern1);
        
        unsigned char com0 = (unsigned char)(pattern0 >> 56);
        unsigned char com1 = (unsigned char)(pattern0 >> 48);
        unsigned char com2 = (unsigned char)(pattern0 >> 40);
        unsigned char com3 = (unsigned char)(pattern0 >> 32);
        unsigned char com4 = (unsigned char)(pattern0 >> 24);
        unsigned char com5 = (unsigned char)(pattern0 >> 16);
        unsigned char com6 = (unsigned char)(pattern0 >> 8);
        unsigned char com7 = (unsigned char)(pattern0 >> 0);
        
        unsigned char com8 = (unsigned char)(pattern1 >> 56);
        unsigned char com9 = (unsigned char)(pattern1 >> 48);
        unsigned char com10 = (unsigned char)(pattern1 >> 40);
        unsigned char com11 = (unsigned char)(pattern1 >> 32);
        unsigned char com12 = (unsigned char)(pattern1 >> 24);
        unsigned char com13 = (unsigned char)(pattern1 >> 16);
        unsigned char com14 = (unsigned char)(pattern1 >> 8);
        unsigned char com15 = (unsigned char)(pattern1 >> 0);
        
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
        unsigned long long pattern0 = 0;
        unsigned long long pattern1 = 0;
        dict->FindLine(compressHeader, compressId, &pattern0, &pattern1);
        
        
        unsigned char com0 = (unsigned char)(pattern0 >> 56);
        unsigned char com1 = (unsigned char)(pattern0 >> 48);
        unsigned char com2 = (unsigned char)(pattern0 >> 40);
        unsigned char com3 = (unsigned char)(pattern0 >> 32);
        unsigned char com4 = (unsigned char)(pattern0 >> 24);
        unsigned char com5 = (unsigned char)(pattern0 >> 16);
        unsigned char com6 = (unsigned char)(pattern0 >> 8);
        unsigned char com7 = (unsigned char)(pattern0 >> 0);
        
        unsigned char com8 = (unsigned char)(pattern1 >> 56);
        unsigned char com9 = (unsigned char)(pattern1 >> 48);
        unsigned char com10 = (unsigned char)(pattern1 >> 40);
        unsigned char com11 = (unsigned char)(pattern1 >> 32);
        unsigned char com12 = (unsigned char)(pattern1 >> 24);
        unsigned char com13 = (unsigned char)(pattern1 >> 16);
        unsigned char com14 = (unsigned char)(pattern1 >> 8);
        unsigned char com15 = (unsigned char)(pattern1 >> 0);
        
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

inline void PushAByteImpl(unsigned short& m_header, unsigned long long& m_pattern0, unsigned long long& m_pattern1,
                          unsigned char byte)
{
    unsigned char tmp1 = (unsigned char)(m_pattern1 >> 56);
    m_pattern1 = m_pattern1 << 8;
    m_pattern1 |= (unsigned long long)byte;
    
    unsigned char tmp0 = (unsigned char)(m_pattern0 >> 56);
    m_pattern0 = m_pattern0 << 8;
    m_pattern0 |= tmp1;
    
    m_header = m_header << 8;
    m_header |= (unsigned short)tmp0;
}

xhn::OutputCommand xhn::MatchBuffer::PushAByte(unsigned char byte)
{
    m_positionCount++;
    OutputCommand ret;
    if (m_numberOfBytesOnPipeline < 18) {
        ret.type = EmptyCommand;
        
        PushAByteImpl(m_header, m_pattern0, m_pattern1, byte);
        
        m_numberOfBytesOnPipeline++;
    } else {
        unsigned int matchLength = 0;
        unsigned int matchId = 0;
        if (m_dictionary->FindLine(m_header, m_pattern0, m_pattern1, m_positionCount, &matchLength, &matchId)) {
            ret.type = CompressCommand;
            ret.compressHeader = m_header;
            ret.compressLength = (unsigned char)matchLength;
            ret.compressId = (unsigned char)matchId;
            ret.dict = m_dictionary;
            m_numberOfBytesOnPipeline -= (matchLength + 2);
            
            PushAByteImpl(m_header, m_pattern0, m_pattern1, byte);
            
            m_numberOfBytesOnPipeline++;
        } else {
            unsigned char retByte = (unsigned char)(m_header >> 8);
            
            PushAByteImpl(m_header, m_pattern0, m_pattern1, byte);
            
            m_dictionary->PushNewLine(m_header, m_pattern0, m_pattern1, m_positionCount);
            
            ret.type = ByteCommand;
            ret.byteData = retByte;
        }
    }
    return ret;
}

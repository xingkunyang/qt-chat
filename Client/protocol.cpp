#include "protocol.h"
#include "stdlib.h"
#include "string.h"

PDU *initPDU(uint uiMsgLen)
{
    uint uiTotalLen = sizeof(PDU) + uiMsgLen;
    PDU* pdu = (PDU*)malloc(uiTotalLen);
    memset(pdu, 0, uiTotalLen);
    pdu->uiTotalLen = uiTotalLen;
    pdu->uiMsgLen = uiMsgLen;
    return pdu;
}

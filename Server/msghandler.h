#ifndef MSGHANDLER_H
#define MSGHANDLER_H

#include "protocol.h"
#include <QFile>
#include <QString>

class MsgHandler
{
public:
    PDU* pdu;
    QFile m_fUploadFile;
    qint64 m_iUploadTotal;
    qint64 m_iUploadReceived;

    MsgHandler();
    PDU* handleRegist();
    PDU* handleLogin(QString& strLoginName);
    PDU* handleFindUser();
    PDU* handleOnlineUser();
    PDU* handleAdjudgeAddFriend();
    PDU* handleAgreeAddFriend();
    PDU* handleFlushFriend();
    PDU* handleDeleteFriend();
    void handleChat();
    PDU* handleMkdir();
    PDU* handleFlushFile();
    PDU* handleDelFile();
    PDU* handleRenameFile();
    PDU* handleInitUploadFile();
    PDU* handleDataUploadFile();
    PDU* handleInitShareFile();
    PDU* handleAgreeShareFile();
};

#endif // MSGHANDLER_H

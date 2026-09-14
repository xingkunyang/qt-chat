#ifndef RESHANDLER_H
#define RESHANDLER_H

#include "protocol.h"

class ResHandler
{
public:
    ResHandler();
    PDU* pdu;
    void handleRegist();
    void handleLogin();
    void handleFindUser();
    void handleOnlineUser();
    void handleAdjudgeAddFriend();
    void handleAdjudgeAddFriendResend();
    void handleAgreeAddFriend();
    void handleFlushFriend();
    void handleDeleteFriend();
    void handleChat();
    void handleMkdir();
    void handleFlushFile();
    void handleDelFile();
    void handleRenameFile();
    void handleInitUploadFile();
    void handleInitShareFile();
    void handleAgreeShareFile();
};

#endif // RESHANDLER_H

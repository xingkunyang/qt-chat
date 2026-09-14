#include "reshandler.h"
#include <QMessageBox>
#include "client.h"
#include "index.h"
#include "string.h"

ResHandler::ResHandler()
{

}

void ResHandler::handleRegist()
{
    bool ret;
    memcpy(&ret, pdu->caData, sizeof(bool));

    if (ret) {
        QMessageBox::information(&Client::getInstance(), "提示", "注册成功");
    } else {
        QMessageBox::information(&Client::getInstance(), "提示", "注册失败");
    }
}

void ResHandler::handleLogin()
{
    bool ret;
    memcpy(&ret, pdu->caData, sizeof(bool));

    if (ret) {
        Index::getInstance().show();
        Index::getInstance().getFriend()->flushFriend();
        Index::getInstance().getFile()->flushFile();
        Client::getInstance().hide();
    } else {
        QMessageBox::information(&Client::getInstance(), "提示", "登录失败");
    }
}

void ResHandler::handleFindUser()
{
    int ret;
    memcpy(&ret, pdu->caData, sizeof(int));

    if (ret == -1) {
        QMessageBox::information(&Index::getInstance(), "提示", "该用户不存在");
    } else if (ret == 0) {
        QMessageBox::information(&Index::getInstance(), "提示", "该用户不在线");
    } else if (ret == 1) {
        QMessageBox::information(&Index::getInstance(), "提示", "该用户在线");
    }
}

void ResHandler::handleOnlineUser()
{
    uint uiSize = pdu->uiMsgLen/32;
    QStringList slName;
    slName.clear();
    char caTmp[32] = {'\0'};
    for (uint i=0; i<uiSize; i++) {
        memcpy(caTmp, pdu->caMsg+i*32, 32);
        slName.append(caTmp);
    }
    Index::getInstance().getFriend()->m_pOnlineUser->updateListWidget(slName);
}

void ResHandler::handleAdjudgeAddFriend()
{
    int ret;
    memcpy(&ret, pdu->caData, sizeof(int));

    if (ret == -1)
    {
        QMessageBox::information(&Index::getInstance(), "提示", "添加错误: 联系开发人员");
    }
    else if (ret == 0)
    {
        QMessageBox::information(&Index::getInstance(), "提示", "该用户不在线");
    }
    else if (ret == -2)
    {
        QMessageBox::information(&Index::getInstance(), "提示", "该用户已经是你的好友");
    }
}

void ResHandler::handleAdjudgeAddFriendResend()
{
    char caName[32] = {'\0'};
    memcpy(caName, pdu->caData, 32);
    int ret = QMessageBox::question(&Index::getInstance(), "添加好友", QString("是否同意 %1 的添加好友请求? ").arg(caName));
    if (ret != QMessageBox::Yes)
    {
        return;
    }

    PDU* respdu = initPDU(0);
    memcpy(respdu->caData, pdu->caData, 64);
    respdu->uiType = ENUM_TYPE_AGREE_ADD_FRIEND_REQ;
    Client::getInstance().sendMsg(respdu);
}

void ResHandler::handleAgreeAddFriend()
{
    bool ret;
    memcpy(&ret, pdu->caData, sizeof(bool));
    if (ret) {
        Index::getInstance().getFriend()->flushFriend();
    } else {
        QMessageBox::information(&Client::getInstance(), "提示", "添加好友失败");
    }
}

void ResHandler::handleFlushFriend()
{
    uint uiSize = pdu->uiMsgLen/32;
    QStringList slName;
    slName.clear();
    char caTmp[32] = {'\0'};
    for (uint i=0; i<uiSize; i++) {
        memcpy(caTmp, pdu->caMsg+i*32, 32);
        slName.append(caTmp);
    }
    Index::getInstance().getFriend()->updateListWidget(slName);
    Index::getInstance().getFile()->m_pShareFile->updateListWidget(slName);
}

void ResHandler::handleDeleteFriend()
{
    bool ret;
    memcpy(&ret, pdu->caData, sizeof(bool));
    if (ret) {
        Index::getInstance().getFriend()->flushFriend();
    } else {
        QMessageBox::information(&Client::getInstance(), "提示", "删除好友失败");
    }
}

void ResHandler::handleChat()
{
    Chat* c = Index::getInstance().getFriend()->m_pChat;
    if (c->isHidden()) {
        c->show();
    }

    char caCurName[32] = {'\0'};
    memcpy(caCurName, pdu->caData, 32);
    QString strChatMsg = QString("%1:%2").arg(caCurName).arg(pdu->caMsg);
    c->updateShow_TE(strChatMsg);
    c->m_strChatName = caCurName;
}

void ResHandler::handleMkdir()
{
    bool ret;
    memcpy(&ret, pdu->caData, sizeof(bool));

    if (ret) {
        Index::getInstance().getFile()->flushFile();
    } else {
        QMessageBox::information(&Index::getInstance(), "提示", "创建文件夹失败");
    }
}

void ResHandler::handleFlushFile()
{
    int iCount = pdu->uiMsgLen/sizeof(FileInfo);
    qDebug() << "handleFlushFile iCount" << iCount;
    QList<FileInfo*> pFileList;

    for (int i=0; i<iCount; i++) {
        FileInfo* pFileInfo = new FileInfo;
        memcpy(pFileInfo, pdu->caMsg+i*sizeof(FileInfo), sizeof(FileInfo));
        pFileList.append(pFileInfo);
    }

    Index::getInstance().getFile()->updateFileList(pFileList);
}

void ResHandler::handleDelFile()
{
    bool ret;
    memcpy(&ret, pdu->caData, sizeof(bool));

    if (ret) {
        Index::getInstance().getFile()->flushFile();
    } else {
        QMessageBox::information(&Index::getInstance(), "提示", "删除失败");
    }
}

void ResHandler::handleRenameFile()
{
    bool ret;
    memcpy(&ret, pdu->caData, sizeof(bool));
    if (ret) {
        Index::getInstance().getFile()->flushFile();
    } else {
        QMessageBox::information(&Index::getInstance(), "提示", "重命名失败");
    }
}

void ResHandler::handleInitUploadFile()
{
    bool ret;
    memcpy(&ret, pdu->caData, sizeof(bool));
    if (ret) {
        Index::getInstance().getFile()->uploadFile();
    } else {
        QMessageBox::information(&Index::getInstance(), "提示", "上传文件失败");
    }
}

void ResHandler::handleInitShareFile()
{
    char caName[32] = {'\0'};
    memcpy(caName, pdu->caData, 32);
    QString strPath = pdu->caMsg;
    int index = strPath.lastIndexOf('/');
    QString strFileName = strPath.right(strPath.size() - index - 1);
    int ret = QMessageBox::question(
        &Index::getInstance(),
        "分享文件",
        QString("%1 分享的文件 %2\n是否同意接收?").arg(caName).arg(strFileName));
    if (ret!= QMessageBox::Yes) {
        return;
    }
    PDU* respdu = initPDU(pdu->uiMsgLen);
    memcpy(respdu->caData, pdu->caData, 64);
    memcpy(respdu->caMsg, pdu->caMsg, pdu->uiMsgLen);
    respdu->uiType = ENUM_TYPE_AGREE_SHARE_FILE_REQ;
    Client::getInstance().sendMsg(respdu);
}

void ResHandler::handleAgreeShareFile()
{
    bool ret;
    memcpy(&ret, pdu->caData, sizeof(bool));
    if (ret) {
        QMessageBox::information(&Index::getInstance(), "提示", "分享文件成功");
        Index::getInstance().getFile()->uploadFile();
    } else {
        QMessageBox::information(&Index::getInstance(), "提示", "分享文件失败");
    }
}


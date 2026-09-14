#include "msghandler.h"
#include "mytcpserver.h"
#include "operatedb.h"
#include "server.h"
#include "stdlib.h"
#include "string.h"

#include <QDebug>
#include <QDir>

MsgHandler::MsgHandler()
{

}

PDU *MsgHandler::handleRegist()
{
    char caName[32] = {'\0'};
    char caPwd[32] = {'\0'};
    memcpy(caName, pdu->caData, 32);
    memcpy(caPwd, pdu->caData+32, 32);
    qDebug() << "regist caName" << caName << "caPwd" << caPwd;
    bool res = OperateDB::getInstance().handleRegist(caName, caPwd);
    qDebug() << "regist res" << res;
    if (res) {
        QDir dir;
        bool ret = dir.mkdir(QString("%1/%2").arg(Server::getInstance().m_strRootPath).arg(caName));
        qDebug() << "mkdir ret" << ret;
    }
    PDU* respdu = initPDU(0);
    respdu->uiType = ENUM_TYPE_REGIST_RES;
    memcpy(respdu->caData, &res, sizeof(bool));
    return respdu;
}


PDU *MsgHandler::handleLogin(QString &strLoginName)
{
    char caName[32] = {'\0'};
    char caPwd[32] = {'\0'};
    memcpy(caName, pdu->caData, 32);
    memcpy(caPwd, pdu->caData+32, 32);
    qDebug() << "login caName" << caName << "caPwd" << caPwd;
    bool res = OperateDB::getInstance().handleLogin(caName, caPwd);
    qDebug() << "login res" << res;
    if (res) {
        strLoginName = caName;
    }
    PDU* respdu = initPDU(0);
    respdu->uiType = ENUM_TYPE_LOGIN_RES;
    memcpy(respdu->caData, &res, sizeof(bool));
    return respdu;
}

PDU *MsgHandler::handleFindUser()
{
    char caName[32] = {'\0'};
    memcpy(caName, pdu->caData, 32);
    qDebug() << "find user caName" << caName;
    int res = OperateDB::getInstance().handleFindUser(caName);
    qDebug() << "find user res" << res;
    PDU* respdu = initPDU(0);
    respdu->uiType = ENUM_TYPE_FIND_USER_RES;
    memcpy(respdu->caData, &res, sizeof(int));
    return respdu;
}

PDU *MsgHandler::handleOnlineUser()
{
    QStringList result = OperateDB::getInstance().handleOnlineUser();
    PDU* respdu = initPDU(result.size() * 32);
    respdu->uiType = ENUM_TYPE_ONLINE_USER_RES;
    for (int i = 0; i < result.size(); i++) {
        memcpy(respdu->caMsg + 32 * i, result[i].toStdString().c_str(), 32);
    }
    return respdu;
}

PDU *MsgHandler::handleAdjudgeAddFriend()
{
    char caCurName[32] = {'\0'};
    char caTarName[32] = {'\0'};
    memcpy(caCurName, pdu->caData, 32);
    memcpy(caTarName, pdu->caData+32, 32);
    qDebug() << "login caCurName" << caCurName << "caTarName" << caTarName;
    int res = OperateDB::getInstance().handleAdjudgeAddFriend(caCurName, caTarName);
    qDebug() << "handleAdjudgeAddFriend res" << res;
    if (res == 1) {
        MyTcpServer::getInstance().resend(caTarName, pdu);
        return NULL;
    } else {
        PDU* respdu = initPDU(0);
        respdu->uiType = ENUM_TYPE_ADJUDGE_ADD_FRIEND_RES;
        memcpy(respdu->caData, &res, sizeof(int));
        return respdu;
    }
}

PDU *MsgHandler::handleAgreeAddFriend()
{
    char caCurName[32] = {'\0'};
    char caTarName[32] = {'\0'};
    memcpy(caCurName, pdu->caData, 32);
    memcpy(caTarName, pdu->caData+32, 32);
    qDebug() << "handleAgreeAddFriend caCurName" << caCurName << "caTarName" << caTarName;

    bool res = OperateDB::getInstance().handleAgreeAddFriend(caCurName, caTarName);
    qDebug() << "handleAgreeAddFriend res" << res;

    PDU* respdu = initPDU(0);
    respdu->uiType = ENUM_TYPE_AGREE_ADD_FRIEND_RES;
    memcpy(respdu->caData, &res, sizeof(bool));
    MyTcpServer::getInstance().resend(caCurName, respdu);
    return respdu;
}

PDU *MsgHandler::handleFlushFriend()
{
    char caName[32] = {'\0'};
    memcpy(caName, pdu->caData, 32);
    qDebug() << "flush friend caName" << caName;
    QStringList result = OperateDB::getInstance().handleFlushFriend(caName);
    PDU* respdu = initPDU(result.size() * 32);
    respdu->uiType = ENUM_TYPE_FLUSH_FRIEND_RES;
    for (int i = 0; i < result.size(); i++) {
        memcpy(respdu->caMsg + 32 * i, result[i].toStdString().c_str(), 32);
    }
    return respdu;
}

PDU *MsgHandler::handleDeleteFriend()
{
    char caCurName[32] = {'\0'};
    char caTarName[32] = {'\0'};
    memcpy(caCurName, pdu->caData, 32);
    memcpy(caTarName, pdu->caData+32, 32);
    qDebug() << "delete friend" << caCurName << caTarName;
    bool res = OperateDB::getInstance().handleDeleteFriend(caCurName, caTarName);
    qDebug() << "delete friend res" << res;
    PDU* respdu = initPDU(0);
    respdu->uiType = ENUM_TYPE_DELETE_FRIEND_RES;
    memcpy(respdu->caData, &res, sizeof(bool));
    return respdu;
}

void MsgHandler::handleChat()
{
    char caTarName[32] = {'\0'};
    memcpy(caTarName, pdu->caData+32, 32);
    qDebug() << "handleChat" << caTarName;
    MyTcpServer::getInstance().resend(caTarName, pdu);
}

PDU *MsgHandler::handleMkdir()
{
    QString strPath = QString("%1/%2").arg(pdu->caMsg).arg(pdu->caData);
    qDebug() << "handleMkdir strPath" << strPath;
    QDir dir;
    bool res = dir.mkdir(strPath);
    qDebug() << "handleMkdir res" << res;
    PDU* respdu = initPDU(0);
    respdu->uiType = ENUM_TYPE_MKDIR_RES;
    memcpy(respdu->caData, &res, sizeof(bool));
    return respdu;
}

PDU *MsgHandler::handleFlushFile()
{
    //获取当前路径下的文件信息
    QDir dir(pdu->caMsg);
    QFileInfoList fileList = dir.entryInfoList();
    qDebug() << "handleFlushFile fileList.size()" << fileList.size();
    //构建响应pdu
    PDU* respdu = initPDU((fileList.size()-2)* sizeof(FileInfo));
    respdu->uiType = ENUM_TYPE_FLUSH_FILE_RES;
    //遍历文件信息
    for (int i=0,j=0; i<fileList.size(); i++) {
        QString strFileName = fileList[i].fileName();
        if(strFileName == "."|| strFileName == "..") {
            continue;
        }
        FileInfo* pFileInfo = (FileInfo*)respdu->caMsg+j++;
        if (fileList[i].isDir()) {
            pFileInfo->uiType = 0;
        } else {
            pFileInfo->uiType = 1;
        }

        memcpy(pFileInfo->caName,
               strFileName.toStdString().c_str(), 32);
        qDebug() << "strFileName" << strFileName;
    }
    return respdu;
}

PDU *MsgHandler::handleDelFile()
{
    QString strPath =
    QString("%1/%2").arg(pdu->caMsg).arg(pdu->caData);
    qDebug() << "handleDelFile strPath" << strPath;
    QDir dir(strPath);
    bool res = dir.removeRecursively();
    qDebug() << "handleMkdir res" << res;
    PDU* respdu = initPDU(0);
    respdu->uiType = ENUM_TYPE_DEL_FILE_RES;
    memcpy(respdu->caData, &res, sizeof(bool));
    return respdu;
}

PDU *MsgHandler::handleRenameFile()
{
    char caOldName[32] = {'\0'};
    char caNewName[32] = {'\0'};
    memcpy(caOldName, pdu->caData, 32);
    memcpy(caNewName, pdu->caData+32, 32);
    qDebug() << "handleRenameFile caOldName" << caOldName << "caNewName" << caNewName;
    QString strOldPath =
    QString("%1/%2").arg(pdu->caMsg).arg(caOldName);
    QString strNewPath =
    QString("%1/%2").arg(pdu->caMsg).arg(caNewName);
    QDir dir;
    bool res = dir.rename(strOldPath, strNewPath);
    qDebug() << "handleRenameFile res" << res;
    PDU* respdu = initPDU(0);
    respdu->uiType = ENUM_TYPE_RENAME_FILE_RES;
    memcpy(respdu->caData, &res, sizeof(bool));
    return respdu;
}

PDU *MsgHandler::handleInitUploadFile()
{
    char caFileName[32] = {'\0'};
    m_iUploadTotal = 0;
    memcpy(caFileName, pdu->caData, 32);
    memcpy(&m_iUploadTotal, pdu->caData+32, sizeof(qint64));
    QString strPath =
    QString("%1/%2").arg(pdu->caMsg).arg(caFileName);
    m_fUploadFile.setFileName(strPath);
    bool res = m_fUploadFile.open(QIODevice::WriteOnly);
    m_iUploadReceived = 0;
    PDU* respdu = initPDU(0);
    respdu->uiType = ENUM_TYPE_INIT_UPLOAD_FILE_RES;
    memcpy(respdu->caData, &res, sizeof(bool));
    return respdu;
}

PDU *MsgHandler::handleDataUploadFile()
{
    qint64 ret = m_fUploadFile.write(pdu->caMsg, pdu->uiMsgLen);
    if (ret < 0) {
        qDebug() << "handleDataUploadFile write error ret" << ret;
        return NULL;
    }
    m_iUploadReceived += pdu->uiMsgLen;
    if (m_iUploadReceived < m_iUploadTotal) {
        return NULL;
    }
    m_fUploadFile.close();
    PDU* respdu = initPDU(0);
    respdu->uiType = ENUM_TYPE_DATA_UPLOAD_FILE_RES;
    return respdu;
}

PDU *MsgHandler::handleInitShareFile()
{
    char caFriendName[32] = {'\0'};
    memcpy(caFriendName, pdu->caData+32, 32);
    MyTcpServer::getInstance().resend(caFriendName, pdu);
    PDU* respdu = initPDU(0);
    respdu->uiType = ENUM_TYPE_INIT_SHARE_FILE_RES;
    return respdu;
}

PDU *MsgHandler::handleAgreeShareFile()
{
    char caCurName[32] = {'\0'};
    memcpy(caCurName, pdu->caData, 32);
    char caFriendName[32] = {'\0'};
    memcpy(caFriendName, pdu->caData+32, 32);
    QString strSrcPath = pdu->caMsg;
    int index = strSrcPath.lastIndexOf('/');
    QString strFileName = strSrcPath.right(strSrcPath.size() - index - 1);
    QString strRecvPath = QString("%1/%2/%3")
            .arg(Server::getInstance().m_strRootPath)
            .arg(caFriendName).arg(strFileName);
    bool res = QFile::copy(strSrcPath, strRecvPath);
    PDU* respdu = initPDU(0);
    respdu->uiType = ENUM_TYPE_AGREE_SHARE_FILE_RES;
    memcpy(respdu->caData, &res, sizeof(bool));
    MyTcpServer::getInstance().resend(caCurName, respdu);
    return respdu;
}

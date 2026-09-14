#include "mytcpserver.h"
#include "mytcpsocket.h"
#include "operatedb.h"
#include "protocol.h"

MyTcpSocket::MyTcpSocket()
{
    m_pmh = new MsgHandler;
}

MyTcpSocket::~MyTcpSocket()
{
    delete m_pmh;
}

void MyTcpSocket::recvMsg()
{
    qDebug() << "recvMsg 接受消息长度" << this->bytesAvailable();
    QByteArray data = this->readAll();
    buffer.append(data);

    while (buffer.size() >= int(sizeof (PDU))) {
        PDU* pdu = (PDU*)buffer.data();
        if (buffer.size() < int(pdu->uiTotalLen)) {
            break;
        }
        PDU* respdu = handleMsg(pdu);
        sendMsg(respdu);
        buffer.remove(0, pdu->uiTotalLen);
    }
}

void MyTcpSocket::sendMsg(PDU *pdu)
{
    if (pdu == NULL){
        return;
    }
    this->write((char*)pdu, pdu->uiTotalLen);
    qDebug() << "send msg uiTotalLen" << pdu->uiTotalLen
             << "uiMsgLen" << pdu->uiMsgLen
             << "uiType" << pdu->uiType
             << "caData" << pdu->caData
             << "caData+32" << pdu->caData+32
             << "caMsg" << pdu->caMsg;

    free(pdu);
    pdu = NULL;
}

PDU *MyTcpSocket::readPDU()
{
    qDebug() << "recvMsgs 接受消息长度" << this->bytesAvailable();
    //先将总长度读出来，确定消息长度
    uint uiTotalLen = 0;
    this->read((char*)&uiTotalLen, sizeof(uint));
    //根据总长度计算得到柔性数组长度，再初始化pdu
    uint uiMsgLen = uiTotalLen - sizeof(PDU);
    PDU* pdu = initPDU(uiMsgLen);
    //将pdu放入read函数进行读取，需要偏移
    this->read((char*)pdu+sizeof(uint), uiTotalLen-sizeof(uint));
    return pdu;
}

PDU *MyTcpSocket::handleMsg(PDU *pdu)
{
    if (pdu == NULL){
        return NULL;
    }
    //打印并释放
    qDebug() << "handleMsg uiTotalLen" << pdu->uiTotalLen
             << "uiMsgLen" << pdu->uiMsgLen
             << "uiType" << pdu->uiType
             << "caData" << pdu->caData
             << "caData+32" << pdu->caData+32
             << "caMsg" << pdu->caMsg;
    m_pmh->pdu = pdu;
    PDU* respdu = NULL;
    switch(pdu->uiType) {
    case ENUM_TYPE_REGIST_REQ: {
        respdu = m_pmh->handleRegist();
        break;
    }
    case ENUM_TYPE_LOGIN_REQ: {
        respdu = m_pmh->handleLogin(m_strLoginName);
        break;
    }
    case ENUM_TYPE_FIND_USER_REQ: {
        respdu = m_pmh->handleFindUser();
        break;
    }
    case ENUM_TYPE_ONLINE_USER_REQ: {
        respdu = m_pmh->handleOnlineUser();
        break;
    }
    case ENUM_TYPE_ADJUDGE_ADD_FRIEND_REQ: {
        respdu = m_pmh->handleAdjudgeAddFriend();
        break;
    }
    case ENUM_TYPE_AGREE_ADD_FRIEND_REQ: {
        respdu = m_pmh->handleAgreeAddFriend();
        break;
    }
    case ENUM_TYPE_FLUSH_FRIEND_REQ: {
        respdu = m_pmh->handleFlushFriend();
        break;
    }
    case ENUM_TYPE_DELETE_FRIEND_REQ: {
        respdu = m_pmh->handleDeleteFriend();
        break;
    }
    case ENUM_TYPE_CHAT_REQ: {
        m_pmh->handleChat();
        break;
    }
    case ENUM_TYPE_MKDIR_REQ: {
        respdu = m_pmh->handleMkdir();
        break;
    }
    case ENUM_TYPE_FLUSH_FILE_REQ: {
        respdu = m_pmh->handleFlushFile();
        break;
    }
    case ENUM_TYPE_DEL_FILE_REQ: {
        respdu = m_pmh->handleDelFile();
        break;
    }
    case ENUM_TYPE_RENAME_FILE_REQ: {
        respdu = m_pmh->handleRenameFile();
        break;
    }
    case ENUM_TYPE_INIT_UPLOAD_FILE_REQ: {
        respdu = m_pmh->handleInitUploadFile();
        break;
    }
    case ENUM_TYPE_DATA_UPLOAD_FILE_REQ: {
        respdu = m_pmh->handleDataUploadFile();
        break;
    }
    case ENUM_TYPE_INIT_SHARE_FILE_REQ: {
        respdu = m_pmh->handleInitShareFile();
        break;
    }
    case ENUM_TYPE_AGREE_SHARE_FILE_REQ: {
        respdu = m_pmh->handleAgreeShareFile();
        break;
    }
    default:
        break;
    }
    return respdu;
}

void MyTcpSocket::clientOffline()
{
    OperateDB::getInstance().handleOffline(m_strLoginName.toStdString().c_str());
    MyTcpServer::getInstance().removeSocket(this);
}

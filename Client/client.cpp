#include "client.h"
#include "index.h"
#include "protocol.h"
#include "ui_client.h"

#include <QFile>
#include <QDebug>
#include <QHostAddress>
#include <QMessageBox>

Client::Client(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Client)
{
    ui->setupUi(this);
    loadConfig();
    connect(&m_socket, &QTcpSocket::connected, this, &Client::showConnected);
    connect(&m_socket, &QTcpSocket::readyRead, this, &Client::recvMsg);
    m_socket.connectToHost(QHostAddress(m_strIP), m_usPort);
    m_prh = new ResHandler;
}

Client::~Client()
{
    delete ui;
    delete m_prh;
}

void Client::loadConfig()
{
    QFile file(":/client.config"); //配置文件固定冒号开头
    if (file.open(QIODevice::ReadOnly)) {
        QString strData = QString(file.readAll());
        qDebug() << "strData" << strData;
        QStringList strList = strData.split("\r\n");
        m_strIP = strList[0];
        m_usPort = strList[1].toUShort();
        m_strRootPath = strList[2];
        qDebug() << "strIP" << m_strIP << "usPort" << m_usPort << "m_strRootPath" << m_strRootPath;
        file.close();
    } else {
        qDebug() << "打开文件失败";
    }
}

Client &Client::getInstance()
{
    static Client instance;
    return instance;
}

void Client::sendMsg(PDU *pdu)
{
    if (pdu == NULL)
    {
        return;
    }

    m_socket.write((char*)pdu, pdu->uiTotalLen);
    qDebug() << "send msg uiTotalLen" << pdu->uiTotalLen
             << "uiMsgLen" << pdu->uiMsgLen
             << "uiType" << pdu->uiType
             << "caData" << pdu->caData
             << "caData+32" << pdu->caData+32
             << "caMsg" << pdu->caMsg;
    free(pdu);
    pdu = NULL;
}

PDU *Client::readPDU()
{
    qDebug() << "recvMsg 接受消息长度" << m_socket.bytesAvailable();
    //先将总长度读出来, 确定消息长度
    uint uiTotalLen = 0;
    m_socket.read((char*)&uiTotalLen, sizeof(uint));
    //根据总长度计算得到柔性数组长度, 再初始化pdu
    uint uiMsgLen = uiTotalLen - sizeof(PDU);
    PDU* pdu = initPDU(uiMsgLen);
    //将pdu放入read函数进行读取, 需要偏移
    m_socket.read((char*)pdu+sizeof(uint),
    uiTotalLen-sizeof(uint));
    return pdu;
}

void Client::handleMsg(PDU *pdu)
{
    qDebug() << "recv msg uiTotalLen" << pdu->uiTotalLen
             << "uiMsgLen" << pdu->uiMsgLen
             << "uiType" << pdu->uiType
             << "caData" << pdu->caData
             << "caData+32" << pdu->caData+32
             << "caMsg" << pdu->caMsg;

    m_prh->pdu = pdu;
    switch(pdu->uiType) {
    case ENUM_TYPE_REGIST_RES: {
        m_prh->handleRegist();
        break;
    }
    case ENUM_TYPE_LOGIN_RES: {
        m_prh->handleLogin();
        break;
    }
    case ENUM_TYPE_FIND_USER_RES: {
        m_prh->handleFindUser();
        break;
    }
    case ENUM_TYPE_ONLINE_USER_RES: {
        m_prh->handleOnlineUser();
        break;
    }
    case ENUM_TYPE_ADJUDGE_ADD_FRIEND_RES: {
        m_prh->handleAdjudgeAddFriend();
        break;
    }
    case ENUM_TYPE_ADJUDGE_ADD_FRIEND_REQ: {
        m_prh->handleAdjudgeAddFriendResend();
        break;
    }
    case ENUM_TYPE_AGREE_ADD_FRIEND_RES: {
        m_prh->handleAgreeAddFriend();
        break;
    }
    case ENUM_TYPE_FLUSH_FRIEND_RES: {
        m_prh->handleFlushFriend();
        break;
    }
    case ENUM_TYPE_DELETE_FRIEND_RES: {
        m_prh->handleDeleteFriend();
        break;
    }
    case ENUM_TYPE_CHAT_REQ: {
        m_prh->handleChat();
        break;
    }
    case ENUM_TYPE_MKDIR_RES: {
        m_prh->handleMkdir();
        break;
    }
    case ENUM_TYPE_FLUSH_FILE_RES: {
        m_prh->handleFlushFile();
        break;
    }
    case ENUM_TYPE_DEL_FILE_RES: {
        m_prh->handleDelFile();
        break;
    }
    case ENUM_TYPE_RENAME_FILE_RES: {
        m_prh->handleRenameFile();
        break;
    }
    case ENUM_TYPE_INIT_UPLOAD_FILE_RES: {
        m_prh->handleInitUploadFile();
        break;
    }
    case ENUM_TYPE_DATA_UPLOAD_FILE_RES: {
        Index::getInstance().getFile()->flushFile();
        break;
    }
    case ENUM_TYPE_INIT_SHARE_FILE_RES: {
        QMessageBox::information(&Index::getInstance(), "提示", "分享文件请求已发送");
        break;
    }
    case ENUM_TYPE_INIT_SHARE_FILE_REQ: {
        m_prh->handleInitShareFile();
        break;
    }
    case ENUM_TYPE_AGREE_SHARE_FILE_REQ: {
        m_prh->handleAgreeShareFile();
        break;
    }
    default:
        break;
    }
}


void Client::showConnected()
{
    qDebug() << "连接服务器成功";
}

void Client::recvMsg()
{
    qDebug() << "recvMsg 接受消息长度" << m_socket.bytesAvailable();
    QByteArray data = m_socket.readAll();
    buffer.append(data);

    while (buffer.size() >= int(sizeof (PDU))) {
        PDU* pdu = (PDU*)buffer.data();
        if (buffer.size() < int(pdu->uiTotalLen)) {
            break;
        }
        handleMsg(pdu);
        buffer.remove(0, pdu->uiTotalLen);
    }
}

//void Client::on_send_PB_clicked()
//{
//    //获取输入框中的文字
//    QString strMsg = ui->input_LE->text();
//    qDebug() << "strMsg" << strMsg;
//    //初始化一个pdu，用于发送
//    PDU* pdu = initPDU(strMsg.toStdString().size());
//    //赋值caData和caMsg
//    pdu->uiType = ENUM_TYPE_MIN;
//    memcpy(pdu->caData, strMsg.toStdString().c_str(),
//           strMsg.toStdString().size());
//    memcpy(pdu->caMsg, strMsg.toStdString().c_str(),
//           strMsg.toStdString().size());

//    //发送并打印
//    m_socket.write((char*)pdu, pdu->uiTotalLen);
//    qDebug() << "send msg uiTotalLen" << pdu->uiTotalLen
//             << "uiMsgLen" << pdu->uiMsgLen
//             << "uiType" << pdu->uiType
//             << "caData" << pdu->caData
//             << "caData+32" << pdu->caData+32
//             << "caMsg" << pdu->caMsg;

//    free(pdu);
//    pdu = NULL;
//}

void Client::on_regist_PB_clicked()
{
    QString strName = ui->name_LE->text();
    QString strPwd = ui->pwd_LE->text();
    if (strName.isEmpty() || strPwd.isEmpty() || strName.toStdString().size() > 32 ||
strPwd.toStdString().size() > 32) {
        QMessageBox::information(this, "提示", "用户名或密码长度非法");
        return;
    }
    PDU* pdu = initPDU(0);
    pdu->uiType = ENUM_TYPE_REGIST_REQ;
    memcpy(pdu->caData, strName.toStdString().c_str(), 32);
    memcpy(pdu->caData+32, strPwd.toStdString().c_str(), 32);
    sendMsg(pdu);
}

void Client::on_login_PB_clicked()
{
    QString strName = ui->name_LE->text();
    QString strPwd = ui->pwd_LE->text();
    if (strName.isEmpty() || strPwd.isEmpty() ||
        strName.toStdString().size() > 32 ||
        strPwd.toStdString().size() > 32) {
        QMessageBox::information(this, "提示", "用户名或密码长度非法");
        return;
    }

    m_strLoginName = strName;
    PDU* pdu = initPDU(0);
    pdu->uiType = ENUM_TYPE_LOGIN_REQ;
    memcpy(pdu->caData, strName.toStdString().c_str(), 32);
    memcpy(pdu->caData+32, strPwd.toStdString().c_str(), 32);
    sendMsg(pdu);
}

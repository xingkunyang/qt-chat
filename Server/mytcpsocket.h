#ifndef MYTCPSOCKET_H
#define MYTCPSOCKET_H

#include "msghandler.h"
#include "protocol.h"

#include <QObject>
#include <QTcpSocket>

class MyTcpSocket : public QTcpSocket
{
    Q_OBJECT
public:
    MyTcpSocket();
    ~MyTcpSocket();
    void recvMsg();
    QString m_strLoginName;
    void sendMsg(PDU* pdu);
    PDU* readPDU();
    PDU* handleMsg(PDU* pdu);
    MsgHandler* m_pmh;
    QByteArray buffer;
public slots:
    void clientOffline();
};

#endif // MYTCPSOCKET_H

#include "clienttask.h"
#include "mytcpserver.h"

MyTcpServer &MyTcpServer::getInstance()
{
    static MyTcpServer instance;
    return instance;
}

void MyTcpServer::incomingConnection(qintptr handle)
{
    qDebug() << "新客户端连接";
    MyTcpSocket* pSocket = new MyTcpSocket;
    pSocket->setSocketDescriptor(handle);
    m_tcpSocketList.append(pSocket);
    ClientTask* task = new ClientTask(pSocket);
    threadPool.start(task);
}

void MyTcpServer::removeSocket(MyTcpSocket *socket)
{
    m_tcpSocketList.removeOne(socket);
    socket->deleteLater();
    socket = NULL;
    qDebug() << "m_tcpSocketList.size()" << m_tcpSocketList.size();
}

void MyTcpServer::resend(char *strName, PDU *pdu)
{
    if (strName == NULL || pdu == NULL)
    {
        return;
    }
    for (int i=0; i<m_tcpSocketList.size(); i++)
    {
        if (m_tcpSocketList[i]->m_strLoginName == strName)
        {
            m_tcpSocketList[i]->write((char*)pdu, pdu->uiTotalLen);
            qDebug() << "resend msg uiTotalLen" << pdu->uiTotalLen
                     << "uiMsgLen" << pdu->uiMsgLen
                     << "uiType" << pdu->uiType
                     << "caData" << pdu->caData
                     << "caData+32" << pdu->caData+32
                     << "caMsg" << pdu->caMsg;
        }
    }
}

MyTcpServer::MyTcpServer()
{
    threadPool.setMaxThreadCount(8);
}

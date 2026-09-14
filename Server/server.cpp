#include "mytcpserver.h"
#include "server.h"
#include "ui_server.h"

#include <QFile>
#include <QDebug>

Server::Server(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Server)
{
    ui->setupUi(this);
    loadConfig();
    MyTcpServer::getInstance().listen(QHostAddress(m_strIP),m_usPort);
}

Server::~Server()
{
    delete ui;
}

void Server::loadConfig()
{
    QFile file(":/server.config"); //配置文件固定冒号开头
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

Server &Server::getInstance()
{
    static Server instance;
    return instance;
}




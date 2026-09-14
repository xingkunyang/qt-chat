#include "client.h"
#include "onlineuser.h"
#include "ui_onlineuser.h"

OnlineUser::OnlineUser(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OnlineUser)
{
    ui->setupUi(this);
}

OnlineUser::~OnlineUser()
{
    delete ui;
}

void OnlineUser::updateListWidget(QStringList slName)
{
    ui->listWidget->clear();
    ui->listWidget->addItems(slName);
}

void OnlineUser::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
    QString strCurName = Client::getInstance().m_strLoginName;
    QString strTarName = item->text();
    PDU* pdu = initPDU(0);
    pdu->uiType = ENUM_TYPE_ADJUDGE_ADD_FRIEND_REQ;
    memcpy(pdu->caData, strCurName.toStdString().c_str(), 32);
    memcpy(pdu->caData+32, strTarName.toStdString().c_str(), 32);
    Client::getInstance().sendMsg(pdu);
}

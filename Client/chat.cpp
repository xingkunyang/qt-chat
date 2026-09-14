#include "chat.h"
#include "client.h"
#include "ui_chat.h"

Chat::Chat(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Chat)
{
    ui->setupUi(this);
}

Chat::~Chat()
{
    delete ui;
}

void Chat::updateShow_TE(QString strChatMsg)
{
    ui->show_TE->append(strChatMsg);
}

void Chat::on_send_PB_clicked()
{
    QString strMsg = ui->input_LE->text();
    if (strMsg.isEmpty()) {
        return;
    }

    ui->input_LE->clear();
    QString strCurName = Client::getInstance().m_strLoginName;
    PDU* pdu = initPDU(strMsg.toStdString().size()+1);
    pdu->uiType = ENUM_TYPE_CHAT_REQ;
    memcpy(pdu->caData, strCurName.toStdString().c_str(), 32);
    memcpy(pdu->caData+32, m_strChatName.toStdString().c_str(), 32);
    memcpy(pdu->caMsg, strMsg.toStdString().c_str(), strMsg.toStdString().size());
    Client::getInstance().sendMsg(pdu);
}

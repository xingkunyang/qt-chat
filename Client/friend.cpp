#include "client.h"
#include "friend.h"
#include "protocol.h"
#include "ui_friend.h"

#include <QInputDialog>
#include <QMessageBox>

Friend::Friend(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Friend)
{
    ui->setupUi(this);
    m_pOnlineUser = new OnlineUser;
    m_pChat = new Chat;
}

Friend::~Friend()
{
    delete ui;
    delete m_pOnlineUser;
    delete m_pChat;
}

void Friend::flushFriend()
{
    QString strName = Client::getInstance().m_strLoginName;
    PDU* pdu = initPDU(0);
    pdu->uiType = ENUM_TYPE_FLUSH_FRIEND_REQ;
    memcpy(pdu->caData, strName.toStdString().c_str(), 32);
    Client::getInstance().sendMsg(pdu);
}

void Friend::updateListWidget(QStringList slName)
{
    ui->listWidget->clear();
    ui->listWidget->addItems(slName);
}

void Friend::on_findUser_PB_clicked()
{
    QString strName = QInputDialog::getText(this, "查找用户", "用户名:");

    if (strName.isEmpty() || strName.toStdString().size() > 32) {
        QMessageBox::information(this, "提示", "用户名长度非法");
        return;
    }

    PDU* pdu = initPDU(0);
    pdu->uiType = ENUM_TYPE_FIND_USER_REQ;
    memcpy(pdu->caData, strName.toStdString().c_str(), 32);
    Client::getInstance().sendMsg(pdu);
}

void Friend::on_onlineUser_PB_clicked()
{
    PDU* pdu = initPDU(0);
    pdu->uiType = ENUM_TYPE_ONLINE_USER_REQ;
    Client::getInstance().sendMsg(pdu);
    m_pOnlineUser->show();
}

void Friend::on_update_PB_clicked()
{
    flushFriend();
}

void Friend::on_delete_PB_clicked()
{
    QListWidgetItem* pItem = ui->listWidget->currentItem();
    if (!pItem) {
        QMessageBox::information(this, "提示", "请选择要删除的好友");
        return;
    }

    QString strCurName = Client::getInstance().m_strLoginName;
    QString strTarName = pItem->text();
    int ret = QMessageBox::question(this, "提示", QString("是否确定删除 %1 好友?").arg(strTarName));
    if (ret != QMessageBox::Yes) {
        return;
    }

    PDU* pdu = initPDU(0);
    pdu->uiType = ENUM_TYPE_DELETE_FRIEND_REQ;
    memcpy(pdu->caData, strCurName.toStdString().c_str(), 32);
    memcpy(pdu->caData + 32, strTarName.toStdString().c_str(), 32);
    Client::getInstance().sendMsg(pdu);
}

void Friend::on_chat_PB_clicked()
{
    QListWidgetItem* pItem = ui->listWidget->currentItem();
    if (!pItem) {
        QMessageBox::information(this, "提示", "请选择要聊天的好友");
        return;
    }
    if (m_pChat->isHidden()) {
        m_pChat->show();
    }
    m_pChat->m_strChatName = pItem->text();
    m_pChat->setWindowTitle(QString("与 %1 聊天:").arg(m_pChat->m_strChatName));
}

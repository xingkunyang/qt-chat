#include "client.h"
#include "sharefile.h"
#include "ui_sharefile.h"

ShareFile::ShareFile(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ShareFile)
{
    ui->setupUi(this);
}

void ShareFile::updateListWidget(QStringList slName)
{
    ui->listWidget->clear();
    ui->listWidget->addItems(slName);
}

ShareFile::~ShareFile()
{
    delete ui;
}

void ShareFile::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
    QString strFriendName = item->text();
    QString strCurName = Client::getInstance().m_strLoginName;
    PDU *pdu = initPDU(m_strShareFilePath.toStdString().size()+1);
    pdu->uiType = ENUM_TYPE_INIT_SHARE_FILE_REQ;
    memcpy(pdu->caData, strCurName.toStdString().c_str(), 32);
    memcpy(pdu->caData+32, strFriendName.toStdString().c_str(), 32);

    memcpy(pdu->caMsg, m_strShareFilePath.toStdString().c_str(),
           m_strShareFilePath.toStdString().size());
    Client::getInstance().sendMsg(pdu);
}

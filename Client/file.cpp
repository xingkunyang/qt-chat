#include "client.h"
#include "file.h"
#include "ui_file.h"
#include "uploader.h"

#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>

File::File(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::File)
{
    ui->setupUi(this);
    m_strUserPath= QString("%1/%2")
        .arg(Client::getInstance().m_strRootPath)
        .arg(Client::getInstance().m_strLoginName);
    m_strCurPath = m_strUserPath;
    m_pShareFile = new ShareFile;
}

File::~File()
{
    delete ui;
    delete m_pShareFile;
}

void File::flushFile()
{
    PDU* pdu = initPDU(m_strCurPath.toStdString().size()+1);
    memcpy(pdu->caMsg, m_strCurPath.toStdString().c_str(),
           m_strCurPath.toStdString().size());
    pdu->uiType = ENUM_TYPE_FLUSH_FILE_REQ;
    Client::getInstance().sendMsg(pdu);
}

void File::updateFileList(QList<FileInfo *> pFileList)
{
    foreach (FileInfo* pFileInfo, m_pFileList) {
        delete pFileInfo;
    }
    m_pFileList = pFileList;
    ui->listWidget->clear();
    foreach (FileInfo* pFileInfo, pFileList) {
        QListWidgetItem* pItem = new QListWidgetItem;
        pItem->setText(pFileInfo->caName);
        if (pFileInfo->uiType == 0) {
            pItem->setIcon(QIcon(QPixmap(":/dir.png")));
        } else if (pFileInfo->uiType == 1) {
            pItem->setIcon(QIcon(QPixmap(":/file.png")));
        }
        ui->listWidget->addItem(pItem);
    }
}

void File::on_mkdir_PB_clicked()
{
    QString strNewDir = QInputDialog::getText(this, "新建文件夹", "文件夹名");
    if (strNewDir.isEmpty() || strNewDir.toStdString().size() > 32) {
        QMessageBox::information(this, "新建文件夹", "文件夹名长度非法");
        return;
    }

    PDU* pdu = initPDU(m_strCurPath.toStdString().size()+1);
    pdu->uiType = ENUM_TYPE_MKDIR_REQ;
    memcpy(pdu->caData, strNewDir.toStdString().c_str(), 32);
    memcpy(pdu->caMsg, m_strCurPath.toStdString().c_str(), m_strCurPath.toStdString().size());
    Client::getInstance().sendMsg(pdu);
}


void File::on_flush_PB_clicked()
{
    flushFile();
}

void File::on_delete_PB_clicked()
{
    QListWidgetItem* pItem = ui->listWidget->currentItem();
    if (pItem == NULL) {
        return;
    }

    QString strDelName = pItem->text();
    int res = QMessageBox::question(this, "删除文件", QString("是否确认删除文件 %1?").arg(strDelName));
    if (res != QMessageBox::Yes) {
        return;
    }

    PDU* pdu = initPDU(m_strCurPath.toStdString().size()+1);
    pdu->uiType = ENUM_TYPE_DEL_FILE_REQ;
    memcpy(pdu->caData, strDelName.toStdString().c_str(), 32);
    memcpy(pdu->caMsg, m_strCurPath.toStdString().c_str(),
           m_strCurPath.toStdString().size());

    Client::getInstance().sendMsg(pdu);
}

void File::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
    QString strDirName = item->text();

    foreach(FileInfo* pFileInfo, m_pFileList) {
        if (strDirName == pFileInfo->caName && pFileInfo->uiType != 0) {
            return;
        }
    }

    m_strCurPath =
    QString("%1/%2").arg(m_strCurPath).arg(strDirName);
    flushFile();
}

void File::on_return_PB_clicked()
{
    if (m_strCurPath == m_strUserPath) {
        return;
    }

    int index = m_strCurPath.lastIndexOf('/');
    m_strCurPath.remove(index, m_strCurPath.size() - index);
    flushFile();
}

void File::on_rename_PB_clicked()
{
    QListWidgetItem* pItem = ui->listWidget->currentItem();
    if (pItem == NULL) {
        return;
    }
    QString strOldName = pItem->text();
    QString strNewName = QInputDialog::getText(this, "重命名", "新文件名");
    if (strNewName.isEmpty() || strNewName.toStdString().size() > 32) {
        QMessageBox::information(this, "重命名", "文件名长度非法");
        return;
    }
    PDU* pdu = initPDU(m_strCurPath.toStdString().size()+1);
    pdu->uiType = ENUM_TYPE_RENAME_FILE_REQ;
    memcpy(pdu->caData, strOldName.toStdString().c_str(), 32);
    memcpy(pdu->caData+32, strNewName.toStdString().c_str(), 32);
    memcpy(pdu->caMsg, m_strCurPath.toStdString().c_str(), m_strCurPath.toStdString().size());
    Client::getInstance().sendMsg(pdu);
}

void File::on_upload_PB_clicked()
{
    m_strUploadPath = QFileDialog::getOpenFileName();
    if (m_strUploadPath.isEmpty()) {
        return;
    }
    PDU* pdu = initPDU(m_strCurPath.toStdString().size()+1);
    pdu->uiType = ENUM_TYPE_INIT_UPLOAD_FILE_REQ;
    int index = m_strUploadPath.lastIndexOf('/');
    QString strFileName =
    m_strUploadPath.right(m_strUploadPath.size() - index - 1);
    QFile file(m_strUploadPath);
    qint64 iFileSize = file.size();
    memcpy(pdu->caData, strFileName.toStdString().c_str(), 32);
    memcpy(pdu->caData+32, &iFileSize, sizeof(qint64));
    memcpy(pdu->caMsg, m_strCurPath.toStdString().c_str(),
    m_strCurPath.toStdString().size());
    Client::getInstance().sendMsg(pdu);
}

void File::uploadFile()
{
    Uploader* uploader = new Uploader(m_strUploadPath);
    connect(uploader, &Uploader::errorMsg, this, &File::uploadErrorBox);
    connect(uploader, &Uploader::uploadPDU, this, &File::sendPDU);
    uploader->start();
}

void File::uploadErrorBox(QString strMsg)
{
    QMessageBox::information(this, "上传文件", strMsg);
}

void File::sendPDU(PDU *pdu)
{
    Client::getInstance().m_socket.write((char*)pdu, pdu->uiTotalLen);
    free(pdu);
    pdu = NULL;
}

void File::on_share_PB_clicked()
{
    QListWidgetItem* pItem = ui->listWidget->currentItem();
    if (pItem == NULL) {
        return;
    }
    m_pShareFile->m_strShareFilePath =
    QString("%1/%2").arg(m_strCurPath).arg(pItem->text());
    if (m_pShareFile->isHidden()) {
        m_pShareFile->show();
    }
}

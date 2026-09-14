#ifndef FILE_H
#define FILE_H

#include "protocol.h"
#include "sharefile.h"

#include <QListWidget>
#include <QWidget>

namespace Ui {
class File;
}

class File : public QWidget
{
    Q_OBJECT

public:
    explicit File(QWidget *parent = nullptr);
    ~File();
    QString m_strUserPath;
    QString m_strCurPath;
    QString m_strUploadPath;
    QList<FileInfo*> m_pFileList;
    ShareFile* m_pShareFile;
    void flushFile();
    void updateFileList(QList<FileInfo*> pFileList);
    void uploadFile();

private slots:
    void on_mkdir_PB_clicked();

    void on_flush_PB_clicked();

    void on_delete_PB_clicked();

    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);

    void on_return_PB_clicked();

    void on_rename_PB_clicked();

    void on_upload_PB_clicked();

    void uploadErrorBox(QString strMsg);

    void sendPDU(PDU* pdu);

    void on_share_PB_clicked();

private:
    Ui::File *ui;
};
#endif // FILE_H

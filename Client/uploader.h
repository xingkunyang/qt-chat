#ifndef UPLOADER_H
#define UPLOADER_H

#include "protocol.h"

#include <QObject>

class Uploader : public QObject
{
    Q_OBJECT

public:
    Uploader(const QString strFilePath);
    QString m_strUploadPath;
    void start();

public slots:
    void uploadFile();

signals:
    void errorMsg(const QString strMsg);
    void uploadPDU(PDU* pdu);
    void finished();
};

#endif // UPLOADER_H

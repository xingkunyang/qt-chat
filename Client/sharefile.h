#ifndef SHAREFILE_H
#define SHAREFILE_H

#include <QListWidget>
#include <QWidget>

namespace Ui {
class ShareFile;
}

class ShareFile : public QWidget
{
    Q_OBJECT

public:
    explicit ShareFile(QWidget *parent = nullptr);
    QString m_strShareFilePath;
    void updateListWidget(QStringList slName);
    ~ShareFile();

private slots:
    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);

private:
    Ui::ShareFile *ui;
};


#endif // SHAREFILE_H

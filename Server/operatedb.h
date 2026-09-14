#ifndef OPERATEDB_H
#define OPERATEDB_H

#include <QObject>
#include <QSqlDatabase>

class OperateDB : public QObject
{
    Q_OBJECT
public:
    static OperateDB& getInstance();
    void connect();
    ~OperateDB();

    bool handleRegist(const char* caName, const char* caPwd);
    bool handleLogin(const char* caName, const char* caPwd);
    void handleOffline(const char* caName);
    int handleFindUser(const char* caName);
    QStringList handleOnlineUser();
    int handleAdjudgeAddFriend(const char* caCurName, const char* caTarName);
    bool handleAgreeAddFriend(const char* caCurName, const char* caTarName);
    QStringList handleFlushFriend(const char* caName);
    bool handleDeleteFriend(const char* caCurName, const char* caTarName);

private:
    explicit OperateDB(QObject *parent = nullptr);
    OperateDB(const OperateDB& instance) = delete;
    OperateDB& operator=(const OperateDB&) = delete;

    QSqlDatabase m_db;

signals:
};

#endif // OPERATEDB_H

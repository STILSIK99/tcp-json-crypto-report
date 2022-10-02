#ifndef MANEGER_H
#define MANEGER_H

#include <QWidget>
#include <QRegExpValidator>
#include <QRegExp>
#include <QJsonArray>
#include <QFileDialog>
#include "server.h"
#include <QAxObject>
#include <QMessageBox>
#include <QVariant>
#include <QtAlgorithms>
#include <QtMath>
#include <QSql>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QTimer>
#include <QFile>
#include <QTextStream>
#include <QtAlgorithms>


namespace Ui {
class Maneger;
}

class Maneger : public QWidget
{
    Q_OBJECT

public:
    explicit Maneger(QWidget *parent = nullptr);
    ~Maneger();
    bool createDataBase (QString path);
    Server *server;
    QString ip;
    QPair<QString, int> curAg;
    QVector<QPair<QString, int>> agents;

private slots:
    QString JsonToString (QJsonObject *a);
    QJsonObject StringToJson (QString a);
    void appendline (QJsonObject &, QPair<QString, int> &);

public slots:
    void slotSendRequest (bool);
    void slotAgentConnect (QPair<QString, int>);
    void slotAgentDisconnect (QPair<QString, int>);
    void slotOperationFinish (bool, QPair<QString, int>);
    void slotProcess (QPair<QString, int>, QJsonObject);

private:
    Ui::Maneger *ui;
    QSqlQuery *query;
    QAxObject *pword;
    QAxObject *prange;

signals:
    void makeConnection (QPair<QString, int>);
    void sendMessage (QString, QPair<QString, int>);

private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_3_clicked();
};
#endif // MAINWINDOW_H

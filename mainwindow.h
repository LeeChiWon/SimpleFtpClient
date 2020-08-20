#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_Download_clicked();
    void on_pushButton_Upload_clicked();
    void on_toolButton_LocalPath_clicked();

    void readyRead();
    void replyFinished(QNetworkReply*);
    void loadError(QNetworkReply::NetworkError);
    void loadProgress(qint64 bytesSent,qint64 bytesTotal);

private:
    Ui::MainWindow *ui;
    QNetworkReply *reply=nullptr;
    QFile *file=nullptr;

    bool checkUrl();
    bool checkPath();
    void buttonsEnabled(bool clicked);
    void startFtpClient(const int &select);

};

#endif // MAINWINDOW_H

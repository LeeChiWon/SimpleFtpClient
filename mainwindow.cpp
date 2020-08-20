#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->progressBar->setValue(0);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_Download_clicked()
{
    if(!checkUrl())
    {
        QMessageBox::warning(this,"SimpleFtpClient","Check the Settings.");
        return;
    }

    if(!checkPath())
    {
        QMessageBox::warning(this,"SimpleFtpClient","Check the path");
        return;
    }

    ui->textEdit_Console->append("Ftp downloading...");
    startFtpClient(0);
}

void MainWindow::on_pushButton_Upload_clicked()
{
    if(!checkUrl())
    {
        QMessageBox::warning(this,"SimpleFtpClient","Check the Settings.");
        return;
    }

    if(!checkPath())
    {
        QMessageBox::warning(this,"SimpleFtpClient","Check the path");
        return;
    }

    QFileInfo fileInfo(ui->lineEdit_LocalPath->text());

    if(!fileInfo.exists())
    {
        QMessageBox::warning(this,"SimpleFtpClient","Uploadfile is not exist.");
        return;
    }

    ui->textEdit_Console->append("Ftp uploading...");
    startFtpClient(1);
}

bool MainWindow::checkUrl()
{
    if(ui->lineEdit_Server->text().isEmpty() || ui->lineEdit_Port->text().isEmpty()
            || ui->lineEdit_Username->text().isEmpty() || ui->lineEdit_Password->text().isEmpty())
    {
        return false;
    }
    return true;
}

bool MainWindow::checkPath()
{
    if(ui->lineEdit_FTPPath->text().isEmpty() || ui->lineEdit_LocalPath->text().isEmpty())
    {
        return false;
    }
    return true;
}

void MainWindow::buttonsEnabled(bool clicked) //during download & upload button enable set
{
    ui->pushButton_Download->setEnabled(!clicked);
    ui->pushButton_Upload->setEnabled(!clicked);
    ui->toolButton_LocalPath->setEnabled(!clicked);
}

void MainWindow::startFtpClient(const int &select)
{
    ui->progressBar->setValue(0);
    buttonsEnabled(true);

    QNetworkAccessManager *accessManager=new QNetworkAccessManager(this);
    accessManager->setNetworkAccessible(QNetworkAccessManager::Accessible);

//    QString path=QString("ftp://%1%2").arg(ui->lineEdit_Server->text(),ui->lineEdit_FTPPath->text());
//    QUrl url(path);

//    url.setPort(ui->lineEdit_Port->text().toInt());
//    url.setUserName(ui->lineEdit_Username->text());
//    url.setPassword(ui->lineEdit_Password->text());
    QUrl url;
    url.setScheme("ftp");
    url.setHost(ui->lineEdit_Server->text());
    url.setPath(ui->lineEdit_FTPPath->text());
    url.setPort(ui->lineEdit_Port->text().toInt());
    url.setUserName(ui->lineEdit_Username->text());
    url.setPassword(ui->lineEdit_Password->text());

    QByteArray byte_file;
    QNetworkRequest request(url);

    switch (select) {
    case 0: //download
        file=new QFile(ui->lineEdit_LocalPath->text());
        file->open(QIODevice::WriteOnly);
        reply=accessManager->get(request);

        connect((QObject *)reply, SIGNAL(readyRead()), SLOT(readyRead()));
        connect(reply, SIGNAL(downloadProgress(qint64,qint64)), SLOT(loadProgress(qint64 ,qint64)));
        break;
    default: //upload                
        file=new QFile(ui->lineEdit_LocalPath->text());
        file->open(QIODevice::ReadOnly);
        byte_file=file->readAll();
        reply = accessManager->put(request, byte_file);

        connect(reply, SIGNAL(uploadProgress(qint64 ,qint64)), SLOT(loadProgress(qint64 ,qint64)));
        break;
    }

    connect(accessManager, SIGNAL(finished(QNetworkReply*)), SLOT(replyFinished(QNetworkReply*)));
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),SLOT(loadError(QNetworkReply::NetworkError)));    
}

void MainWindow::on_toolButton_LocalPath_clicked() // download & upload set local path
{
    ui->lineEdit_LocalPath->setText(QFileDialog::getSaveFileName(this,"Save File",QApplication::applicationDirPath()));
}

void MainWindow::readyRead() //create file
{
    file->write(reply->readAll());
}

void MainWindow::replyFinished(QNetworkReply*)    //Delete pointer, update and close file
{    
    if(reply->error() == QNetworkReply::NoError)
    {        
        ui->textEdit_Console->append("Ftp complete.");
    }
    else
    {        
        ui->textEdit_Console->append(reply->errorString());
    }

    reply->deleteLater();
    file->flush();
    file->close();
    file->deleteLater();
    buttonsEnabled(false);
}

void MainWindow::loadProgress(qint64 bytesSent, qint64 bytesTotal)    //Update progress bar
{
    ui->label_Byte->setText(QString("%1 / %2").arg(bytesSent).arg(bytesTotal));
    ui->progressBar->setMaximum(bytesTotal); //Max
    ui->progressBar->setValue(bytesSent);  //The current value
}

void MainWindow::loadError(QNetworkReply::NetworkError)    //Error output during transmission
{
    ui->textEdit_Console->append(reply->errorString());
    qDebug()<<"Error: "<<reply->error();
}


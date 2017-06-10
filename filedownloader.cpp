#include "filedownloader.h"

FileDownloader::FileDownloader(QList<QString> list, QObject *parent) : QObject(parent)
{
    _downloadedCounter = 0;
    _filesName = list;
    connect(&_webCtrl, SIGNAL (finished(QNetworkReply*)), this, SLOT (fileDownloaded(QNetworkReply*)));
}

FileDownloader::FileDownloader(QString url, QObject *parent) : QObject(parent)
{
    _downloadedCounter = 0;
    _filesName.append(url);
    connect(&_webCtrl, SIGNAL (finished(QNetworkReply*)), this, SLOT (fileDownloaded(QNetworkReply*)));
}


FileDownloader::FileDownloader(QObject *parent) :  QObject(parent)
{
    _downloadedCounter = 0;
    connect(&_webCtrl, SIGNAL (finished(QNetworkReply*)), this, SLOT (fileDownloaded(QNetworkReply*)));
}

FileDownloader::~FileDownloader() { }


void FileDownloader::fileDownloaded(QNetworkReply* pReply)
{
    _downloadedData = pReply->readAll();

    //emit a signal
    pReply->deleteLater();
    emit downloaded();
}

QByteArray FileDownloader::GetDownloadedDatas() const
{
    return _downloadedData;
}

bool FileDownloader::StartNextDownload()
{
    if( _filesName.length() == _downloadedCounter ) return false;
    QNetworkRequest request(_filesName.at(_downloadedCounter++));
    _webCtrl.get(request);
    return true;
}

void FileDownloader::AddFileUrl(QString fileUrl)
{
    _filesName.append(fileUrl);
}
void FileDownloader::Clear()
{
    _downloadedCounter = 0;
    _filesName.clear();
}

int FileDownloader::GetCurrentIndex()
{
    return _downloadedCounter - 1;
}



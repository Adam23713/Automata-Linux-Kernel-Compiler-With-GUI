#ifndef FILEDOWNLOADER_H
#define FILEDOWNLOADER_H

#include <QObject>
#include <QByteArray>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

class FileDownloader : public QObject
{
    Q_OBJECT

public:
    explicit FileDownloader( QList<QString> list, QObject *parent = 0);
    explicit FileDownloader(QString url, QObject *parent = 0);
    explicit FileDownloader(QObject *parent = 0);
    void AddFileUrl(QString fileUrl);
    void Clear();
    int GetCurrentIndex();
    bool StartNextDownload();
    virtual ~FileDownloader();
    QByteArray GetDownloadedDatas() const;

signals:
    void downloaded();

private slots:
    void fileDownloaded(QNetworkReply* pReply);

private:
    int _downloadedCounter;
    QList<QString> _filesName;
    QNetworkAccessManager _webCtrl;
    QByteArray _downloadedData;

};

#endif // FILEDOWNLOADER_H

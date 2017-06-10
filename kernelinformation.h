#ifndef KERNELINFORMATION_H
#define KERNELINFORMATION_H

#include <QMap>
#include <QObject>
#include <QRegExp>
#include <QEventLoop>
#include <QNetworkAccessManager>
#include <QNetworkReply>

class KernelInformation : public QObject
{
    Q_OBJECT

private:
    QMap<QString,QString> webPagesSourceCode;
    QMap<QString,QList<QString>> datasFromWeb;

public:
    explicit KernelInformation(QObject *parent = 0);
    QList<QString> getDataToSource(QString sourceName, QString regexp);
    bool downloadHtmlSourceCodeTo(QString sourceName, QString webPageUrl);
    QString getWebPageSource(QString sourceName);

signals:

public slots:
};

#endif // KERNELINFORMATION_H

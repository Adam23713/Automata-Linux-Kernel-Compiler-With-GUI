#include "kernelinformation.h"
#include <QDebug>

KernelInformation::KernelInformation(QObject *parent) : QObject(parent)
{

}

bool KernelInformation::downloadHtmlSourceCodeTo(QString sourceName, QString webPageUrl)
{

    //Ellenőrizni mert mindig lefut
    //if(QUrl(webPageUrl).isValid())
      //  return false;

    QNetworkAccessManager manager;
    QNetworkReply *response = manager.get(QNetworkRequest(QUrl(webPageUrl)));
    QEventLoop event;

    connect(response,SIGNAL(finished()),&event,SLOT(quit()));


    event.exec();
    webPagesSourceCode[sourceName] = response->readAll();
   // qDebug() << webPagesSourceCode[sourceName];


    disconnect(response,SIGNAL(finished()),&event,SLOT(quit()));
    response->close();
    delete response;

    return webPagesSourceCode[sourceName].isEmpty() ? false : true;
}

QList<QString> KernelInformation::getDataToSource(QString sourceName, QString regexp)
{
    QList<QString> list;
    QRegExp rx(regexp);

    int pos = 0;
    while ((pos = rx.indexIn(webPagesSourceCode[sourceName], pos)) != -1)
    {
        list.push_back( rx.cap() );
        pos += rx.matchedLength();
    }

    datasFromWeb[sourceName] = list;

    return list;
}

QString KernelInformation::getWebPageSource(QString sourceName)
{
    return webPagesSourceCode[sourceName];
}

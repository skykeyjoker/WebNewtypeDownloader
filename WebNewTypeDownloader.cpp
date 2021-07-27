#include <iostream>
#include <fmt/core.h>
#include <fmt/printf.h>
#include <fmt/format.h>
#include <QObject>
#include <QCoreApplication>
#include <QtCore>
#include <QTextStream>
#include <QEventLoop>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonObject>
#include <QFile>
#include <QDir>
#include <QDebug>

const QString PREFIX = "https://comic.webnewtype.com";
typedef QVector<QString> UrlList;

QJsonDocument getJson(const QString &series, const QString &chapter)
{
	QJsonDocument ret;
	QNetworkAccessManager manager;
	QNetworkRequest request;
	QString jsonUrl(QObject::tr("https://comic.webnewtype.com/contents/%1/%2/json/").arg(series).arg(chapter));
	qDebug() << jsonUrl;
	request.setUrl(QUrl(jsonUrl));

	QEventLoop eventLoop;
	QObject::connect(&manager, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));
	QNetworkReply* reply = manager.get(request);
	eventLoop.exec();

	if(reply->error()!=QNetworkReply::NoError)
	{
		fmt::print("Request Failed!\n");
		return ret;
	}

	QByteArray retbuf = reply->readAll();
	QJsonParseError jsonParseError;
	ret = QJsonDocument::fromJson(retbuf, &jsonParseError);
	if(jsonParseError.error!=QJsonParseError::NoError)
	{
		fmt::print("Read Json Failed!\n");
		return ret;
	}

	return ret;
}

UrlList getUrls(const QJsonDocument &doc)
{
	UrlList urls;
	
	QJsonArray arr = doc.array();
	for(const auto &item : arr)
	{
		QString url = PREFIX + item.toString();
		url = url.mid(0, url.lastIndexOf('/',url.size()-2));
		urls.push_back(url);
	}

	fmt::print("{}p founded!\n", urls.size());
	
	return urls;
}

void downLoad(const UrlList &urls, const QString& series, const QString& chapter)
{
	QDir dir(qApp->applicationDirPath());
	dir.mkdir(series);
	dir.cd(series);
	dir.mkdir(chapter);
	dir.cd(chapter);
	

	fmt::print("Start download...\n");
	int count = 1;
	
	for(const auto &url : urls)
	{
		fmt::print("Downloading {}...\n",url.toStdString());
		
		QNetworkAccessManager manager;
		QNetworkRequest request;
		request.setUrl(QUrl(url));
		
		QEventLoop eventLoop;
		QObject::connect(&manager, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));
		QNetworkReply* reply = manager.get(request);
		eventLoop.exec();

		if (reply->error() != QNetworkReply::NoError)
		{
			fmt::print("Download Failed at {}\n",url.toStdString());
		}

		QString fileName;
		fileName = dir.absolutePath() + "/" + QString::number(count)+".jpg";
		//qDebug() << fileName;
		QFile file(fileName);
		if(!file.open(QIODevice::WriteOnly))
		{
			fmt::print("Failed to write {}", fileName.toStdString());
		}
		file.write(reply->readAll());
		file.close();

		count++;
	}

	fmt::print("Download Completed!\n");
}

int main(int argc, char* argv[])
{
	QCoreApplication app(argc, argv);

	while (1)
	{
		QString series, chapter;
		QJsonDocument retJson;

		QTextStream qin(stdin);
		fmt::printf("Enter series: ");
		qin >> series;
		fmt::printf("Enter Chapter: ");
		qin >> chapter;

		retJson = getJson(series, chapter);

		UrlList urls = getUrls(retJson);

		downLoad(urls, series, chapter);
	}
	
	return app.exec();
}

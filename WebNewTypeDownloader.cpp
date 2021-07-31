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
#include <QThreadPool>
#include <QRunnable>
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

class downLoader: public QRunnable
{
public:
	downLoader(const QString& url, const QString& fileName): m_url(url), m_fileName(fileName)
	{}

	void run() override
	{
		fmt::print("Downloading {}...\n", m_url.toStdString());

		QNetworkAccessManager manager;
		QNetworkRequest request;
		request.setUrl(QUrl(m_url));

		QEventLoop eventLoop;
		QObject::connect(&manager, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));
		QNetworkReply* reply = manager.get(request);
		eventLoop.exec();

		if (reply->error() != QNetworkReply::NoError)
		{
			fmt::print("Download Failed at {}\n", m_url.toStdString());
		}

		QFile file(m_fileName);
		if (!file.open(QIODevice::WriteOnly))
		{
			fmt::print("Failed to write {}", m_fileName.toStdString());
		}
		file.write(reply->readAll());
		file.close();

		fmt::print("{} Download Completed!\n", m_fileName.toStdString());
	}

private:
	QString m_url;
	QString m_fileName;
};

int main(int argc, char* argv[])
{
	QCoreApplication app(argc, argv);

	QString series, chapter;
	QJsonDocument retJson;

	QTextStream qin(stdin);
	fmt::printf("Enter series: ");
	qin >> series;
	fmt::printf("Enter Chapter: ");
	qin >> chapter;

	// Create ouput Dir
	QDir dir(qApp->applicationDirPath());
	dir.mkdir(series);
	dir.cd(series);
	dir.mkdir(chapter);
	dir.cd(chapter);

	// Get Urls
	retJson = getJson(series, chapter);
	UrlList urls = getUrls(retJson);


	QString pathPre = dir.absolutePath() + "/";

	// Start Downloading
	QThreadPool threadpool;
	threadpool.setMaxThreadCount(4);

	int count = 0;
	for (const auto& url : urls)
	{
		QString fileName = pathPre + QString::number(++count) + ".jpg";
		threadpool.start(new downLoader(url, fileName));
	}

	threadpool.waitForDone();
	fmt::print("All Downloading Completed!\n");
	
	return app.exec();
}

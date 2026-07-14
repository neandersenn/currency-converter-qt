#include "ratesworker.h"
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

RatesWorker::RatesWorker(QObject *parent) : QObject(parent)
{

}

void RatesWorker::doWork(){

    if(!manager){
    manager = new QNetworkAccessManager();
    }

    QNetworkRequest request(QUrl("https://api.nbrb.by/exrates/rates?periodicity=0&limit=500"));
    QNetworkReply *reply = manager->get(request);

    connect(reply, &QNetworkReply::finished, [=]() {
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray data = reply->readAll();
            QJsonDocument doc = QJsonDocument::fromJson(data);

            if (doc.isArray()) {
                rates.clear();
                rates["BYN"] = 1.0;

                for (const QJsonValue &v : doc.array()) {
                    QJsonObject obj = v.toObject();
                    QString cur = obj["Cur_Abbreviation"].toString();
                    double rate = obj["Cur_OfficialRate"].toDouble();
                    int scale = obj["Cur_Scale"].toInt();

                    if (rate > 0 && scale > 0) {
                        rates[cur] = rate / scale;
                    }
                }
                emit rateReady(rates);
                emit resultRequest("Курсы обновлены ");
            } else {
                emit resultRequest("Ошибка неверный ответ");
            }
        } else {
            emit resultRequest("Ошибка загрузки проверьте интернет.");
        }
        reply->deleteLater();
    });
}

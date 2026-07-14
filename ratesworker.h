#ifndef RATESWORKER_H
#define RATESWORKER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QMap>

class RatesWorker : public QObject
{
    Q_OBJECT
public:
    RatesWorker(QObject *parent = nullptr);
    void doWork();

signals:
    void rateReady(QMap<QString, double>rates);
    void resultRequest(QString result);

private:
    QNetworkAccessManager *manager = nullptr;

    QMap<QString, double>rates;
};

#endif // RATESWORKER_H

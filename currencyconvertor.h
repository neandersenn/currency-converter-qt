#ifndef CURRENCYCONVERTOR_H
#define CURRENCYCONVERTOR_H

#include <QWidget>
#include <QNetworkAccessManager>
#include <QMap>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include "ratesworker.h"

class CurrencyConvertor : public QWidget{
    Q_OBJECT
public:
    explicit CurrencyConvertor(QWidget *parent = nullptr);
    ~CurrencyConvertor();

private slots:
    void onRateReady(QMap<QString, double>rates);
    void onResultRequest(QString result);

private:
    void setupUI();
    void fetchRates();
    void convert();

    QLabel *title;
    QLineEdit *amount;
    QComboBox *fromCombo;
    QComboBox *toCombo;
    QPushButton *convertBtn;
    QPushButton *refreshBtn;
    QLabel *resultLabel;
    QPushButton *swapBtn;
    QMap<QString, double>rates;

    RatesWorker *worker;
    QThread *thread;
};


#endif // CURRENCYCONVERTOR_H

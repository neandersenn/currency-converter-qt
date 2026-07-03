#ifndef CURRENCYCONVERTOR_H
#define CURRENCYCONVERTOR_H

#include <QWidget>
#include <QNetworkAccessManager>
#include <QMap>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>

class CurrencyConvertor : public QWidget{
    Q_OBJECT
public:
    explicit CurrencyConvertor(QWidget *parent = nullptr);
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

    QNetworkAccessManager *manager;
    QMap<QString, double>rates;
};


#endif // CURRENCYCONVERTOR_H

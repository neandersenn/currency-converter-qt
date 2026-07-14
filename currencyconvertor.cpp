#include "currencyconvertor.h"
#include <ratesworker.h>

#include <QThread>
#include <QApplication>
#include <QLabel>
#include <QWidget>
#include <QFrame>
#include <QGraphicsDropShadowEffect>
#include <QLineEdit>
#include <QDoubleValidator>
#include <QComboBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPlainTextEdit>
#include <QSvgRenderer>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTimer>
#include <QMap>
#include <QUrl>
#include <QSvgRenderer>
#include <QPainter>
#include <QPixmap>
#include <QCoreApplication>
#include <QTimer>

CurrencyConvertor::CurrencyConvertor(QWidget *parents) : QWidget(parents){

    worker = new RatesWorker();
    thread = new QThread(this);

    worker->moveToThread(thread);

    connect(worker, &RatesWorker::rateReady, this, &CurrencyConvertor::onRateReady);
    connect(worker, &RatesWorker::resultRequest, this, &CurrencyConvertor::onResultRequest);

    thread->start();

    setupUI();
    QTimer::singleShot(0, worker, &RatesWorker::doWork);
}

void CurrencyConvertor::setupUI(){

    QHBoxLayout *windowLayout = new QHBoxLayout(this);

    QFrame *card = new QFrame(this);
    card->setFixedSize(600, 400);
    card->setStyleSheet(
        "background-color: #ffffff;"
        "border-radius: 16px;"
    );

    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(card);
    shadow->setBlurRadius(30);
    shadow->setOffset(0, 4);
    shadow->setColor(QColor(0, 0, 0, 30));
    card->setGraphicsEffect(shadow);


    title = new QLabel("Currency convertor", card);

    title->setStyleSheet(
        "color: #1a1a1a;"
        "font-size: 24px;"
        "font-weight: bold;"
    );

    title->setAlignment(Qt::AlignCenter);

    amount = new QLineEdit(card);
    amount->setPlaceholderText("Amount");
    amount->setFixedWidth(240);

    amount->setValidator(new QDoubleValidator(0, 1e12, 6, amount));

    amount->setStyleSheet(
        "background-color: #ffffff;"
        "color: #1e293b;"
        "border: 2px solid #d1d5db;"
        "border-radius: 10px;"
        "padding: 12px 16px;"
        "font-size: 16px;"
    );


    fromCombo = new QComboBox(card);
    toCombo = new QComboBox(card);

    auto styleCombo = [&](QComboBox *c){
        const QStringList list = {"USD", "EUR", "RUB", "BYN", "GBP", "CNY", "KZT"};
        c->addItems(list);
        c->setEditable(false);
        c->setStyleSheet(
            "QComboBox {"
            "    background-color: #ffffff;"
            "    color: #1e293b;"
            "    border: 2px solid #d1d5db;"
            "    border-radius: 10px;"
            "    padding: 10px 14px;"
            "    font-size: 15px;"
            "}"
            "QComboBox:hover {"
            "    border-color: #94a3b8;"
            "}"
            "QComboBox::drop-down {"
            "    border: none;"
            "}"
        );
    };

    styleCombo(fromCombo);
    styleCombo(toCombo);
    fromCombo->setCurrentText("BYN");

    convertBtn = new QPushButton("Convert");

    convertBtn->setStyleSheet(
        "QPushButton {"
        "    background-color: #3b82f6;"
        "    color: #ffffff;"
        "    border: none;"
        "    border-radius: 10px;"
        "    padding: 12px 24px;"
        "    font-size: 16px;"
        "    font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "    background-color: #2563eb;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #1d4ed8;"
        "}"
    );

    convertBtn->setFixedWidth(240);

    refreshBtn = new QPushButton("Refresh");

    refreshBtn->setStyleSheet(
        "QPushButton {"
        "    background-color: #f1f5f9;"
        "    color: #475569;"
        "    border: 2px solid #cbd5e1;"
        "    border-radius: 10px;"
        "    padding: 12px 24px;"
        "    font-size: 16px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #e2e8f0;"
        "}"
    );

    refreshBtn->setFixedWidth(120);

    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->addWidget(convertBtn);
    btnLayout->addStretch();
    btnLayout->addWidget(refreshBtn);

    resultLabel = new QLabel("Let`s go!");

    resultLabel->setStyleSheet(
        "color: #0f172a;"
        "font-size: 20px;"
        "font-weight: 600;"
    );
    resultLabel->setAlignment(Qt::AlignCenter);

    swapBtn = new QPushButton();
    swapBtn->setIcon(QIcon(":/new/prefix1/arrow-right-left.svg"));
    swapBtn->setIconSize(QSize(32, 32));
    swapBtn->setStyleSheet("border: none; background: transparent;");
    swapBtn->setCursor(Qt::PointingHandCursor);

    connect(convertBtn, &QPushButton::clicked, this, &CurrencyConvertor::convert);

   connect(refreshBtn, &QPushButton::clicked, worker, &RatesWorker::doWork);



    QObject::connect(swapBtn, &QPushButton::clicked, [&](){
        QString fromText = fromCombo->currentText();
        QString toText = toCombo->currentText();
        fromCombo->setCurrentText(toText);
        toCombo->setCurrentText(fromText);
    });

    QHBoxLayout *amountLayout = new QHBoxLayout();
    amountLayout->addWidget(amount);
    amountLayout->addStretch();
    amountLayout->addWidget(fromCombo);
    amountLayout->addWidget(swapBtn);
    amountLayout->addWidget(toCombo);

    QVBoxLayout *cardLayout = new QVBoxLayout(card);

    card->setLayout(cardLayout);
    cardLayout->addWidget(title);
    cardLayout->addStretch();
    cardLayout->addLayout(amountLayout);
    cardLayout->addStretch();
    cardLayout->addLayout(btnLayout);
    cardLayout->addStretch();
    cardLayout->addWidget(resultLabel);

    cardLayout->setContentsMargins(30, 30, 30, 30);
    cardLayout->setSpacing(15);

    windowLayout->addStretch();
    windowLayout->addWidget(card);
    windowLayout->addStretch();
}

void CurrencyConvertor::fetchRates() {


}

void CurrencyConvertor::convert(){

        double value = amount->text().toDouble();

        if (amount->text().isEmpty() || amount->text().toDouble() <= 0) {
            resultLabel->setText("Введите сумму");
            return;
        }

        QString valueToCombo = toCombo->currentText();
        QString valueFromCombo = fromCombo->currentText();

        if(!rates.isEmpty()){
            double result = (value / rates[valueToCombo]) * rates[valueFromCombo];
            resultLabel->setText(QString::number(result, 'f', 4) + " " + toCombo->currentText());
        }

}

void CurrencyConvertor::onRateReady(QMap<QString, double>rates){
    this->rates = rates;
    resultLabel->setText("Курсы загружены");
}

void CurrencyConvertor::onResultRequest(QString result){
    resultLabel->setText(result);
}

CurrencyConvertor::~CurrencyConvertor() {
    thread->quit();
    thread->wait();
    delete worker;
    delete thread;
}

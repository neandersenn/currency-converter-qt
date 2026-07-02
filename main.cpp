#include "mainwindow.h"
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

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QWidget window;
    window.setStyleSheet("background-color: #f0f4f8;");

    QHBoxLayout *windowLayout = new QHBoxLayout(&window);

    QFrame *card = new QFrame(&window);
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


    QLabel *title = new QLabel("Currency convertor", card);

    title->setStyleSheet(
        "color: #1a1a1a;"
        "font-size: 24px;"
        "font-weight: bold;"
    );

    title->setAlignment(Qt::AlignCenter);

    QLineEdit *amount = new QLineEdit(card);
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


    QComboBox *fromCombo = new QComboBox(card);
    QComboBox *toCombo = new QComboBox(card);

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

    QPushButton *convertBtn = new QPushButton("Convert");

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

    QPushButton *refreshBtn = new QPushButton("Refresh");

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

    QLabel *resultLabel = new QLabel("Let`s go!");

    resultLabel->setStyleSheet(
        "color: #0f172a;"
        "font-size: 20px;"
        "font-weight: 600;"
    );
    resultLabel->setAlignment(Qt::AlignCenter);

   QNetworkAccessManager *manager = new  QNetworkAccessManager(&window);

    QMap<QString, double> rates;

    auto fetchRates = [manager, &rates, &resultLabel]() {
        QNetworkRequest  request(QUrl("https://api.nbrb.by/exrates/rates?periodicity=0&limit=500"));
        QNetworkReply *reply = manager->get(request);

        QObject::connect(reply,&QNetworkReply::finished,  [reply, &rates, &resultLabel](){
            if(reply->error() == QNetworkReply::NoError){
                QByteArray arr = reply->readAll();
                QJsonDocument doc = QJsonDocument::fromJson(arr);

                if(doc.isArray()){
                    rates.clear();
                    rates["BYN"] = 1.0;

                    for(QJsonValue v : doc.array()){
                        QJsonObject obj = v.toObject();
                        QString cur = obj["Cur_Abbreviation"].toString();
                        double rate = obj["Cur_OfficialRate"].toDouble();
                        int scale = obj["Cur_Scale"].toInt();
                        if(rate > 0 && scale > 0){
                            rates[cur] = rate / scale;
                        };
                    }
                }
            } else {
                resultLabel->setText("Ошибка загрузки");
            }
          reply->deleteLater();
        });
  };





    QObject::connect(convertBtn, &QPushButton::clicked,[&](){
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

    });

    QObject::connect(refreshBtn, &QPushButton::clicked,[&](){
        fetchRates();
        resultLabel->setText("Rates refresh");
    });

    fetchRates();

    QPushButton *swapBtn = new QPushButton();
    swapBtn->setIcon(QIcon(":/new/prefix1/arrow-right-left.svg"));
    swapBtn->setIconSize(QSize(32, 32));
    swapBtn->setStyleSheet("border: none; background: transparent;");
    swapBtn->setCursor(Qt::PointingHandCursor);

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

    window.setFixedSize(700, 500);
    window.show();
    return app.exec();
}

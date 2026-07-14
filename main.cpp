#include <QApplication>
#include "currencyconvertor.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    CurrencyConvertor window;

    qRegisterMetaType<QMap<QString, double>>("QMap<QString,double>");

    window.setFixedSize(700, 500);
    window.show();
    return app.exec();
}

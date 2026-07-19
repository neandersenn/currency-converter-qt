### Currency Converter

Simple currency converter using rates from NBRB.

![Скриншот приложения](screenshots/app.png)

## Features

- Conversion of 7 currencies: USD, EUR, RUB, BYN, GBP, CNY, KZT
- Update rates with **Refresh** button
- Rates are updated every 5 minutes

## Requirements
- Qt 5.15 or higher
- CMake 3.5+

## Build and run

```bash
cd currency-converter-qt
mkdir build && cd build
cmake ..
make
./currency-converter-qt

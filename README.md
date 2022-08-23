# cpp-spreadsheet

О программе Spreadsheet
Это эмуляция электронной таблицы

Сборка
Для сборки этого приложения вам понадобится: cmake версии 3.10 или выше (https://cmake.org/).
ANTLR версии 4.7.2 (https://www.antlr.org).
И компилятор с поддержкой С++17 и выше.

Для сборки (из директории ./spreadsheet) выполните следующие действия:
(Убедитесь, что у Вас есть права на создание файлов в рабочем каталоге)

1. создайте поддиректорию ./build
( mkdir ./build )
2. перейдя в ./build выполните
cmake ../
(По умолчанию будет создан makefile для релизной сборки пограммы,
если Вам необходима отладочная версия используйте флаг -DCMAKE_BUILD_TYPE=Debug )
3. cmake --build .

В результате последней команды будет создан исполняемый файл ./spreadsheet

Программа была успешно собрана на Debian/Linux version 5.10.0-14-amd64 с gcc версии 10.2.1

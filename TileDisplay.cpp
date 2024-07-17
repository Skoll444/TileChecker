#include <iostream>
#include <stdio.h>
#include <math.h>
#include <fstream>
#include <string>
#include <unistd.h>
#include "boost/filesystem.hpp"

#include <QtWidgets>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QObject>
#include <QSlider>

#include <QApplication>
#include <QLabel>
#include <QPushButton>
#include <QGridLayout>
#include <QDir>

#include <QLineEdit>
#include <QWidget>
#include <QShortcut>
#include <boost/filesystem/operations.hpp>

#include "TileDisplay.h"

#define Couts // Закомментировать для печати

using namespace std;
namespace fs = boost::filesystem;

int TilesDisplay::x[SIZE], TilesDisplay::y[SIZE];

int TilesDisplay::AllCoordX[SIZE * IMAGE], TilesDisplay::AllCoordY[SIZE * IMAGE];

string TilesDisplay::URLS[SIZE * IMAGE];

string TilesDisplay::ServerName = "http://10.1.1.218/tile/";

string TilesDisplay::Slash = "/";

int TilesDisplay::Zoom = 0;

int TilesDisplay::long2tilex ( double lon, int z )
{
    return (int(floor((lon + 180.0) / 360.0 * pow(2.0, z))));
}

int TilesDisplay::lat2tiley ( double lat, int z )
{
    return (int(floor((1.0 - log(tan(lat * M_PI / 180.0) + 1.0 / cos(lat * M_PI / 180.00)) / M_PI) / 2.0 * pow(2.0, z))));
}

int TilesDisplay::massivXForCurrentZoom ( int CurrentX, int zoom )
{
    for ( int i = 0; i < 5; i++)
    {
        AllCoordX[zoom * 20 + i] = AllCoordX[zoom * 20 + (i + 5)] = AllCoordX[zoom * 20 + (i + 10)] = AllCoordX[zoom * 20 + (i + 15)] = CurrentX - (1 - i);
#ifndef Couts
        for (int i=0; i<20; i++ ) {
            cout << "Номер ячейки: " << zoom*19+i << " Значение ячейки: " << AllCoordX[zoom*19+i] << endl;
        }

#endif
    }
    return 0;
}

int TilesDisplay::massivYForCurrentZoom ( int CurrentY, int zoom )
{
    for ( int i = 0; i < 4; i++)
    {
        AllCoordY[zoom * 20 + i * 4 + i] = AllCoordY[zoom * 20 + i * 4 + (i + 1)] = AllCoordY[zoom * 20 + i * 4 + (i + 2)] = AllCoordY[zoom * 20 + i * 4 + (i + 3)] = AllCoordY[zoom * 20 + i * 4 + (i + 4)] = CurrentY - (2 - i);
    }
#ifndef Couts
        for (int i=0; i<20; i++ ) {
            cout << "Номер ячейки: " << zoom*19+i << " Значение ячейки: " << AllCoordX[zoom*19+i] << endl;
        }

#endif
    return 0;
}

string TilesDisplay::URLs ( int zoom )
{

    for ( int i = 0; i < 20; i++)
    {
        string CurrentX, CurrentY, CurrentZoom, IP;

        CurrentX = std::to_string(AllCoordX[zoom * 20 + i]);
        CurrentY = std::to_string(AllCoordY[zoom * 20 + i]);
        CurrentZoom = std::to_string(zoom);
        IP = ServerName + CurrentZoom + Slash + CurrentX + Slash + CurrentY;

        URLS[zoom * 20 + i] = IP;

#ifndef Couts
       cout << "Текущая ячейка СТРИНГ: " << zoom*20+i << " Содержимое ячейки: " << URLS[zoom*20+i] << endl;
#endif

    }
    return " 1 ";
}

void TilesDisplay::SaveImages ( int zoom ) // TODO: Убрать непонятные циклы с 0 до 3. Для этого нужно чтобы значения не могли быть равны <0
{

    //Временный костыль для зумов 0,1,2;
    for ( int i = 0; i < 20; i++)
    {

        if ( zoom == 0 )
        {
            if ( i != 11 ) continue;
        }

        if ( zoom == 1 )
        {
            if ( i != 11 && i != 12 && i != 16 && i != 17 ) continue;
        }
        if ( zoom == 2 )
        {
            if ( i < 6 || i == 10 || i == 15 ) continue;
        }

        string CurrentURL = URLS[zoom * 20 + i];
        string format = "png";
        string CurrentZoom = std::to_string(zoom);
        string CurrentI = std::to_string(i + 1);
        std::string command = "wget -O images/" + CurrentZoom + "/image" + CurrentI + ".png " + CurrentURL + ".png";
        system(command.c_str());
    }

}
void TilesDisplay::DeleteAndMakeDir ()
{

    namespace fs = boost::filesystem;
    std::string directory = "images";

    // Удаляем существующие папки с тайлами в виде png
    for ( const auto &entry : fs::directory_iterator(directory))
    {
        if ( fs::is_directory(entry.path()) )
        {
            fs::remove_all(entry.path());
        }
    }

    // Создаём новые папки от 0 до 18
    for ( int i = 0; i < 19; i++)
    {
        fs::create_directory(directory + "/" + std::to_string(i));
    }
}
int main ( int argc, char *argv[] )
{

    QApplication app(argc, argv);

    // Создание главного окна приложения
    QWidget mainWindow;
    mainWindow.setWindowTitle("Image Viewer"); //Имя приложения
    mainWindow.resize(800, 600);  // Размеры экрана 1920x1080

    // Создание виджетов QLabel для отображения картинок
    QLabel imageLabels[20];
    for ( int i = 0; i < 20; ++i)
    {
        imageLabels[i].setAlignment(Qt::AlignCenter);
        imageLabels[i].setScaledContents(true);
        imageLabels[i].setFixedSize(256, 256);
    }

    // Создание метки для отображения текста
    QLabel actionLabel(&mainWindow);

    // Создание кнопки для выхода из приложения
    QShortcut *shortcut = new QShortcut(QKeySequence("q"), &mainWindow);

    // Создание кнопки zoom
    QPushButton zoomButtonPlus("+");
    QPushButton zoomButtonMinus("-");

    // Создание сетки для размещения виджетов
    QGridLayout layout;

    // Создание виджетов для ввода чисел
    QLineEdit CurrentLonInput(&mainWindow);
    QLineEdit CurrentLatInput(&mainWindow);

    // Создание кнопок "Загрузить карту" и "Очистить карту"
    QPushButton Loadbutton("Загрузить карту", &mainWindow);
    QPushButton CleanButton("Очиститить карту", &mainWindow);

    // Виджет для текстового описания кнопок долготы и широты, zoom
    QLabel CurrentLonLabelInput("lon", &mainWindow);
    QLabel CurrentLatLabelInput("lat", &mainWindow);
    QLabel TextOfZoom("zoom", &mainWindow);

    // Создание виджетов для отображения картинок
    for ( int i = 0; i < 4; i++) // ЕСЛИ СДЕЛАТЬ <5, ТО БУДЕТ 20 КАРТИНОК, А СЕЙЧАС 16! ИЗОБРАЖЕНИЕ 1024Х1024
    {
        layout.setSpacing(0); // с помощью этого метода расстояние между виджетами равно 0. Картинки отображаются как целая единица.
        layout.addWidget(&imageLabels[i], 0, i * 4, 5, 4);     // 1 строкаfinalImage.save("combined_image.png");
        layout.addWidget(&imageLabels[i + 5], 5, i * 4, 5, 4);   // 2 строка
        layout.addWidget(&imageLabels[i + 10], 10, i * 4, 5, 4); // 3 строка
        layout.addWidget(&imageLabels[i + 15], 15, i * 4, 5, 4); // 4 строка
    }

    // Создание виджетов для ввода долготы и широты
    layout.addWidget(&CurrentLonInput, 0, 20, 2, 3);
    layout.addWidget(&CurrentLatInput, 0, 20, 3, 3);

    // Создание виджета для кнопок "Загрузить карту" и "Очистить карту"
    layout.addWidget(&Loadbutton, 0, 20, 4, 3);
    layout.addWidget(&CleanButton, 0, 20, 8, 5);

    // Создание виджета для отображения текста в ходе результата
    layout.addWidget(&actionLabel, 0, 20, 5, 10);

    // Создание текстовых виджетов долготы и широты, zoom'а
    layout.addWidget(&CurrentLonLabelInput, 0, 17, 2, 3);
    layout.addWidget(&CurrentLatLabelInput, 0, 17, 3, 3);
    layout.addWidget(&TextOfZoom, 0, 23, 1, 3);

    // Создание виджета для использования кнопки "zoom"
    layout.addWidget(&zoomButtonPlus, 0, 20, 1, 3);
    layout.addWidget(&zoomButtonMinus, 0, 17, 1, 3);

    mainWindow.setLayout(&layout);    // Установка сетки в главное окно

    // Установка источников изображений в соответствии с выбранным значением zoom
    auto setImageSources = [ & ] ()
    {
        for (int i = 0; i < 20; ++i)
        {
            QString imageSource = QString("images/%1/image%2.png").arg(TilesDisplay::Zoom).arg(i+1);
            imageLabels[i].setPixmap(QPixmap(imageSource));
        }
        /*
        QPixmap pixmap(mainWindow.size());
            mainWindow.render(&pixmap);
            pixmap.save("allmap.png");
*/
       QImage finalImage(1024, 1024, QImage::Format_RGB32);
        finalImage.fill(Qt::white);
        QPainter painter(&finalImage);
        int XPos =0;
        int YPos =0;
        for (int i = 0; i < 20; i++) {

           QImage image(QString("images/%1/image%2.png").arg(TilesDisplay::Zoom).arg(i+1));
           painter.drawImage(XPos, YPos, image);
           XPos +=256;
           if (XPos >1024) {
               XPos = 0;
               YPos +=256;
           }
        }
        painter.end();
        std::string newname = "combined_image" + std::to_string(TilesDisplay::Zoom) + ".png";
        std::string originalname = "combined_image.png";
        finalImage.save("combined_image.png");
        fs::rename(originalname,newname);

    };

    // Обработчик нажатия кнопки zoom+
    QObject::connect(&zoomButtonPlus, &QPushButton::clicked, [ & ] ()
    {
        if (TilesDisplay::Zoom < 18)
        ++TilesDisplay::Zoom;
        else
        {
            TilesDisplay::Zoom = 18;
        }
        setImageSources();
    });

    // Обработчик нажатия кнопки zoom-
    QObject::connect(&zoomButtonMinus, &QPushButton::clicked, [ & ] ()
    {
        if (TilesDisplay::Zoom > 0 )
        {
            --TilesDisplay::Zoom;
        }
        else
        {
            TilesDisplay::Zoom = 0;
        }
        setImageSources();
    });

    // Обработчик нажатия кнопки выполнить
    QObject::connect(&Loadbutton, &QPushButton::clicked, [ & ] ()
    {

        double CurrentLon =CurrentLonInput.text().toDouble();
        double CurrentLat =CurrentLatInput.text().toDouble();

        //CurrentLon = -122.1981; // значение долготы и широты, относительно которых ведутся вычисления // -121.89783
        // CurrentLat = 37.7490;  // УДАЛИТЬ, ПОКА ДЛЯ ПРОВЕРКИ СДЕЛАЛ! // 37.29036

            for (int i=0; i<TilesDisplay::SIZE; i++)
            {
                TilesDisplay::x[i]=TilesDisplay::long2tilex(CurrentLon,i);
                TilesDisplay::y[i]=TilesDisplay::lat2tiley(CurrentLat,i);
                TilesDisplay::massivXForCurrentZoom (TilesDisplay::x[i],i);
                TilesDisplay::massivYForCurrentZoom (TilesDisplay::y[i],i);
                TilesDisplay::URLs(i);
                TilesDisplay::SaveImages(i);

#ifndef Couts
            cout << "Определение центральной точки: Число x: " <<TilesDisplay::x[i] << " Число y: " << TilesDisplay::y[i] << " Для zoom'a: " << i << endl;
#endif
        }
        actionLabel.setText("Карта закружена! Для очистки карты нажмите кнопку ниже");
    });

    // Обработчик нажатия кнопки "Очистить"
    QObject::connect(&CleanButton, &QPushButton::clicked, [ & ] ()
    {

        TilesDisplay::DeleteAndMakeDir();
        actionLabel.setText("Карта очищена! Для загрузки карты нажмите кнопку выше");
        setImageSources();
        TilesDisplay::Zoom = 0;

    });

    // Проверка нажатия кнопки Q - выход из приложения
    QObject::connect(shortcut, &QShortcut::activated, &app, &QApplication::quit);

    // Инициализация источников изображений
    setImageSources();

    // Отображение главного окна приложения
    mainWindow.show();
    return app.exec();
}


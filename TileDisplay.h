#ifndef TILEDISPLAY_H_
#define TILEDISPLAY_H_

#include <iostream>
#include <stdio.h>
#include <math.h>
#include <string>


using namespace std;

/**
 * @class Класс отображения тайлов, получаемых с OSM Server'а
 */
class  TilesDisplay {
public:

/**
 * @b определяет номер центрального тайла(по x) для текущего значения долготы
 * @param lon текущая долгота
 * @param z - текущий уровень приближения(zoom)
 */
static int long2tilex(double lon, int z);

/**
 * @b определяет номер центрального тайла(по y) для текущего значения долготы
 * @param lat текущая широта
 * @param z текущий уровень приближения(zoom)
 */
static int lat2tiley(double lat,int z);

/**
 * @b Заполняет массив AllCoordX значениями X в зависимости от текущего zoom
 * @param CurrentX центральная клетка по оси X,в зависимости от которой и заполняется массив
 * @param zoom текущее значение приближения
 */
static int massivXForCurrentZoom (int CurrentX, int zoom);

/**
 * @b Заполняет массив AllCoordY значениями Y в зависимости от текущего zoom
 * @param CurrentY центральная клетка по оси Y,в зависимости от которой и заполняется массив
 * @param zoom текущее значение приближения
 */
static int massivYForCurrentZoom (int CurrentY, int zoom);

/**
 * @b Заполняет массив URLS адрессами нахождения тайлов
 * @param zoom текущий уровень приближения
 * @param
 */
static string URLs (int zoom);

/** @b Скачивает все необходимые тайлы в формате png с сервера
 *  @param zoom текущий уровень приближения
 */
static void SaveImages(int zoom);

/**
 * @b Удаляет все папки с тайлами из директории, и создаёт новые, пустые папки
 */
static void DeleteAndMakeDir();

static const int SIZE=19; ///<<< размер массива

static const int IMAGE=20; ///<<<  Количество картинок, для одного zoom'а

static int x[SIZE], y[SIZE]; ///<<<  объявление массивов, которые хранят номера центральных ячеек для x и y соответсвенно.

static int AllCoordX[SIZE*IMAGE], AllCoordY[SIZE*IMAGE]; ///<<<  обьявление массивов, хранящие номера ячеек всех отображаемых тайлов

static string URLS[SIZE*IMAGE]; ///<<<  Массив, который содержат адреса ячеек

static string ServerName; ///<<< Имя сервера, с которого мы требуем картинки

static string Slash; ///<<< Слэш

static int Zoom; ///<<< Индекс текущего источника изображений

};
#endif /* TILEDISPLAY_H_ */

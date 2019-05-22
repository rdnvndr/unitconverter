#include <iostream>
#include <stdio.h>

#define _USE_MATH_DEFINES
#define EPS 1e-14

#include <math.h>
#include <cfloat>

using namespace std;

//! Переводит в базовую единицу измерения
double toBaseUnit(double unit) {
//    return unit * 180 / M_PI;
    return unit * 0.061048;
//    return unit / 3;
//    return unit / 100;
}

//! Переводит из базовой единицы измерения
double fromBaseUnit(double unit) {
//    return unit * M_PI / 180;
    return unit / 0.061048;
//     return unit * 3;
//    return unit * 100;
}


//! Математическое округление
double roundNumber(double number)
{
    return number < 0.0 ? ceil(number - 0.5) : floor(number + 0.5);
}

//! Математическое округление до указанного знака
double roundNumber(double number, int places)
{
    double off = pow(10.0, places);
    return roundNumber(number*off)/off;
}

//! Округление до целого в меньшую сторону
double floorNumber(double number, int places)
{
    double off = pow(10.0, places);
    return floor(number*off)/off;
}

//! Количество цифр после запятой
int digitsAfterDecimalPoint(double number) {
    int count = 0;

    double n = floor(number);
    while (abs(n - number) > EPS) {
        count++;
        n = floorNumber(number, count);
    }
    return count;
}

//! Количество цифр до запятой
int digitsBeforeDecimalPoint(double number) {
    int count = 0;

    double n = floor(number);
    while (n >= 1) {
        count++;
        n = number * pow(10.0, -count);
    }
    return count;
}

//! Количество нулей после запятой
int zerosAfterDecimalPoint(double number) {
    int count = 0;

    double n = number;
    while (n < 1) {
        count++;
        n = number * pow(10.0, count);
    }

    return count;
}

//! Конвертирование
double convert(double number, bool isToBase, double (convertUnit)(double)) {
    // Определяется точность числа
    int countOfDigits = digitsAfterDecimalPoint(number);

    // Проверка на переполнение мантисы
    if (digitsBeforeDecimalPoint(number) + countOfDigits + 1 >= DBL_DIG)
         return convertUnit(number);

    // Уменьшаем точность приконвертировании из базовой
    if (!isToBase) {
        number = floorNumber(number, countOfDigits);
        --countOfDigits;
    }

    // Задаётся погрешность числа, при конвертации. Увеличивает точность при
    // конвертации в базовую
    double ex1 = (isToBase ? -0.05 : -0.5) * pow(10.0, -countOfDigits);
    double ex2 = (isToBase ?  0.04 :  0.4) * pow(10.0, -countOfDigits);

    // Конвертируется число
    double y  = convertUnit(number);
    double y1 = convertUnit(number + ex1);
    double y2 = convertUnit(number + ex2);

    // Вычисляется погрешность сконвертированного числа
    double ey  = abs(y2 - y1);
    double ey1 = abs(y  - y1);
    double ey2 = abs(y  - y2);

    // Если точность сконвертированного числа больше точности типа, то число
    // по точности не округляется
    if (ey1 > EPS && ey2 > EPS) {
        // Вычисляется точность сконвертированного числа
        int countOfZeros = zerosAfterDecimalPoint(ey1 > ey2 ? ey1 : ey2);

        // Уточняется точность сконвертированного числа
        if (ey1 * pow(10.0, countOfZeros) > 5 || ey2 * pow(10.0, countOfZeros) > 4)
            countOfZeros--;

        // Уменьшает точность для восстановления оригинального значения
        if (!isToBase)
            countOfZeros--;

        // Конвертируется число с заданной точностью
        double result = roundNumber(y, countOfZeros);
        if (isToBase) {
            // При конвертации в базовую для сохранения точности дробного числа
            // (например 0.70) в конец  добавляет число-маркер.
            // Число 1.55 с округлением используется для зануления разрядов мантисы
            result = roundNumber(result +  1.55 * pow(10.0, -(countOfZeros + 1)),
                                 (countOfZeros + 1));
        }
//        cout << "countOfZeros:" << countOfZeros << "\n";
        return result;
    } else {
        return y;
    }
}

int main(int argc, char *argv[])
{
    double srcUnit;
    cout.precision(DBL_DIG);

    printf("From unit:  ");
    cin >> srcUnit;
    cout << "Input unit: " << srcUnit << "\n";

    double dstUnit = convert(srcUnit, true,  toBaseUnit);
    double rslUnit = convert(dstUnit, false, fromBaseUnit);

    cout << "To unit:    " << dstUnit << "\n";
    cout << "Result:     " << rslUnit << "\n";

    return 0;
}

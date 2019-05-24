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

//! Сравнение вещественных чисел
bool compare(double p1, double p2) {
    if (abs(p1) <= 0.000000000001 || abs(p2) <= 0.000000000001) {
        ++p1;
        ++p2;
    }
    return (abs(p1 - p2) * 1000000000000.0 <= fmin(abs(p1), abs(p2)));
}

//! Количество цифр после запятой
int digitsAfterDecimalPoint(double number) {
    int count = 0;

    double n = floor(number);
    while (abs(number - n) > EPS) {
        ++count;
        n = floorNumber(number, count);
    }
    return count;
}

//! Количество цифр до запятой
int digitsBeforeDecimalPoint(double number) {
    int count = 0;

    double n = floor(number);
    while (n >= 1) {
        ++count;
        n = number * pow(10.0, -count);
    }
    return count;
}

//! Количество нулей после запятой
int zerosAfterDecimalPoint(double number) {
    int count = 0;

    double n = number;
    while (n < 1) {
        ++count;
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

    // Уменьшаем точность при конвертировании из базовой
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

    // Вычисляется абсолютную погрешность сконвертированного числа
    double ey1 = abs(y  - y1);
    double ey2 = abs(y  - y2);

    // Если точность сконвертированного числа больше точности типа, то число
    // по точности не округляется
    if (ey1 > EPS && ey2 > EPS) {
        // Вычисляется точность сконвертированного числа
        int countOfZeros = zerosAfterDecimalPoint(ey1 > ey2 ? ey1 : ey2);

        // Уточняется точность сконвертированного числа
        if (ey1 * pow(10.0, countOfZeros) > 5 || ey2 * pow(10.0, countOfZeros) > 4)
            --countOfZeros;

        // Уменьшает точность для восстановления оригинального значения
        if (!isToBase)
            --countOfZeros;

        // Конвертируется число с заданной точностью
        double result = roundNumber(y, countOfZeros);
        if (isToBase) {
            // При конвертации в базовую для сохранения точности дробного числа
            // (например 0.70) в конец  добавляет число-маркер.
            result = floorNumber(result +  0.11111111111111 * pow(10.0, -countOfZeros),
                                 countOfZeros + 1);
        }
        return result;
    } else {
        return y;
    }
}

//! Тест проверки конвертирования
void convertingTest(double n, double (toBase)(double), double (fromBase)(double))
{
    double dstUnit = convert(n, true,  toBase);
    double srcUnit = convert(dstUnit, false, fromBase);

    if (compare(n, srcUnit)) {
        cout << "[OK]   Convert: " << n << " -> " << dstUnit << "\n"
             << "                " << srcUnit << "\n";
    } else {
        cout << "[FAIL] Convert: " << n << " -> " << dstUnit << "\n"
             << "                " << srcUnit << "\n";
    }
    return;
}

//! Прогон тестов конвертирования
void tests() {
    double (*toBase)(double);
    double (*fromBase)(double);

    cout << "Convert Degree -> Radian \n"
         << "---------------------------------------------------------------\n";
    toBase   = [] (double n) ->double { return n * 180 / M_PI; };
    fromBase = [] (double n) ->double { return n * M_PI / 180; };

    convertingTest(3.14, toBase, fromBase);
    convertingTest(0.12345678, toBase, fromBase);
    convertingTest(0.1234568, toBase, fromBase);
    convertingTest(0.1235, toBase, fromBase);


    cout << "\nConvert Kilograme -> Pood \n"
         << "---------------------------------------------------------------\n";
    toBase   = [] (double n) ->double { return n * 0.061048; };
    fromBase = [] (double n) ->double { return n / 0.061048; };

    convertingTest(16.38055300746, toBase, fromBase);
    convertingTest(16.3805530074695, toBase, fromBase);
    convertingTest(1.234567890123456, toBase, fromBase);
    convertingTest(12.34567890123456, toBase, fromBase);


    cout << "\nConvert 1 -> 1/3 \n"
         << "---------------------------------------------------------------\n";
    toBase   = [] (double n) ->double { return n / 3; };
    fromBase = [] (double n) ->double { return n * 3; };

    convertingTest(1, toBase, fromBase);
    convertingTest(2.1, toBase, fromBase);
    convertingTest(2.9, toBase, fromBase);
    convertingTest(7.5, toBase, fromBase);
    convertingTest(1.235, toBase, fromBase);


    cout << "\nConvert 1 -> 1/100 \n"
         << "---------------------------------------------------------------\n";
    toBase   = [] (double n) ->double { return n / 100; };
    fromBase = [] (double n) ->double { return n * 100; };

    convertingTest(1, toBase, fromBase);
    convertingTest(1.1, toBase, fromBase);
    convertingTest(1.01, toBase, fromBase);
    convertingTest(1.2346, toBase, fromBase);

    cout << "\n";
}

int main(int argc, char *argv[])
{
    double srcUnit;
    cout.precision(DBL_DIG);

    tests(); return 0;

    printf("From unit:  ");
    cin >> srcUnit;
    cout << "Input unit: " << srcUnit << "\n";

    double dstUnit = convert(srcUnit, true,  toBaseUnit);
    double rslUnit = convert(dstUnit, false, fromBaseUnit);

    cout << "To unit:    " << dstUnit << "\n";
    cout << "Result:     " << rslUnit << "\n";

    return 0;
}

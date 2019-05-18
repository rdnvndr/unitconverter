#include <iostream>
#include <stdio.h>

#define _USE_MATH_DEFINES
#include <math.h>
#include <cfloat>

using namespace std;

//! Переводит из радиан в градус
double toDeg(double rad) {
    return rad * 180 / M_PI;
}

//! Переводит из градус в радиан
double toRad(double deg) {
    return deg * M_PI / 180;
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


//! Количество цифр после запятой
int digitsAfterDecimalPoint(double number) {
    int count = 0;

    double n = number;
    while ((n - floor(n)) != 0.0) {
        count++;
        n = number * pow(10.0, count);
    }
    return count+1;
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
double convert(double number, double (convertUnit)(double) ) {
    // Определяется точность числа
    int countOfDigits = digitsAfterDecimalPoint(number);

    // Задаётся погрешность числа
    double ex1 = -5 * pow(10.0, -countOfDigits);
    double ex2 =  4 * pow(10.0, -countOfDigits);

    // Вычисляется погрешность сконвертированного числа
    double ey = convertUnit(abs((number+ex2)-(number+ex1)));

    // Вычисляется точность сконвертированного числа
    int countOfZeros = zerosAfterDecimalPoint(ey);

    // Уточняется точность сконвертированного числа
    if (ey * pow(10.0, countOfZeros) <= 5)
        countOfZeros--;

    // Конвертируется число с заданной точностью
    double y = convertUnit(number);
    return roundNumber(y, countOfZeros);
}

int main(int argc, char *argv[])
{
    double rad1;
    cout.precision(DBL_DIG);

    printf("Enter radians: ");
    cin >> rad1;

    double deg = convert(rad1, toDeg);
    double rad = convert(deg,  toRad);

    cout << "Degrees: " << deg << "\n";
    cout << "Result:  " << rad << "\n";

    return 0;
}

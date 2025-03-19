#include <Windows.h>
#include <iostream>
#include <vector>
#include <fstream>  // Добавили библиотеку для работы с файлами
#include <cmath>
#define _USE_MATH_DEFINES
#include <math.h>

using namespace std;

int main()
{

    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    // Максимальное количество гармоник
    const int maxHarmonics = 4;

    // Ввод количества гармоник
    cout << "Введите количество гармоник (не более 4): ";
    int numHarmonics;
    cin >> numHarmonics;

    // Проверяем корректность ввода количества гармоник
    if (numHarmonics <= 0 || numHarmonics > maxHarmonics) {
        cerr << "Ошибка! Количество гармоник должно быть от 1 до 4." << endl;
        return 1;
    }

    vector<double> amplitudes(numHarmonics);      // Амплитуды гармоник
    vector<double> frequencies(numHarmonics);     // Частоты гармоник
    vector<double> initialPhases(numHarmonics);   // Начальные фазы гармоник

    // Ввод амплитуд, частот и начальных фаз гармоник
    for (int i = 0; i < numHarmonics; ++i) {
        double amplitude;
        cout << "Введите амплитуду для гармоники " << i + 1 << ": ";
        cin >> amplitude;
        amplitudes[i] = amplitude;

        double frequency;
        cout << "Введите частоту для гармоники " << i + 1 << ": ";
        cin >> frequency;
        frequencies[i] = frequency;

        double phase;
        cout << "Введите начальную фазу для гармоники " << i + 1 << ": ";
        cin >> phase;
        initialPhases[i] = phase * M_PI / 180;  // Преобразуем градусы в радианы
    }

    // Ввод частоты дискретизации
    cout << "Введите частоту дискретизации: ";
    double samplingFrequency;
    cin >> samplingFrequency;

    // Ввод шага времени дельта-t
    cout << "Введите шаг времени дельта-t: ";
    double deltaT;
    cin >> deltaT;

    // Ввод количества точек для генерации сигнала
    cout << "Введите количество точек для генерации сигнала: ";
    int numPoints;
    cin >> numPoints;

    // Генерация массива значений сигнала
    vector<vector<double>> signalValues(numHarmonics, vector<double>(numPoints));

    for (int harmonicIndex = 0; harmonicIndex < numHarmonics; ++harmonicIndex) {
        for (int timeIndex = 0; timeIndex < numPoints; ++timeIndex) {
            double time = timeIndex * deltaT;
            signalValues[harmonicIndex][timeIndex] =
                amplitudes[harmonicIndex] *
                sin(2 * M_PI * frequencies[harmonicIndex] * time + initialPhases[harmonicIndex]);
        }
    }

    // Суммируем значения по гармоникам в одном массиве
    vector<double> resultantSignal(numPoints);
    for (int timeIndex = 0; timeIndex < numPoints; ++timeIndex) {
        double sum = 0.0;
        for (int harmonicIndex = 0; harmonicIndex < numHarmonics; ++harmonicIndex) {
            sum += signalValues[harmonicIndex][timeIndex];
        }
        resultantSignal[timeIndex] = sum;
    }

    // Запись данных в текстовый файл
    ofstream outputFile("result.txt");
    if (outputFile.is_open()) {
        // Запишем заголовок файла
        outputFile << "Время\t";
        for (int i = 1; i <= numHarmonics; ++i) {
            outputFile << "Гармоника_" << i << "\t";
        }
        outputFile << "Результирующая_гармоника\n";

        // Запишем данные
        for (int timeIndex = 0; timeIndex < numPoints; ++timeIndex) {
            double time = timeIndex * deltaT;
            outputFile << time << "\t";  // Время

            for (int harmonicIndex = 0; harmonicIndex < numHarmonics; ++harmonicIndex) {
                outputFile << signalValues[harmonicIndex][timeIndex] << "\t";
            }

            outputFile << resultantSignal[timeIndex] << "\n";  // Результирующий сигнал
        }

        outputFile.close();
        cout << "Данные успешно записаны в файл result.txt!" << endl;
    }
    else {
        cerr << "Не удалось открыть файл для записи!" << endl;
    }

    return 0;
}
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#define _USE_MATH_DEFINES
#include <math.h>
#include <complex>
#include <algorithm>
#include <windows.h>
#include <fftw3.h>
#pragma comment(lib, "libfftw3-3.lib")

using namespace std;

const int maxHarmonics = 4;

struct HarmonicParams {
    double amplitude;
    double frequency;
    double initialPhaseDegrees;
};

// Функция для загрузки параметров из файла
bool loadFromFile(const string& filename, vector<HarmonicParams>& harmonics, int& numHarmonics, double& samplingFreq, double& deltaT, int& numPoints) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error opening file '" << filename << "'" << endl;
        return false;
    }

    file >> numHarmonics;
    if (numHarmonics <= 0 || numHarmonics > maxHarmonics) {
        cerr << "Invalid number of harmonics in file." << endl;
        return false;
    }

    harmonics.resize(numHarmonics);
    for (int i = 0; i < numHarmonics; ++i) {
        file >> harmonics[i].amplitude
            >> harmonics[i].frequency
            >> harmonics[i].initialPhaseDegrees;
    }

    file >> samplingFreq >> deltaT >> numPoints;
    return true;
}

// Функция для ручного ввода параметров
void manualInput(vector<HarmonicParams>& harmonics, int& numHarmonics, double& samplingFreq, double& deltaT, int& numPoints) {
    cout << "Введите количество гармоник (не более 4): ";
    cin >> numHarmonics;
    if (numHarmonics <= 0 || numHarmonics > maxHarmonics) {
        cerr << "Ошибка! Количество гармоник должно быть от 1 до 4." << endl;
        exit(EXIT_FAILURE);
    }

    harmonics.resize(numHarmonics);
    for (int i = 0; i < numHarmonics; ++i) {
        cout << "Введите амплитуду для гармоники " << i + 1 << ": ";
        cin >> harmonics[i].amplitude;
        cout << "Введите частоту для гармоники " << i + 1 << ": ";
        cin >> harmonics[i].frequency;
        cout << "Введите начальную фазу для гармоники " << i + 1 << "(градусы): ";
        cin >> harmonics[i].initialPhaseDegrees;
    }

    cout << "Введите частоту дискретизации: ";
    cin >> samplingFreq;
    cout << "Введите шаг времени дельта-t: ";
    cin >> deltaT;
    cout << "Введите количество точек для генерации сигнала: ";
    cin >> numPoints;
}

int main() {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    vector<HarmonicParams> harmonics(maxHarmonics);
    int numHarmonics, inputMethod;
    double samplingFreq, deltaT;
    int numPoints;

    cout << "Выберите источник параметров:\n";
    cout << "0 - Ручной ввод\n";
    cout << "1 - Загрузка из файла 'params.txt'\n";
    cin >> inputMethod;

    if (inputMethod == 0) {
        manualInput(harmonics, numHarmonics, samplingFreq, deltaT, numPoints);
    }
    else if (inputMethod == 1) {
        bool success = loadFromFile("params.txt", harmonics, numHarmonics, samplingFreq, deltaT, numPoints);
        if (!success) {
            cerr << "Ошибка загрузки из файла. Используйте ручной ввод.\n";
            manualInput(harmonics, numHarmonics, samplingFreq, deltaT, numPoints);
        }
    }
    else {
        cerr << "Некорректный выбор метода ввода." << endl;
        return EXIT_FAILURE;
    }

    vector<vector<double>> signalValues(numHarmonics, vector<double>(numPoints));
    for (int harmonicIndex = 0; harmonicIndex < numHarmonics; ++harmonicIndex) {
        for (int timeIndex = 0; timeIndex < numPoints; ++timeIndex) {
            double time = timeIndex * deltaT;
            double phaseRad = harmonics[harmonicIndex].initialPhaseDegrees * M_PI / 180.0;
            signalValues[harmonicIndex][timeIndex] = harmonics[harmonicIndex].amplitude *
                sin(2 * M_PI * harmonics[harmonicIndex].frequency * time + phaseRad);
        }
    }

    vector<double> resultantSignal(numPoints);
    for (int timeIndex = 0; timeIndex < numPoints; ++timeIndex) {
        double sum = 0.0;
        for (int harmonicIndex = 0; harmonicIndex < numHarmonics; ++harmonicIndex) {
            sum += signalValues[harmonicIndex][timeIndex];
        }
        resultantSignal[timeIndex] = sum;
    }

    // Запись данных в файл
    ofstream outputFile("result.txt");
    if (outputFile.is_open()) {
        outputFile << "Время\t";
        for (int i = 1; i <= numHarmonics; ++i) {
            outputFile << "Гармоника_" << i << "\t";
        }
        outputFile << "Результирующая_гармоника\n";

        for (int timeIndex = 0; timeIndex < numPoints; ++timeIndex) {
            double time = timeIndex * deltaT;
            outputFile << time << "\t";
            for (int harmonicIndex = 0; harmonicIndex < numHarmonics; ++harmonicIndex) {
                outputFile << signalValues[harmonicIndex][timeIndex] << "\t";
            }
            outputFile << resultantSignal[timeIndex] << "\n";
        }
        outputFile.close();
        cout << "Данные успешно записаны в файл result.txt!" << endl;
    }
    else {
        cerr << "Не удалось открыть файл для записи!" << endl;
    }

    return 0;
}
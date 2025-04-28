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

typedef complex<double> Complex;

struct Harmonic {
    double amplitude;
    double phase;
    double frequency;
};

struct DFTPeak {
    double frequency;
    double amplitude;
    double phaseDegrees;
};

vector<Complex> computeDFT(const vector<double>& signal) {
    int N = signal.size();
    vector<Complex> result(N);
    for (int k = 0; k < N; ++k) {
        double real = 0.0;
        double imag = 0.0;
        for (int n = 0; n < N; ++n) {
            double angle = 2 * M_PI * k * n / N;
            real += signal[n] * cos(angle);
            imag -= signal[n] * sin(angle);
        }
        result[k] = Complex(real, imag);
    }
    return result;
}

vector<DFTPeak> analyzeDFT(const vector<complex<double>>& dftResult, double samplingFreq, int numPoints) {
    vector<DFTPeak> peaks;
    int N = numPoints;
    for (int k = 0; k < N / 2; ++k) {
        double amplitude = 2.0 * abs(dftResult[k]) / N;
        if (amplitude < 1e-6) continue;
        // Коррекция фазы для перехода от sin() к cos()
        double phase = arg(dftResult[k]) * 180.0 / M_PI + 90.0;
        phase = fmod(phase + 180.0, 360.0) - 180.0;
        double frequency = k * samplingFreq / N;
        peaks.push_back({ frequency, amplitude, phase });
    }
    return peaks;
}

// Функция для загрузки параметров из файла
bool loadFromFile(const string& filename, vector<Harmonic>& harmonics, int& numHarmonics, double& samplingFreq, double& deltaT, int& numPoints) {
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
            >> harmonics[i].phase;
    }

    file >> samplingFreq >> deltaT >> numPoints;
    return true;
}

int main() {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    vector<Harmonic> harmonics(maxHarmonics);
    int numHarmonics, inputMethod;
    double samplingFreq, deltaT;
    int numPoints;

    bool success = loadFromFile("params.txt", harmonics, numHarmonics, samplingFreq, deltaT, numPoints);

    vector<vector<double>> signalValues(numHarmonics, vector<double>(numPoints));
    for (int harmonicIndex = 0; harmonicIndex < numHarmonics; ++harmonicIndex) {
        for (int timeIndex = 0; timeIndex < numPoints; ++timeIndex) {
            double time = timeIndex * deltaT;
            double phaseRad = harmonics[harmonicIndex].phase * M_PI / 180.0;
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

    // Вычисление DFT
    vector<complex<double>> dftResult = computeDFT(resultantSignal);
    vector<DFTPeak> peaks = analyzeDFT(dftResult, samplingFreq, numPoints);

    // Генерация восстановленных гармоник
    vector<vector<double>> restoredHarmonics(peaks.size(), vector<double>(numPoints));
    for (size_t harmonicIdx = 0; harmonicIdx < peaks.size(); ++harmonicIdx) {
        for (int timeIdx = 0; timeIdx < numPoints; ++timeIdx) {
            double t = timeIdx * deltaT;
            double phaseRad = peaks[harmonicIdx].phaseDegrees * M_PI / 180.0;
            restoredHarmonics[harmonicIdx][timeIdx] = peaks[harmonicIdx].amplitude *
                sin(2 * M_PI * peaks[harmonicIdx].frequency * t + phaseRad);
        }
    }

    // Запись в restored.txt
    ofstream restoredFile("restored.txt");
    if (restoredFile.is_open()) {
        restoredFile << "Время\t";
        for (size_t i = 0; i < peaks.size(); ++i) {
            restoredFile << "Гармоника_" << i + 1 << "\t";
        }
        restoredFile << "\n";

        for (int timeIdx = 0; timeIdx < numPoints; ++timeIdx) {
            double time = timeIdx * deltaT;
            restoredFile << time << "\t";
            for (size_t harmonicIdx = 0; harmonicIdx < peaks.size(); ++harmonicIdx) {
                restoredFile << restoredHarmonics[harmonicIdx][timeIdx] << "\t";
            }
            restoredFile << "\n";
        }
        restoredFile.close();
        cout << "Восстановленные гармоники записаны в restored.txt!" << endl;
    }
    else {
        cerr << "Ошибка при создании файла restored.txt!" << endl;
    }

    // Проверка точности восстановления
    vector<double> rmseErrors(peaks.size(), 0.0);

    // Сопоставление исходных и восстановленных гармоник по частоте
    for (size_t restoredIdx = 0; restoredIdx < peaks.size(); ++restoredIdx) {
        double restoredFreq = peaks[restoredIdx].frequency;

        // Поиск ближайшей исходной гармоники
        int closestOrigIdx = -1;
        double minDiff = INFINITY;
        for (int origIdx = 0; origIdx < numHarmonics; ++origIdx) {
            double origFreq = harmonics[origIdx].frequency;
            double diff = abs(restoredFreq - origFreq);
            if (diff < minDiff) {
                minDiff = diff;
                closestOrigIdx = origIdx;
            }
        }

        // Расчет ошибки, если найдено соответствие
        if (closestOrigIdx != -1 && minDiff < 1e-6) {
            double sumSq = 0.0;
            for (int timeIdx = 0; timeIdx < numPoints; ++timeIdx) {
                double origVal = signalValues[closestOrigIdx][timeIdx];
                double restoredVal = restoredHarmonics[restoredIdx][timeIdx];
                sumSq += (origVal - restoredVal) * (origVal - restoredVal);
            }
            rmseErrors[restoredIdx] = sqrt(sumSq / numPoints);
            cout << "Ошибка RMSE для гармоники " << restoredIdx + 1
                << " (частота " << restoredFreq << " Гц): "
                << rmseErrors[restoredIdx] << endl;
        }
    }

    return 0;
}
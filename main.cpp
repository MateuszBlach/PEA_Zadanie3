#include <iostream>
#include "headers\Menu.h"
#include "headers\Matrix.h"
#include <chrono>
#include <math.h>
#include "headers\MyFunctions.h"
#include <random>
#include <fstream>
#include <vector>


using namespace std;
using namespace std::chrono;

int mainMenuChoice;
Menu menu = Menu();
//Zmienie i obiekty potrzebne do wykonania algorytmu
Matrix matrix = Matrix();

int* bestPath;
int bestPathLength;
long long timeToFindBest;

string savePath;

//kryterium stopu
int stop;

//wspolczynniki
double crossingFactor,mutationFactor;

int populationSize;
int **population;

//zmienne potrzebne do wykresow
vector<long long> timeVector;
vector<double> bladVector;

void manageMainMenu();

int** generateStartingPopulation();
int* generateRandomChromosome();

void releasePopulationMemory();

int* inversionMutation();

double simulatedAnnealingAlgorithm(double temperature,int* startingPath, int startingPathLength);


void test();

void testBladWzgledny();

int opt;

int main()
{
    srand(time(NULL));

    do {
        menu.displayMainMenu();
        manageMainMenu();
    } while (mainMenuChoice != 0);
    menu.~Menu();

    delete[] bestPath;
    return 0;
}

void manageMainMenu() {
    string path;
    string readPath;
    int fileChoice;

    int* startingPath = nullptr;
    int startingPathLength;


    double temperature;
    double bladWzgledny;
    double wyrazenieTemperatury;

    int numberOfCities;

    cin >> mainMenuChoice;
    switch (mainMenuChoice) {
        case 1:
            //Wczytanie z pliku
            menu.displayFileMenu();
            cin >> fileChoice;

            switch (fileChoice) {
                case 1:
                    matrix.loadFromFile("testing_files/ftv47.atsp");
                    opt = 1776;
                    break;
                case 2:
                    matrix.loadFromFile("testing_files/ftv170.atsp");
                    opt = 2755;
                    break;
                case 3:
                    matrix.loadFromFile("testing_files/rbg403.atsp");
                    opt = 2465;
                    break;
                case 4:
                    cout << "Podaj sciezke do pliku" << endl;
                    cin >> path;
                    matrix.loadFromFile(path);
                    break;
                default:
                    break;
            }
            break;
        case 2:
            cout << "Podaj kryterium stopu w sekundach " << endl;
            cin >> stop;
            break;
        case 3:
            cout << "Podaj wspolczynnik krzyzowania" << endl;
            cin >> crossingFactor;
            break;
        case 4:
            cout << "Podaj wspolczynnik mutacji" << endl;
            cin >> mutationFactor;
            break;
        case 5:
            cout << "Podaj wielkosc populacji" << endl;
            cin >> populationSize;
            break;
        case 6:
            population = generateStartingPopulation();


            cout << "Dlugosc sciezki wyliczonej: " << bestPathLength << endl;

            for (int i = 0; i < matrix.numberOfCities; i++) {
                cout << bestPath[i] << "->";
            }
            cout << bestPath[matrix.numberOfCities] << endl;
            cout << "Czas potrzebny na znalezienie rozwiazania: " << timeToFindBest/1000000 << "s" << endl;

            bladWzgledny = abs(bestPathLength - opt) / (double)opt;
            cout << "Blad wzgledny " << bladWzgledny << "%" << endl;

            releasePopulationMemory();
            break;
        case 7:
            test();
            break;
        default:
            break;


    }


}

int** generateStartingPopulation(){
    int** startingPopulation = new int*[populationSize];
    for(int i = 0; i < populationSize; i++){
        startingPopulation[i] = generateRandomChromosome();
    }
    return startingPopulation;
}


int* generateRandomChromosome() {
    bool* visited = new bool[matrix.numberOfCities];
    int* chromosome = new int[matrix.numberOfCities];

    int starting = generateRandomInt(0,matrix.numberOfCities-1);

    for (int i = 0; i < matrix.numberOfCities; i++) {
        visited[i] = false;
    }
    visited[starting] = true;
    chromosome[0] = starting;

    int nextCity;
    for (int i = 0; i < matrix.numberOfCities - 1; i++) {
        do {
            nextCity = generateRandomInt(0,matrix.numberOfCities-1);
        } while (!visited[nextCity]);
        visited[nextCity] = true;
        chromosome[i + 1] = nextCity;
    }
    delete[] visited;
    return chromosome;
}

void releasePopulationMemory(){
    for (int i = 0; i < populationSize; ++i) {
        delete[] population[i];
    }
    delete[] population;
}

int* inversionMutation(int* chromosome){
    int firstIndex;
    int secondIndex;
    do{
        firstIndex = generateRandomInt(0,matrix.numberOfCities-1);
        secondIndex = generateRandomInt(0,matrix.numberOfCities-1);
    }while(abs(firstIndex-secondIndex) < 2);
    if(firstIndex > secondIndex){
        int holder = firstIndex;
        firstIndex = secondIndex;
        secondIndex = holder;
    }

    int* mutatedChromosome = new int[matrix.numberOfCities];
    for(int i = 0;  i < firstIndex; i++){
        mutatedChromosome[i] = chromosome[i];
    }
    //TODO
    for(int i = firstIndex;  i < secondIndex; i++){
        mutatedChromosome[i] = chromosome[i];
    }

}


double simulatedAnnealingAlgorithm(double temperature, int* startingPath, int startingPathLength) {
    steady_clock::time_point start = steady_clock::now();
    steady_clock::duration duration;

    //na potrzeby stworzenia wykresow
    timeVector.clear();
    bladVector.clear();

    //na poczatek najlepsza sciezka jest sciezka startowa - wyliczona metoda zachlanna
    bestPath = startingPath;
    bestPathLength = startingPathLength;

    //sciezka najlepsza lokalnie
    int* currentBestPath = new int[matrix.numberOfCities + 1];
    for (int i = 0; i <= matrix.numberOfCities; i++) {
        currentBestPath[i] = startingPath[i];
    }
    int currentBestPathLength = startingPathLength;

    //zmienna pomocna do losowej zmiany w tabeli
    int temporary;

    do {
        //sprawdzenie czy lokalnie najlepsza sciezka jest lepsza od najlepszej globalnie
        if (currentBestPathLength < bestPathLength) {
            duration = steady_clock::now() - start;
            timeToFindBest = duration_cast<microseconds>(duration).count();
            delete[] bestPath;
            bestPath = new int[matrix.numberOfCities + 1];
            for (int i = 0; i <= matrix.numberOfCities; i++) {
                bestPath[i] = currentBestPath[i];
            }
            bestPathLength = currentBestPathLength;

            timeVector.push_back(timeToFindBest);
            bladVector.push_back((bestPathLength - opt) / (double)opt);
        }

        //losowanie indeksow
        int v1, v2;
        do {
            v1 = generateRandomInt(1, matrix.numberOfCities - 1);
            v2 = generateRandomInt(1, matrix.numberOfCities - 1);
        } while (
                v1 == v2
                );


        int* newPath = new int[matrix.numberOfCities + 1];
        int newPathLength;
        //kopia lokalnej najlepszej do nowej sciezki losowej
        for (int i = 0; i <= matrix.numberOfCities; i++) {
            newPath[i] = currentBestPath[i];
        }

        //zamiana indeksow wylosowanych
        temporary = newPath[v2];
        newPath[v2] = newPath[v1];
        newPath[v1] = temporary;


        //obliczenie nowo powstałej trasy
        newPathLength = 0;
        for (int i = 0; i < matrix.numberOfCities; i++) {
            newPathLength += matrix.matrix[newPath[i]][newPath[i + 1]];
        }

        //decyzja czy zmiana nastąpi
        if (newPathLength < currentBestPathLength) {
            delete[] currentBestPath;
            currentBestPath = new int[matrix.numberOfCities + 1];
            for (int i = 0; i <= matrix.numberOfCities; i++) {
                currentBestPath[i] = newPath[i];
            }
            currentBestPathLength = newPathLength;
        }
        else {
            double probability = exp(-abs(newPathLength - currentBestPathLength) / temperature);
            random_device rd;
            mt19937 gen(rd());
            uniform_real_distribution<double> dis(0.0, 1.0);
            double randomDouble = dis(gen);

            if (randomDouble < probability) {
                delete[] currentBestPath;
                currentBestPath = new int[matrix.numberOfCities + 1];
                for (int i = 0; i <= matrix.numberOfCities; i++) {
                    currentBestPath[i] = newPath[i];
                }
                currentBestPathLength = newPathLength;
            }
        }

        delete[] newPath;

        //schładzanie
        temperature *= alpha;

        //sprawdzenie czasu
        duration = steady_clock::now() - start;
    } while (duration_cast<seconds>(duration).count() < static_cast<long long>(stop) * 1);
    return temperature;
}

void savePathToFile(int* path) {
    //ofstream outfile(savePath, std::ios::out | std::ios::app);
    ofstream outfile(savePath);
    if (outfile.is_open()) {
        outfile << matrix.numberOfCities << endl;
        for (int i = 0; i <= matrix.numberOfCities; i++) {
            outfile << path[i] << endl;;
        }
        outfile.close();
    }
    else {
        std::cout << "Nie mozna otworzyc pliku." << endl;
    }
}

void readPathFromFile(string filePath) {
    ifstream  myFile(filePath);
    if (myFile.fail()) {
        cout << "Podano bledna sciezke do pliku!" << endl;
    }
    else {
        bestPathLength = 0;

        int len;
        myFile >> len;
        delete[] bestPath;
        bestPath = new int[len+1];

        for (int i = 0; i <= len; i++) {
            myFile >> bestPath[i];
        }


        myFile.close();
    }
}

void test() {

    double testingAlpha[] = { 0.99999975,0.999999925};
    int stopArr[] = { 60,120,240 };
    int optArr[] = { 1608,2755,1163 };
    string filePaths[] = { "testing_files/ftv55.atsp","testing_files/ftv170.atsp","testing_files/rbg358.atsp" };
    string savePaths[] = { "testing_files/sa_ftv55.txt","testing_files/sa_ftv170.txt","testing_files/sa_rbg358.txt" };



    int* startingPath = nullptr;
    int startingPathLength;


    double temperature;
    double bladWzgledny;
    double wyrazenieTemperatury;


    for (int fileNumber = 0; fileNumber < 3; fileNumber++) {
        matrix.loadFromFile(filePaths[fileNumber]);
        savePath = savePaths[fileNumber];
        stop = stopArr[fileNumber];
        opt = optArr[fileNumber];

        int* bestForFile = new int[matrix.numberOfCities + 1];
        int bestForFileLen = INT_MAX;

        for (int alphaNumber = 0; alphaNumber < 2; alphaNumber++) {
            cout << "Testowanie alpha:" << testingAlpha[alphaNumber] << " plik " << filePaths[fileNumber] << endl;
            alpha = testingAlpha[alphaNumber];
            cout << "alpha " << alpha << endl;
            for (int k = 0; k < 10; k++) {

                startingPath = generateStartingPath();
                startingPathLength = 0;
                for (int i = 0; i < matrix.numberOfCities; i++) {
                    startingPathLength += matrix.matrix[startingPath[i]][startingPath[i + 1]];
                }

                temperature = calculateStartingTemperature();
                temperature = simulatedAnnealingAlgorithm(temperature, startingPath, startingPathLength);
                bladWzgledny = abs(bestPathLength - opt) / (double)opt;
                wyrazenieTemperatury = exp(-1 / temperature);

                ofstream outfile("testing_files/testResults.txt", std::ios::out | std::ios::app);
                if (outfile.is_open()) {
                    outfile << bestPathLength;
                    outfile << ";";
                    outfile << bladWzgledny;
                    outfile << ";";
                    outfile << temperature;
                    outfile << ";";
                    outfile << wyrazenieTemperatury;
                    outfile << ";";
                    outfile << alpha;
                    outfile << ";";
                    outfile << timeToFindBest;
                    outfile << ";" << endl;
                    outfile.close();
                }
                else {
                    cout << "Nie mozna otworzyc pliku." << endl;
                }

                if (bestPathLength < bestForFileLen) {
                    bestForFileLen = bestPathLength;
                    delete[] bestForFile;
                    bestForFile = new int[matrix.numberOfCities + 1];
                    for (int i = 0; i <= matrix.numberOfCities; i++) {
                        bestForFile[i] = bestPath[i];
                    }
                }

            }
        }
        savePathToFile(bestForFile);
        delete[] bestForFile;
        //zapis najlepszego rozwiazania
    }

}

void testBladWzgledny() {
    double testingAlpha[] = {
            0.99999975,
            0.9999999,
            0.999999925 };
    int stopArr[] = { 60,120,240 };
    int optArr[] = { 1608,2755,1163 };
    string filePaths[] = { "testing_files/ftv55.atsp","testing_files/ftv170.atsp","testing_files/rbg358.atsp" };


    int* startingPath = nullptr;
    int startingPathLength;


    double temperature;
    double bladWzgledny;
    double wyrazenieTemperatury;


    for (int fileNumber = 0; fileNumber < 3; fileNumber++) {
        matrix.loadFromFile(filePaths[fileNumber]);
        stop = stopArr[fileNumber];
        opt = optArr[fileNumber];


        for (int alphaNumber = 0; alphaNumber < 3; alphaNumber++) {
            alpha = testingAlpha[alphaNumber];
            startingPath = generateStartingPath();
            startingPathLength = 0;
            for (int i = 0; i < matrix.numberOfCities; i++) {
                startingPathLength += matrix.matrix[startingPath[i]][startingPath[i + 1]];
            }

            temperature = calculateStartingTemperature();
            temperature = simulatedAnnealingAlgorithm(temperature, startingPath, startingPathLength);

            ofstream outfile("testing_files/testBlad.txt", std::ios::out | std::ios::app);
            if (outfile.is_open()) {
                outfile << filePaths[fileNumber] << ";" << testingAlpha[alphaNumber] << ";" << endl;
                for (int i = 0; i < bladVector.size(); i++) {
                    outfile << timeVector[i] << ";";
                }
                outfile << endl << endl;
                for (int i = 0; i < bladVector.size(); i++) {
                    outfile << bladVector[i] << ";";
                }
                outfile << endl;
                outfile.close();
            }
            else {
                cout << "Nie mozna otworzyc pliku." << endl;
            }

        }
    }
}
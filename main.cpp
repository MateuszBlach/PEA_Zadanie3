#include <iostream>
#include "headers\Menu.h"
#include "headers\Matrix.h"
#include "headers\Chromosome.h"
#include <chrono>
#include <math.h>
#include "headers\MyFunctions.h"
#include <random>
#include <fstream>
#include <vector>
#include <algorithm>
#include <set>



using namespace std;
using namespace std::chrono;

int mainMenuChoice;
Menu menu = Menu();
//Zmienie i obiekty potrzebne do wykonania algorytmu
Matrix matrix = Matrix();
Chromosome bestChromosome;
int bestChromosomeLength;
long long timeToFindBest;

string savePath;

//kryterium stopu
int stop;

//wspolczynniki
int crossingFactor,mutationFactor;

int populationSize;
vector<Chromosome> population;

//zmienne potrzebne do wykresow
vector<long long> timeVector;
vector<double> bladVector;

vector<long long> bestTimeVector;
vector<double> bestBladVector;


void manageMainMenu();
vector<Chromosome> generateStartingPopulation();
Chromosome generateRandomChromosome();
Chromosome inversionMutation();
Chromosome orderCrossover(Chromosome firstParent, Chromosome secondParent, int firstIndex, int secondIndex);
void geneticAlgorithm();
int calculateChromosomeLength(Chromosome chromosome);
void selection();
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

    return 0;
}

void manageMainMenu() {
    string path;
    string readPath;
    int fileChoice;


    double bladWzgledny;

    int numberOfCities;

    cin >> mainMenuChoice;
    switch (mainMenuChoice) {
        case 1:
            //Wczytanie z pliku
            menu.displayFileMenu();
            cin >> fileChoice;

            switch (fileChoice) {
                case 1:
                    matrix.loadFromFile("../testing_files/ftv47.atsp");
                    opt = 1776;
                    break;
                case 2:
                    matrix.loadFromFile("../testing_files/ftv170.atsp");
                    opt = 2755;
                    break;
                case 3:
                    matrix.loadFromFile("../testing_files/rbg403.atsp");
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
            bestChromosomeLength = INT_MAX;

            population = generateStartingPopulation();


            cout << "Dlugosc najlepszego chromosomu przed algorytmem: " << bestChromosomeLength << endl;
            for (int i = 0; i < matrix.numberOfCities; i++) {
                cout << bestChromosome.genes[i] << "->";
            }
            cout << bestChromosome.genes[matrix.numberOfCities] << endl;

            geneticAlgorithm();

            cout << "Dlugosc najlepszego chromosomu po algorytmie: " << bestChromosomeLength << endl;
            for (int i = 0; i < matrix.numberOfCities; i++) {
                cout << bestChromosome.genes[i] << "->";
            }
            cout << bestChromosome.genes[matrix.numberOfCities] << endl;
            cout << "Czas potrzebny na znalezienie rozwiazania: " << timeToFindBest/1000000 << "s" << endl;
            bladWzgledny = abs(bestChromosomeLength - opt) / (double)opt;
            cout << "Blad wzgledny " << bladWzgledny << "%" << endl;

            break;
        case 7:
            test();
            break;
        default:
            break;


    }


}

vector<Chromosome> generateStartingPopulation(){
    vector<Chromosome> startingPopulation;
    for(int i = 0; i < populationSize; i++){
        startingPopulation.push_back(generateRandomChromosome());
        int chromosomeLength = calculateChromosomeLength(startingPopulation[i]);
        if(chromosomeLength < bestChromosomeLength){
            bestChromosomeLength = chromosomeLength;
            bestChromosome = startingPopulation[i];
        }
    }
    return startingPopulation;
}

Chromosome generateRandomChromosome() {
    bool* visited = new bool[matrix.numberOfCities];
    Chromosome chromosome;

    int starting = generateRandomInt(0,matrix.numberOfCities-1);

    for (int i = 0; i < matrix.numberOfCities; i++) {
        visited[i] = false;
    }
    visited[starting] = true;
    chromosome.genes.push_back(starting);

    int nextCity;
    for (int i = 0; i < matrix.numberOfCities - 1; i++) {
        do {
            nextCity = generateRandomInt(0,matrix.numberOfCities-1);
        } while (visited[nextCity]);
        visited[nextCity] = true;
        chromosome.genes.push_back(nextCity);
    }
    delete[] visited;
    chromosome.value = calculateChromosomeLength(chromosome);

    set<int> mySet(chromosome.genes.begin(), chromosome.genes.end());
    if(chromosome.genes.size() == mySet.size()){
        return chromosome;
    }else{
        cout << "zle" << endl;
        throw exception();
    }
}

Chromosome inversionMutation(Chromosome chromosome){
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

    Chromosome mutatedChromosome;
    for(int i = 0;  i < firstIndex; i++){
        mutatedChromosome.genes.push_back(chromosome.genes[i]);
    }

    for(int i = firstIndex, j =  secondIndex ;  i <= secondIndex; i++,j--){
        mutatedChromosome.genes.push_back(chromosome.genes[j]);
    }

    for(int i = secondIndex+1; i < matrix.numberOfCities;i++){
        mutatedChromosome.genes.push_back(chromosome.genes[i]);
    }
    mutatedChromosome.value = calculateChromosomeLength(mutatedChromosome);


    set<int> mySet(mutatedChromosome.genes.begin(), mutatedChromosome.genes.end());
    if(mutatedChromosome.genes.size() == mySet.size()){
        return mutatedChromosome;
    }else{
        cout << "zle " << mutatedChromosome.genes.size() << " " << mySet.size() <<  endl;
        throw exception();
    }

}

int calculateChromosomeLength(Chromosome chromosome){
    int length = 0;
    for (int i = 0; i < matrix.numberOfCities-1; i++) {
        length += matrix.matrix[chromosome.genes[i]][chromosome.genes[i + 1]];
    }
    length += matrix.matrix[chromosome.genes[matrix.numberOfCities-1]][chromosome.genes[0]];
    return length;
}

Chromosome orderCrossover(Chromosome firstParent, Chromosome secondParent, int firstIndex, int secondIndex){

    Chromosome newChromosome;
    for(int i = 0; i < matrix.numberOfCities; i++){
        newChromosome.genes.push_back(-1);
    }

    vector<bool> taken(matrix.numberOfCities,false);

    for(int i = firstIndex; i < secondIndex;i++){
        newChromosome.genes[i] = firstParent.genes[i];
        taken[newChromosome.genes[i]] = true;
    }


    int newChromosomePos = secondIndex;

    for(int i = secondIndex; i < matrix.numberOfCities; i++){
        if(!taken[secondParent.genes[i]]){
            newChromosome.genes[newChromosomePos] = secondParent.genes[i];
            taken[secondParent.genes[i]] = true;
            if(newChromosomePos == matrix.numberOfCities-1)
                newChromosomePos = (newChromosomePos + 1) % matrix.numberOfCities;
            else
                newChromosomePos++;
        }
    }

    for(int i = 0; i <= firstIndex; i++){
        if(!taken[secondParent.genes[i]]){
            newChromosome.genes[newChromosomePos] = secondParent.genes[i];
            taken[secondParent.genes[i]] = true;
            if(newChromosomePos == matrix.numberOfCities-1)
                newChromosomePos = (newChromosomePos + 1) % matrix.numberOfCities;
            else
                newChromosomePos++;
        }
    }

    for(int i = secondIndex; i < matrix.numberOfCities; i++){
        if(!taken[firstParent.genes[i]]){
            newChromosome.genes[newChromosomePos] = firstParent.genes[i];
            taken[firstParent.genes[i]] = true;
            if(newChromosomePos == matrix.numberOfCities-1)
                newChromosomePos = (newChromosomePos + 1) % matrix.numberOfCities;
            else
                newChromosomePos++;
        }
    }

    for(int i = 0; i <= firstIndex; i++) {
        if (!taken[firstParent.genes[i]]) {
            newChromosome.genes[newChromosomePos] = firstParent.genes[i];
            taken[firstParent.genes[i]] = true;
            if(newChromosomePos == matrix.numberOfCities-1)
                newChromosomePos = (newChromosomePos + 1) % matrix.numberOfCities;
            else
                newChromosomePos++;
        }
    }

    newChromosome.value = calculateChromosomeLength(newChromosome);

        set<int> mySet(newChromosome.genes.begin(), newChromosome.genes.end());
        if(newChromosome.genes.size() == mySet.size()){
            return newChromosome;
        }else{
            throw exception();
        }

}

void geneticAlgorithm() {
    steady_clock::time_point start = steady_clock::now();
    steady_clock::duration duration;

    timeVector.clear();
    bladVector.clear();

    int crossoverStatus, crossoverFirstIndex;

    do {
        crossoverStatus = 0;
        crossoverFirstIndex = 0;
        for(int i = 0; i < population.size(); i++){
            int chance = generateRandomInt(1,100);

            if(chance < crossingFactor){
                if(crossoverStatus == 0){
                    crossoverStatus = 1;
                    crossoverFirstIndex = i;
                }else{
                    crossoverStatus = 0;
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

                    Chromosome firstNewChromosome = orderCrossover(population[crossoverFirstIndex],population[i],firstIndex,secondIndex);

                    if(firstNewChromosome.value < bestChromosomeLength){
                        duration = steady_clock::now() - start;
                        timeToFindBest = duration_cast<microseconds>(duration).count();

                        timeVector.push_back(timeToFindBest);
                        bladVector.push_back((bestChromosomeLength - opt) / (double)opt);

                        bestChromosome = firstNewChromosome;
                        bestChromosomeLength = firstNewChromosome.value;
                    }
                    population.push_back(firstNewChromosome);

                    Chromosome secondNewChromosome = orderCrossover(population[i],population[crossoverFirstIndex],firstIndex,secondIndex);
                    if(secondNewChromosome.value < bestChromosomeLength){
                        duration = steady_clock::now() - start;
                        timeToFindBest = duration_cast<microseconds>(duration).count();

                        timeVector.push_back(timeToFindBest);
                        bladVector.push_back(100.0*(bestChromosomeLength - opt) / (double)opt);

                        bestChromosome = secondNewChromosome;
                        bestChromosomeLength = secondNewChromosome.value;
                    }
                    population.push_back(secondNewChromosome);
                }
            }else if(chance < crossingFactor + mutationFactor){
                Chromosome mutatedChromosome = inversionMutation(population[i]);
                if(mutatedChromosome.value < bestChromosomeLength){
                    duration = steady_clock::now() - start;
                    timeToFindBest = duration_cast<microseconds>(duration).count();

                    timeVector.push_back(timeToFindBest);
                    bladVector.push_back((bestChromosomeLength - opt) / (double)opt);

                    bestChromosome = mutatedChromosome;
                    bestChromosomeLength = mutatedChromosome.value;
                }
                population.push_back(mutatedChromosome);
            }
        }
        selection();

        duration = steady_clock::now() - start;
    } while (duration_cast<seconds>(duration).count() < static_cast<long long>(stop) * 1);
}

void selection(){
    sort(population.begin(), population.end(), [](const Chromosome& lhs, const Chromosome& rhs) {
        return lhs.value < rhs.value;
    });

    while (population.size() > populationSize){
        population.pop_back();
    }
}


void test() {
    int optArr[] = { 1776,2755,2465 };
    int stopArr[] = { 60, 120,240};
    int populationSizeArr[] = {1000,2500,5000};
    string filePaths[] = { "../testing_files/ftv47.atsp","../testing_files/ftv170.atsp","../testing_files/rbg403.atsp" };

    crossingFactor = 80;
    mutationFactor = 1;

    for (int fileNumber = 0; fileNumber < 3; fileNumber++) {
        for(int pop : populationSizeArr) {

            populationSize = pop;

            matrix.loadFromFile(filePaths[fileNumber]);
            stop = stopArr[fileNumber];
            opt = optArr[fileNumber];

            int bestForFileLen = INT_MAX;

            double bladWzgledny;

            for (int k = 0; k < 10; k++) {
                cout << "Testowanie plik " << filePaths[fileNumber]<< " populacja " << pop << " iteracja " << k+1 << endl;

                bestChromosomeLength = INT_MAX;

                population = generateStartingPopulation();

                geneticAlgorithm();

                bladWzgledny = 100.0 * abs(bestChromosomeLength - opt) / (double) opt;

                ofstream outfile("../testing_files/testResults.txt", std::ios::out | std::ios::app);
                if (outfile.is_open()) {
                    outfile << pop;
                    outfile << ";";
                    outfile << bestChromosomeLength;
                    outfile << ";";
                    outfile << bladWzgledny;
                    outfile << ";";
                    outfile << timeToFindBest/1000000.0;
                    outfile << ";" << endl;
                    outfile.close();
                } else {
                    cout << "Nie mozna otworzyc pliku." << endl;
                }

                if (bestChromosomeLength < bestForFileLen) {
                    bestForFileLen = bestChromosomeLength;
                    bestBladVector = bladVector;
                    bestTimeVector = timeVector;
                }
            }

            ofstream outfile("../testing_files/testBlad.txt", std::ios::out | std::ios::app);
            if (outfile.is_open()) {
                outfile << filePaths[fileNumber] << ";" <<pop << ";" << endl;
                for (int i = 0; i < bladVector.size(); i++) {
                    outfile << timeVector[i]/1000000.0 << ";";
                }
                outfile << endl << endl;
                for (int i = 0; i < bladVector.size(); i++) {
                    outfile << bladVector[i] << ";";
                }
                outfile << endl;
                outfile.close();
            } else {
                cout << "Nie mozna otworzyc pliku." << endl;
            }
        }
    }
}

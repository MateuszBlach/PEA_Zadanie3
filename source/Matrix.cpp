#include "..\headers\Matrix.h"
#include <iostream>
#include <fstream>
#include <iomanip>


using namespace std;

Matrix::Matrix(){
	matrix = nullptr;
	numberOfCities = 0;
}

Matrix::~Matrix(){
	if (numberOfCities != 0) {
		for (int i = 0; i < numberOfCities; ++i) {
			delete[] matrix[i];
			numberOfCities = 0;
		}
		delete[] matrix;
	}
}

void Matrix::loadFromFile(string filePath){
	ifstream  myFile(filePath);
	if (myFile.fail()) {
		cout << "Podano bledna sciezke do pliku!" << endl;
	}
	else {
		Matrix::~Matrix();

		string line;

		getline(myFile, line);
		getline(myFile, line);
		getline(myFile, line);

		string temp;
		myFile >> temp;
		int tempInt;
		myFile >> tempInt;
		numberOfCities = tempInt;

		getline(myFile, line);
		getline(myFile, line);
		getline(myFile, line);
		getline(myFile, line);

		matrix = new int* [numberOfCities];
		for (int i = 0; i < numberOfCities; i++) matrix[i] = new int[numberOfCities];

		for (int i = 0; i < numberOfCities; i++) {
			for (int j = 0; j < numberOfCities; j++) {
				int value;
				myFile >> value;
				matrix[i][j] = value;
			}
		}
	

		myFile.close();
	}
}


void Matrix::display() {
	cout << "   ";
	for (int i = 0; i < numberOfCities; i++) cout << setw(4) << i;
	cout << endl;
	for (int i = 0; i < numberOfCities; i++)
	{
		cout << setw(3) << i;
		for (int j = 0; j < numberOfCities; j++) cout << setw(4) << matrix[i][j];
		cout << endl;
	}
}
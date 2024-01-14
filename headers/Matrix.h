#pragma once
#include <string>

using namespace std;

class Matrix {

public:
	int** matrix;
	int numberOfCities;

	Matrix();
	~Matrix();

	void loadFromFile(string filePath);
	void display();
};
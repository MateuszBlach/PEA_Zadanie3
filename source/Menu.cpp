#include "..\headers\Menu.h"
#include <iostream>

using namespace std;

Menu::Menu() {}

Menu::~Menu() {}

void Menu::displayMainMenu() {
	cout << "Glowne menu" << endl;
	cout << "1. Wczytaj dane z pliku" << endl;
	cout << "2. Podaj kryterium stopu" << endl;
	cout << "3. Podaj wspolczynnik krzyzowania" << endl;
    cout << "4. Podaj wspolczynnik mutacji" << endl;
    cout << "5. Podaj wielkosc populacji" << endl;
	cout << "6. Uruchom algorytm genetyczny" << endl;
	cout << "7. Testy" << endl;
	cout << "0. Zakoncz program" << endl;
	cout << "Wybor: ";
}

void Menu::displayFileMenu() {
	cout << "1. ftv47.atsp" << endl;
	cout << "2. ftv170.atsp" << endl;
	cout << "3. rbg403.atsp" << endl;
	cout << "4. Inny plik" << endl;
	cout << "Wybor: ";
}
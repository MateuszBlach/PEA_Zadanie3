#include <cstdlib>
#include "..\headers\MyFunctions.h"

using namespace std;

int generateRandomInt(int min, int max) {
	return rand() % (max - min + 1) + min;
}


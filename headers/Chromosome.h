#pragma once
#include <vector>
using namespace std;

class Chromosome {

public:
    vector<int> genes;
    int value;
    Chromosome();
    ~Chromosome();
};
#include "../lib/all_headers.h"
#include<iostream>
using namespace std;

int main(int argc, char **argv) {
    BaseFeature* f;
    if(argc!=4)
        throw("Number of arguments is not enough.");
    if(string(argv[1])==string("eye-color"))
    {
        f = new EyeColor(argc, argv);
    }
    else
    {
        throw("Invalid feature");
        return -1;
    }
    f->calculateAndDisplay();

    return 0;
}
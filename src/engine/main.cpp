#include "../../lib/all_headers.h"
#include<iostream>
using namespace std;

int main(int argc, char **argv) {
    BaseFeature* f;

    if(argc<4)
        throw("Number of arguments is not enough.");
    if(string(argv[1])==string("eye_color"))
    {
        f = new EyeColor(argc, argv);
    }
    else if(string(argv[1])==string("add_beard"))
    {
        f = new AddBeard(argc, argv);
    }
    else if(string(argv[1])==string("add_smile")) {
        f = new AddSmile(argc, argv);
    }
    else
    {
        throw("Invalid feature");
        return -1;
    }

    f->calculateAndDisplay();

    return 0;
}

#include "../lib/all_headers.h"
#include<iostream>
using namespace std;

int main(int argc, char **argv) {
    BaseFeature* f;
/*    cout<<argc<<endl;
    for(int i=0; i<argc; i++)
        cout<<argv[i]<<endl;*/

    if(argc!=4)
        throw("Number of arguments is not enough.");
    if(string(argv[1])==string("EyeColor"))
    {
        f = new EyeColor(argc, argv);
    }
    else if(string(argv[1])==string("AddBeard"))
    {
        f = new AddBeard(argc, argv);
    }
    else
    {
        throw("Invalid feature");
        return -1;
    }

    f->calculateAndDisplay();

    return 0;
}
#include <iostream>
#include <string>
using namespace std;

int func(int argc, char** argv) {
    /* using std:: */
    std::string str1 = "world";
    std::cout << "Hello " << str1 << std::endl;

    /* Big Brainlet hours */
    string str2 = "world";
    cout << "Hello " << str2 << endl;

    return 0;
}

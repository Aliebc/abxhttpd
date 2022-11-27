
#include <abxhttpd/hash.H>
#include <iostream>
#include <string>
using namespace std;
int main(){
    cout << abxhttpd::Hash::SHA256("Hi") << endl;
    return 0;
}
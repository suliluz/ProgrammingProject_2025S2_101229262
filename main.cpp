#include <iostream>
#include "headers/List.h"

using namespace std;

int main() {
    auto list = List<int>();

    list.push(1);
    list.push(2);
    list.push(3);
    list.push(4);
    list.push(5);

    cout << "From head: " << endl;

    for (auto it = list.getIterator(); it != it.end(); ++it) {
        cout << it.getCurrent()->getValue() << endl;
    }

    cout << "From tail: " << endl;
    for (auto it = list.getReverseIterator(); it != it.end(); --it) {
        cout << it.getCurrent()->getValue() << endl;
    }

    return 0;
}
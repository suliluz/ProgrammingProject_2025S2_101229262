#include <iostream>
#include "headers/List.h"
#include "headers/Stack.h"
#include "headers/Queue.h"

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

    // Test stack
    auto stack = Stack<int>();

    stack.push(1);
    stack.push(2);
    stack.push(3);
    stack.push(4);

    cout << "Current stack size: " << stack.size() << endl;
    cout << "Is empty: " << stack.isEmpty() << endl;

    // Pop stack two times
    cout << "Popped: " << stack.pop() << endl;
    cout << "Popped: " << stack.pop() << endl;

    // Return size
    cout << "Current stack size: " << stack.size() << endl;

    // Pop the rest
    for (int i = 0; i <= stack.size(); i++) {
        cout << stack.pop() << endl;
    }

    // Check empty again
    cout << "Is empty: " << stack.isEmpty() << endl;

    return 0;
}

#include <iostream>
#include <cassert>
#include "base/headers/Stack.h"
#include "base/headers/Queue.h"

void testStack() {
    std::cout << "Testing Stack with SinglyLinkedList..." << std::endl;

    Stack<int> stack;

    // Test isEmpty on new stack
    assert(stack.isEmpty() == true);
    assert(stack.size() == 0);

    // Test push
    stack.push(10);
    stack.push(20);
    stack.push(30);

    assert(stack.isEmpty() == false);
    assert(stack.size() == 3);

    // Test top (LIFO - Last In First Out)
    assert(stack.top() == 30);

    // Test pop
    assert(stack.pop() == 30);
    assert(stack.size() == 2);
    assert(stack.top() == 20);

    assert(stack.pop() == 20);
    assert(stack.pop() == 10);

    assert(stack.isEmpty() == true);
    assert(stack.size() == 0);

    std::cout << "✓ Stack tests passed!" << std::endl;
}

void testQueue() {
    std::cout << "Testing Queue with SinglyLinkedList..." << std::endl;

    Queue<int> queue;

    // Test isEmpty on new queue
    assert(queue.isEmpty() == true);
    assert(queue.size() == 0);

    // Test enqueue
    queue.enqueue(10);
    queue.enqueue(20);
    queue.enqueue(30);

    assert(queue.isEmpty() == false);
    assert(queue.size() == 3);

    // Test front (FIFO - First In First Out)
    assert(queue.front() == 10);

    // Test dequeue
    assert(queue.dequeue() == 10);
    assert(queue.size() == 2);
    assert(queue.front() == 20);

    assert(queue.dequeue() == 20);
    assert(queue.dequeue() == 30);

    assert(queue.isEmpty() == true);
    assert(queue.size() == 0);

    std::cout << "✓ Queue tests passed!" << std::endl;
}

void testStackWithStrings() {
    std::cout << "Testing Stack with strings..." << std::endl;

    Stack<std::string> stack;
    stack.push("first");
    stack.push("second");
    stack.push("third");

    assert(stack.top() == "third");
    assert(stack.pop() == "third");
    assert(stack.pop() == "second");
    assert(stack.pop() == "first");

    std::cout << "✓ Stack string tests passed!" << std::endl;
}

void testQueueWithStrings() {
    std::cout << "Testing Queue with strings..." << std::endl;

    Queue<std::string> queue;
    queue.enqueue("first");
    queue.enqueue("second");
    queue.enqueue("third");

    assert(queue.front() == "first");
    assert(queue.dequeue() == "first");
    assert(queue.dequeue() == "second");
    assert(queue.dequeue() == "third");

    std::cout << "✓ Queue string tests passed!" << std::endl;
}

int main() {
    std::cout << "\n=== Testing SinglyLinkedList-based Stack and Queue ===\n" << std::endl;

    try {
        testStack();
        testQueue();
        testStackWithStrings();
        testQueueWithStrings();

        std::cout << "\n✓ All tests passed successfully!" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}

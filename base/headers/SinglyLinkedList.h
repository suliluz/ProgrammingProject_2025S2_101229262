#pragma once
#include "SinglyLinkedNode.h"
#include "ForwardIterator.h"

using namespace std;

template <class T>
class SinglyLinkedList {
private:
    typedef SinglyLinkedNode<T> Node;

    Node* head;
    Node* tail;
    int count;

public:
    // Constructor
    SinglyLinkedList() : head(&Node::NIL), tail(&Node::NIL), count(0) {}

    // Destructor
    ~SinglyLinkedList() {
        clear();
    }

    // Copy constructor
    SinglyLinkedList(const SinglyLinkedList& other) : head(&Node::NIL), tail(&Node::NIL), count(0) {
        Node* current = other.head;
        while (current != &Node::NIL) {
            push(current->getValue());
            current = current->getNext();
        }
    }

    // Copy assignment
    SinglyLinkedList& operator=(const SinglyLinkedList& other) {
        if (this != &other) {
            clear();
            Node* current = other.head;
            while (current != &Node::NIL) {
                push(current->getValue());
                current = current->getNext();
            }
        }
        return *this;
    }

    // Push to back (for Queue enqueue and general use)
    void push(const T& value) {
        Node* newNode = new Node(value);
        newNode->setNext(&Node::NIL);

        if (isEmpty()) {
            head = newNode;
            tail = newNode;
        } else {
            tail->setNext(newNode);
            tail = newNode;
        }
        count++;
    }

    // Push to front (for Stack push - O(1))
    void pushFront(const T& value) {
        Node* newNode = new Node(value);
        newNode->setNext(head);
        head = newNode;

        if (tail == &Node::NIL) {
            tail = newNode;
        }
        count++;
    }

    // Pop from front (for Stack pop and Queue dequeue - O(1))
    T popFront() {
        if (isEmpty()) {
            throw runtime_error("Cannot pop from empty SinglyLinkedList");
        }

        Node* oldHead = head;
        T value = oldHead->getValue();
        head = head->getNext();

        if (head == &Node::NIL) {
            tail = &Node::NIL;
        }

        delete oldHead;
        count--;
        return value;
    }

    // Get first element (const)
    const T& getFirst() const {
        if (isEmpty()) {
            throw runtime_error("List is empty");
        }
        return head->getValue();
    }

    // Get last element (const)
    const T& getLast() const {
        if (isEmpty()) {
            throw runtime_error("List is empty");
        }
        return tail->getValue();
    }

    // Check if empty
    [[nodiscard]]
    bool isEmpty() const {
        return head == &Node::NIL;
    }

    // Get size
    [[nodiscard]]
    int length() const {
        return count;
    }

    // Clear all elements
    void clear() {
        while (!isEmpty()) {
            popFront();
        }
    }

    // Get iterator
    ForwardIterator<T> getIterator() {
        return ForwardIterator<T>(head);
    }

    // Get const iterator
    ForwardIterator<T> getIterator() const {
        return ForwardIterator<T>(head);
    }

    // Find element by value (returns true if found)
    bool contains(const T& value) const {
        Node* current = head;
        while (current != &Node::NIL) {
            if (current->getValue() == value) {
                return true;
            }
            current = current->getNext();
        }
        return false;
    }

    // Remove first occurrence of value
    bool remove(const T& value) {
        if (isEmpty()) {
            return false;
        }

        // Special case: removing head
        if (head->getValue() == value) {
            Node* oldHead = head;
            head = head->getNext();
            if (head == &Node::NIL) {
                tail = &Node::NIL;
            }
            delete oldHead;
            count--;
            return true;
        }

        // General case: find and remove
        Node* prev = head;
        Node* current = head->getNext();

        while (current != &Node::NIL) {
            if (current->getValue() == value) {
                prev->setNext(current->getNext());
                if (current == tail) {
                    tail = prev;
                }
                delete current;
                count--;
                return true;
            }
            prev = current;
            current = current->getNext();
        }

        return false;
    }
};

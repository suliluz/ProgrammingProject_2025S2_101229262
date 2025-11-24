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
    SinglyLinkedList() : head(&Node::NIL), tail(&Node::NIL), count(0) {}

    ~SinglyLinkedList() {
        clear();
    }

    SinglyLinkedList(const SinglyLinkedList& other) : head(&Node::NIL), tail(&Node::NIL), count(0) {
        Node* current = other.head;
        while (current != &Node::NIL) {
            push(current->getValue());
            current = current->getNext();
        }
    }

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

    void pushFront(const T& value) {
        Node* newNode = new Node(value);
        newNode->setNext(head);
        head = newNode;

        if (tail == &Node::NIL) {
            tail = newNode;
        }
        count++;
    }

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

    const T& getFirst() const {
        if (isEmpty()) {
            throw runtime_error("List is empty");
        }
        return head->getValue();
    }

    const T& getLast() const {
        if (isEmpty()) {
            throw runtime_error("List is empty");
        }
        return tail->getValue();
    }

    [[nodiscard]]
    bool isEmpty() const {
        return head == &Node::NIL;
    }

    [[nodiscard]]
    int length() const {
        return count;
    }

    void clear() {
        while (!isEmpty()) {
            popFront();
        }
    }

    ForwardIterator<T> getIterator() {
        return ForwardIterator<T>(head);
    }

    ForwardIterator<T> getIterator() const {
        return ForwardIterator<T>(head);
    }

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

#pragma once
#include <iostream>
#include "DoublyLinkedNode.h"
#include "BidirectionalIterator.h"

using namespace std;

template <class T>
class List {
    typedef DoublyLinkedNode<T> Node;
    Node* head;
    Node* tail;

    int count;
public:
    ~List() {
        BidirectionalIterator<T> it(head);
        while (it != it.end()) {
            Node* toDelete = it.getCurrent();
            ++it;
            delete toDelete;
        }
    }

    List(): head(&Node::NIL), tail(&Node::NIL), count(0) {}

    [[nodiscard]]
    bool isEmpty() const {
        return head == &Node::NIL;
    }

    // Push, aka append to tail
    void push(const T value) {
        // Create a node
        Node* node = new Node(value);

        // If list is empty
        if (isEmpty()) {
            head = node;
            tail = node;
        }
        else {
            tail->append(node);
            tail = node;
        }

        ++count;
    }

    // Push front, append as head
    void pushFront(const T value) {
        Node* node = new Node(value);

        if (!isEmpty()) {
            head->prepend(node);
        }
        else {
            tail = node;
        }

        head = node;

        ++count;
    }

    T shift() {
        if (isEmpty()) {
            throw std::out_of_range("List is empty.");
        }

        Node* toDelete = head;
        T copyValue = toDelete->getValue();
        head = head->getNext();

        if (head != &Node::NIL) {
            head->setPrevious(&Node::NIL);
        }
        else {
            tail = &Node::NIL;
        }

        delete toDelete;
        --count;

        return copyValue;
    }

    T pop() {
        if (isEmpty()) {
            throw std::out_of_range("List is empty.");
        }

        Node* toDelete = tail;
        T copyValue = toDelete->getValue();
        tail = tail->getPrevious();

        if (tail == &Node::NIL) {
            head = &Node::NIL;
        }
        else {
            tail->setNext(&Node::NIL);
        }

        delete toDelete;
        --count;

        return copyValue;
    }

    void insertAt(const int index, const T value) {
        if (index < 0 || index >= count) {
            throw std::out_of_range("Index out of range.");
        }

        if (index == 0) {
            pushFront(value);
        }
        else if (index == count) {
            push(value);
        }
        else {
            // Create node
            Node* node = new Node(value);

            // Loop 
            int currIndex = 0;
            BidirectionalIterator<T> it(head);

            while (it != it.end()) {
                if (currIndex == index) {
                    // Get this node
                    Node* currentNode = it.getCurrent();

                    // Link this node prev to currentNode previous
                    node->setPrevious(currentNode->getPrevious());

                    // Link this node next to currentNode
                    node->setNext(currentNode);

                    // Link precursorNextNode to this new node
                    currentNode->getPrevious()->setNext(node);

                    // Link precursorNode next to this node 
                    currentNode->setPrevious(node);

                    ++count;
                    return;
                }

                ++currIndex;
                ++it;
            }
        }
    }

    void removeAt(int index) {
        if (index < 0 || index >= count) {
            throw std::out_of_range("Index out of range.");
        }

        int currIndex = 0;
        BidirectionalIterator<T> it(head);

        while (it != it.end()) {
            if (currIndex == index) {
                Node* toDelete = it.getCurrent();
                if (toDelete == head) {
                    head = head->getNext();
                    if (head != &Node::NIL) {
                        head->setPrevious(&Node::NIL);
                    }
                    else {
                        tail = &Node::NIL;
                    }
                }
                else if (toDelete == tail) {
                    tail = tail->getPrevious();
                    if (tail != &Node::NIL) {
                        tail->setNext(&Node::NIL);
                    }
                    else {
                        head = &Node::NIL;
                    }
                }

                delete toDelete;
                --count;
                break;
            }

            ++currIndex;
            ++it;
        }
    }

    Node* get(int index) {
        if (index < 0 || index >= count) {
            throw std::out_of_range("Index out of range.");
        }

        int currIndex = 0;
        BidirectionalIterator<T> it(head);

        while (it != it.end()) {
            if (currIndex == index) {
                return it.getCurrent();
            }

            ++currIndex;
            ++it;
        }

        return &Node::NIL;
    }


    T& operator[](const int index) {
        if (index >= count) {
            throw std::out_of_range("Index out of range.");
        }

        int currIndex = 0;
        BidirectionalIterator<T> it(head);

        while (it != it.end()) {
            if (currIndex == index) {
                return it.getCurrent()->getValue();
            }
            ++currIndex;
            ++it;
        }

        throw std::out_of_range("Index out of range.");
    }

    T& getFirst() {
        return head->getValue();
    }

    T& getLast() {
        return tail->getValue();
    }

    [[nodiscard]]
    int length() const {
        return count;
    }

    BidirectionalIterator<T> getIterator() {
        return BidirectionalIterator<T>(head);
    }

    BidirectionalIterator<T> getReverseIterator() {
        return BidirectionalIterator<T>(tail);
    }
};

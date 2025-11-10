#pragma once

template <class T>
class DoublyLinkedNode {
private:
    typedef DoublyLinkedNode<T> Node;

    T value;
    Node* next;
    Node* previous;
public:
    static Node NIL;

    ~DoublyLinkedNode() = default;
    DoublyLinkedNode(): next(&NIL), previous(&NIL) {}
    explicit DoublyLinkedNode(T value): value(value), next(&NIL), previous(&NIL) {}

    T& getValue() {
        return value;
    }
    void setValue(T fValue) {
        this->value = fValue;
    }

    Node* getNext() const {
        return next;
    }
    void setNext(Node* fNext) {
        this->next = fNext;
    }

    Node* getPrevious() const {
        return previous;
    }
    void setPrevious(Node* fPrevious) {
        this->previous = fPrevious;
    }

    void prepend(Node* newNode) {
        newNode->next = this;

        if (this->previous != &NIL) {
            newNode->previous = this->previous;
            this->previous->next = newNode;
        }

        this->previous = newNode;
    }

    void append(Node* newNode) {
        newNode->previous = this;

        if (this->next != &NIL) {
            newNode->next = this->next;
            this->next->previous = newNode;
        }

        this->next = newNode;
    }

    void remove() {
        if (this->previous != &NIL) {
            this->previous->next = this->next;
        }

        if (this->next != &NIL) {
            this->next->previous = this->previous;
        }

        this->next = &NIL;
        this->previous = &NIL;

        delete this;
    }
};

template <class T>
DoublyLinkedNode<T> DoublyLinkedNode<T>::NIL;
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
    DoublyLinkedNode(): next(NIL), previous(NIL) {}
    explicit DoublyLinkedNode(T value): value(value), next(NIL), previous(NIL) {}

    T& getValue() const {
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
};

template <class T>
DoublyLinkedNode<T> DoublyLinkedNode<T>::NIL;
#pragma once

template <class T>
class SinglyLinkedNode {
private:
    typedef SinglyLinkedNode<T> Node;

    T value;
    Node* next;
public:
    static Node NIL;

    ~SinglyLinkedNode() = default;
    SinglyLinkedNode(): value(T()), next(NIL) {}
    explicit SinglyLinkedNode(T value): value(value), next(NIL) {};

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
};

template <class T>
SinglyLinkedNode<T> SinglyLinkedNode<T>::NIL;
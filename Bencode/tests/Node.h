#ifndef NODE_H
#define NODE_H

template <typename T>
class Node {
public:
    T value;
    Node<T> *next;

    Node(const T& value): value(value), next(nullptr) {};

    Node(const T& value, Node<T>* next): value(value), next(next) {};
};

#endif // NODE_H

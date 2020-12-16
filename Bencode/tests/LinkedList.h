#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include "Node.h"
#include <stdexcept>

template <typename T>
class LinkedList {
    Node<T> *head;
    int len;

    Node<T>* getNode(int index) {
        Node<T> *n = head;
        for (int i = 0; i < index; i++, n = n->next);

        return n;
    };

public:

    // default constructor - creates an empty list
    LinkedList(): head(nullptr), len(0) {};

    // copy constructor
    LinkedList(const LinkedList<T>& other): len(other.len) {
        if (other.head == nullptr) {
            head = nullptr;
            return;
        }

        head = new Node<T>(other.head->value);
        
        Node<T> *n = head;
        Node<T> *next = other.head->next;
        while (next != nullptr) { 
            n->next = new Node<T>(next->value);
            n = n->next;
            next = next->next;
        }
    };

    // move constructor
    LinkedList(LinkedList<T>&& other): LinkedList() {
        swap(*this, other);
    };

    ~LinkedList() {
        for (Node<T> *n = head; n != nullptr;) {
            Node<T> *old = n;
            n = n->next;
            delete old;
        }
    };

    // assignment operator - copies or moves value on right side depending if
    // it's an l- or r-value
    LinkedList<T>& operator=(LinkedList<T> other) {
        swap(*this, other);
        return *this;
    };

    // adds a value to the end of the list
    void add(const T& value) {
        if (head == nullptr) {
            head = new Node<T>(value);
        } else {
            getNode(len - 1)->next = new Node<T>(value);
        }

        len++;
    };

    // adds a value at the specified index, shifting everything at index or
    // greater up by one index
    void add(int index, const T& value) {
        if (index < 0 || index > len) {
            throw std::out_of_range("index out of range");
        }

        if (index == len) {
            return add(value);
        }

        Node<T> *newN = new Node<T>(value);

        if (index == 0) {
            newN->next = head;
            head = newN;
        } else {
            Node<T> *previous = getNode(index - 1);
            newN->next = previous->next;
            previous->next = newN;
        }

        len++;
    };

    // retreives the value at an index
    T get(int index) {
        if (index < 0 || index >= len) {
            throw std::out_of_range("index out of range");
        }

        Node<T> *n = getNode(index);
        return n->value;
    };

    int length() {
        return len;
    }

    // removes the value at an index, shifting everything at index or greater
    // down by one index
    void remove(int index) {
        if (index < 0 || index >= len) {
            throw std::out_of_range("index out of range");
        }

        Node<T> *old;

        if (index == 0) {
            old = head;
            head = head->next;
        } else {
            Node<T> *previous = getNode(index - 1);
            old = previous->next;
            previous->next = old->next;
        }

        delete old;
        len--;
    };

    // sets the value at an index
    void set(int index, const T& value) {
        if (index < 0 || index >= len) {
            throw std::out_of_range("index out of range");
        }

        Node<T> *n = getNode(index);
        n->value = value;
    };

    friend void swap(LinkedList<T>& first, LinkedList<T>& second) {
        using std::swap; //enable ADL

        swap(first.head, second.head);
        swap(first.len, second.len);
    };

    // TODO: add iterator
};

#endif // LINKEDLIST_H

#include <fstream>
#include <iostream>

struct ObjectA {
    int data;

    ObjectA() {
        std::cout << "ObjectA constructor.\n";
    }

    ~ObjectA() {
        std::cout << "ObjectA destructor.\n";
    }
};

struct ObjectB {
    ObjectA* ptr;
    int data;
    int data2;

    ObjectB() {
        std::cout << "ObjectB constructor.\n";
    }

    ~ObjectB() {
        std::cout << "ObjectB destructor.\n";
    }
};

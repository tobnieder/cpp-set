//
//  main.cpp
//  Set
//
//  Created by Antonio Seprano on 10/08/14.
//  Modified by Tobias Niederbrunner 2023
//  Copyright (c) 2014 Antonio Seprano. All rights reserved.
//

#include "set.hpp"
#include <iostream>
#include <string>

/* struct NotComparable {
    int one;
    std::string two;
}; */

int main() {
    UnorderedSet<int> s1{ 1, 2, 3 }, s2{ 1, 2, 4 }, s3 = (s1 - s2) + (s2 - s1);
    auto n = (s1 + s2).count(1);
    std::cout << s1 << "\n";
    std::cout << s2 << "\n";
    std::cout << s3 << "\n";
    std::cout << (s1 + s2) << "\n";
    std::cout << "size: " << n << "\n";

    OrderedSet<int> os{ 6464, 1, 2, 3 };


    std::cout << os << "\n";

    UnorderedSet<UnorderedSet<int>> nested{
        { 7, 1, 2, 3 },
        { 1, 2, 4 }
    };

    std::cout << nested << "\n";

    auto n2 = nested.count({ 1, 2, 4 });
    std::cout << "size: " << n2 << std::endl;
    auto n3 = nested.count({ 1, 2, 3 });
    std::cout << "size: " << n3 << std::endl;


    /*     UnorderedSet<NotComparable> shouldError{};

    std::cout << shouldError; */

    return 0;
}

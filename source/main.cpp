#include <iostream>

#include "MyIterator.h"
#include "MyVector.h"

#include <vector>

int main() {
    MyVector<std::string> stringVector{"Hello my vector!\n"};

    std::cout << stringVector[0];
    std::cout << "size: " << stringVector.size() << "\n";
    std::cout << "data: " << stringVector.data() << "\n";
    std::cout << "max_size: " << stringVector.max_size() << "\n";
    std::cout << "capacity: " << stringVector.capacity() << "\n";
    std::cout << "empty: " << std::boolalpha << stringVector.empty() << "\n";
}
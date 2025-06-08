#include <iostream>


class SharedExample {
    public:
     SharedExample(int value) : value_(value) {
         std::cout << "[SharedExample] Created with value: " << value_ << "\n";
     }
     int getValue() const {
         return value_;
     }
     private:
        int value_;
};
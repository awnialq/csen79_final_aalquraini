/*
 * Sin-Yaw Wang <swang24@scu.edu
 */
#include <iostream>
#include <iomanip>
#include <string>
#include "record.h"

namespace csen79 {

// Attr1, Attr2, Attr3, Category
std::istream& operator>>(std::istream& is, Record& record) {
    char comma;
    for (int i = 0; i < Record::NATTRIBUTES; ++i) 
        is >> record.attributes[i] >> comma;

    std::string cat;
    is >> cat;
    record.category = cat == "Red" ? Record::RED : Record::BLUE;
    return is;
}

std::ostream& operator<<(std::ostream& os, const Record& record) {
    for (int i = 0; i < Record::NATTRIBUTES; ++i) 
        os << std::setw(3) << record.attributes[i] << std::setw(0) << ", ";
        
    os << (record.category == Record::RED ? "Red" : "Blue");
    return os;
}

}
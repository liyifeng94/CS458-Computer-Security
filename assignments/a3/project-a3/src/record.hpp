//
// Created by Ethan Li
//

#ifndef RECORD_HPP
#define RECORD_HPP

#include <vector>
#include <string>

#define NAME_FIELD      0
#define GENDER_FIELD    1
#define DOB_FIELD       2
#define TELE_FIELD      3
#define POSTAL_FIELD    4
#define DISEASE_FIELD   5
#define ROW_LENGTH      6

class Record
{
public:
    size_t index;
    std::vector<std::string> row;
    Record(const size_t& s, const std::vector<std::string>& r) : index(s), row(r) {}
};


#endif //RECORD_HPP

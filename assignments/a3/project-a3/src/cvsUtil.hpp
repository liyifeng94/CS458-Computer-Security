//
// Created by Ethan Li
//

#ifndef CVSUTIL_HPP
#define CVSUTIL_HPP

#include <vector>
#include <string>

#define CVS_HEADER      0

#define CVS_EXT         ".csv"

extern int parseCVSFile(const std::string& fileName, std::vector<std::vector<std::string>>& output);

extern int writeCVSFile(const std::string& fileName, const std::vector<std::vector<std::string>>& output);

#endif //CVSUTIL_HPP

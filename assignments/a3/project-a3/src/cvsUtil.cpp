//
// Created by Ethan Li
//

#include "cvsUtil.hpp"

#include <iostream>
#include <vector>
#include <string>
#include <fstream>

using namespace std;

int parseCVSFile(const string& fileName, vector<vector<string>>& output)
{
    string line;
    ifstream cvsFile (fileName);
    if(cvsFile.is_open())
    {
        while(getline(cvsFile,line))
        {
            string temp = "";
            vector<string> cvsLine;
            for (auto c : line)
            {
                if(c == ',')
                {
                    cvsLine.push_back(temp);
                    temp = "";
                }
                else
                {
                    temp += c;
                }
            }
            cvsLine.push_back(temp);
            output.push_back(cvsLine);
        }
        cvsFile.close();
    }
    else
    {
        cerr<<"Unable to open cvs file: "<<fileName<<endl;
        return -1;
    }
    return 0;
}

int writeCVSFile(const string& fileName, const vector<vector<string>>& output)
{
    ofstream cvsFile(fileName);
    if(cvsFile.is_open())
    {
        for(const auto& cvsLine : output)
        {
            string line = "";
            for(const auto& item : cvsLine)
            {
                line += item;
                line += ',';
            }
            line[line.size()-1] = '\n';

            cvsFile<<line;
        }
        cvsFile.close();
    }
    else
    {
        cerr<<"Unable to open cvs file: "<<fileName<<endl;
        return -1;
    }
    return 0;
}
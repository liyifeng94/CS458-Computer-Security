//
// Created by Ethan Li
//

#include <iostream>
#include <cstdlib>
#include <vector>
#include <string>
#include <unordered_map>

#include "cvsUtil.hpp"
#include "record.hpp"

using namespace std;

#define ANONYMIZED_FILE     "anonymized"
#define NON_KANONYMIZED     "non-kanonymized"
#define STAGE_1_POSTFIX     ".1"
#define STAGE_2_POSTFIX     ".2"
#define STAGE_3_POSTFIX     ".3"

vector<string> cvsHeader;

string mapKey(const vector<string>& row)
{
    string output = "";
    output += row[GENDER_FIELD];
    output += row[POSTAL_FIELD];
    output += row[DOB_FIELD];
    return output;
}



int nonAnonoRecords(const int& k, const vector<Record>& records,const vector<vector<string>>& origin)
{

    unordered_map<string,int> map;
    vector<vector<string>> cvsRecords;
    vector<int> recordIDs;

    //count sets
    for (const auto& r: records)
    {
        string key = mapKey(r.row);
        if(map.find(key) == map.end())
        {
            map[key] = 1;
        }
        else
        {
            map[key] += 1;
        }
    }

    //check for number of records
    for(auto& r : records)
    {
        string key = mapKey(r.row);
        if(map[key] < k)
        {
            recordIDs.push_back(r.index);
        }
    }

    cvsRecords.push_back(cvsHeader);

    for(auto& i : recordIDs)
    {
        cvsRecords.push_back(origin[i]);
    }

    string outputFile = NON_KANONYMIZED;
    outputFile += CVS_EXT;
    int err = writeCVSFile(outputFile,cvsRecords);
    return err;
}

int writeStage(const vector<Record>& records, const string postfix)
{
    string outputName = "";
    outputName += ANONYMIZED_FILE;
    outputName += CVS_EXT;
    outputName += postfix;
    vector<vector<string>> cvsVector;
    cvsVector.push_back(cvsHeader);
    for(const auto& r : records)
    {
        cvsVector.push_back(r.row);
    }
    int err = writeCVSFile(outputName,cvsVector);
    return err;
}

string anonPostal(const string& postal)
{
    string output = "";
    for(size_t i = 0; i < 3; ++i)
    {
        output += postal[i];
    }
    output += " *";
    return output;
}


void final(const int& k, vector<Record>& records)
{
    unordered_map<string,int> map;

    //count sets
    for (const auto& r: records)
    {
        string key = mapKey(r.row);
        if(map.find(key) == map.end())
        {
            map[key] = 1;
        }
        else
        {
            map[key] += 1;
        }
    }

    //check for number of records
    for(auto& r : records)
    {
        string key = mapKey(r.row);
        if(map[key] < k)
        {
            r.row[POSTAL_FIELD] = anonPostal(r.row[POSTAL_FIELD]);
        }
    }
}

string anonMonth(const string& date)
{
    string output = "*-*";
    size_t i = 3;
    if(date[3] != '-')
    {
        i = 4;
    }
    for(; i < date.size(); ++i)
    {
        output += date[i];
    }
    return output;
}

void stage3(const int& k, vector<Record>& records)
{
    unordered_map<string,int> map;

    //count sets
    for (const auto& r: records)
    {
        string key = mapKey(r.row);
        if(map.find(key) == map.end())
        {
            map[key] = 1;
        }
        else
        {
            map[key] += 1;
        }
    }

    //check for number of records
    for(auto& r : records)
    {
        string key = mapKey(r.row);
        if(map[key] < k)
        {
            r.row[DOB_FIELD] = anonMonth(r.row[DOB_FIELD]);
        }
    }
}

string anonDay(const string& date)
{
    string output = "*";
    size_t i = 1;
    if(date[1] != '-')
    {
        i = 2;
    }
    for(; i < date.size(); ++i)
    {
        output += date[i];
    }
    return output;
}

void stage2(const int& k, vector<Record>& records)
{
    unordered_map<string,int> map;

    //count sets
    for (const auto& r: records)
    {
        string key = mapKey(r.row);
        if(map.find(key) == map.end())
        {
            map[key] = 1;
        }
        else
        {
            map[key] += 1;
        }
    }

    //check for number of records
    for(auto& r : records)
    {
        string key = mapKey(r.row);
        if(map[key] < k)
        {
            r.row[DOB_FIELD] = anonDay(r.row[DOB_FIELD]);
        }
    }
}

int process(const int& k, const vector<vector<string>>& origin)
{
    vector<Record> records;
    size_t index = 0;

    //stage 1
    for(const auto& r : origin)
    {
        records.push_back(Record(index,r));
        records.back().row[NAME_FIELD] = "*";
        records.back().row[TELE_FIELD] = "*";
        ++index;
    }
    //remove header field
    records.erase(records.begin()+CVS_HEADER);

    writeStage(records,STAGE_1_POSTFIX);

    stage2(k,records);

    writeStage(records,STAGE_2_POSTFIX);

    stage3(k,records);

    writeStage(records,STAGE_3_POSTFIX);

    final(k,records);

    writeStage(records,"");

    nonAnonoRecords(k,records,origin);


    return 0;
}

int main(int argc, char* argv[])
{
    if(argc != 3)
    {
        cerr<<"ERROR: Invalid number of args."<<endl;
        exit(1);
    }

    string kString = argv[1];
    string fileName = argv[2];
    int k = stoi(kString);
    int err = 0;
    vector<vector<string>> cvsParsed;

    err = parseCVSFile(fileName,cvsParsed);
    if(err < 0)
    {
        return err;
    }

    cvsHeader = cvsParsed[CVS_HEADER];

    process(k,cvsParsed);

    /*
    string outputFileName = "";
    outputFileName += ANONYMIZED_FILE;
    outputFileName += CVS_EXT;
    err = writeCVSFile(outputFileName,cvsParsed);
    if(err < 0)
    {
        return err;
    }
     */



    return 0;
}
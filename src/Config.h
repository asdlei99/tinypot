#pragma once

#include "INIReader.h"
#include <stdint.h>
#include <vector>

class Config
{
    struct Record
    {
        std::string filename;
        int second;
        int64_t time;
    };

private:
    std::string content_;
    std::string filename_;
    Config();
    virtual ~Config();

    INIReaderNormal ini_;

    std::vector<std::string> ignore_strs_;

public:
    void init(std::string filepath);

    void write();
    static Config* getInstance()
    {
        static Config c;
        return &c;
    }

    //xmlֻ���ִ�������������ִ�����
    std::string getString(const std::string& name, std::string def = "");
    int getInteger(const std::string& name, int def = 0);

    void setString(const std::string& name, const std::string v);
    void setInteger(const std::string& name, int v);

    //��¼
    int getRecord(const std::string& name);
    std::string getNewestRecord();
    void removeRecord(const std::string& name);
    void setRecord(const std::string& name, int v);
    void clearAllRecord();
    void autoClearRecord();

    std::string dealFilename(const std::string& s0);

    std::string enStr(const std::string& in);
    std::string deStr(std::string out);
    std::vector<Record> getSortedRecord();

    std::string findSuitableFilename(const std::string& filename);
};

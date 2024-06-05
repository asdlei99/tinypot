#pragma once
#include "PotSubtitle.h"
#include <cstdio>
#include <string>
#include <vector>

struct PotSubtitleAtom
{
    int begintime;
    int endtime;
    std::string str;
};

class PotSubtitleSrt : public PotSubtitle
{
private:
    std::vector<PotSubtitleAtom> atom_list_;
    std::string content_;
    int readIndex();

    int fram_w_, frame_h_;

public:
    PotSubtitleSrt() = default;
    virtual ~PotSubtitleSrt() = default;

    //virtual void init();
    //virtual void destroy();
    virtual bool openSubtitle(const std::string& filename);
    virtual void closeSubtitle();
    virtual int show(int time);
    virtual void setFrameSize(int w, int h)
    {
        fram_w_ = w;
        frame_h_ = h;
    }

    std::string code_ = "utf-8";

    bool isUTF8(const void* pBuffer, long size);

    //����Ƚϼ򵥣�init��destroy����Ϊ��

    /*bool openSubtitle(const string& filename);
    void show(int time);
    bool exist();
    void closeSubtitle();
    void setFrameSize(int w, int h);
    void tryOpenSubtitle(string open_filename);
    void init();
    void destroy();*/
};

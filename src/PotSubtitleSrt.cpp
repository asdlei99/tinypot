#include "PotSubtitleSrt.h"
#include "filefunc.h"
#include "Font.h"
#include "PotConv.h"
#include "strfunc.h"

int PotSubtitleSrt::readIndex()
{
    auto lines = strfunc::splitString(content_, "\n");

    int lineno = 0;
    int state = -1;    //0: line number, 1: time, 2: content, -1: blank line
    PotSubtitleAtom pot;
    for (auto& line : lines)
    {
        lineno++;
        if (lineno == 1)
        {
            if (line.size() >= 3 && (unsigned char)line[0] == 0xEF && (unsigned char)line[1] == 0xBB && (unsigned char)line[2] == 0xBF)
            {
                code_ = "utf-8";
                line = line.substr(3);
            }
            else
            {
                if (!isUTF8(content_.data(), content_.size()))
                {
                    code_ = "cp936";
                }
            }
        }
        if (lines.empty() || line.find_first_not_of(" \t\r") == std::string::npos)
        {
            pot.str = PotConv::conv(strfunc::replaceAllSubString(pot.str, "\r", ""), code_, "cp936");
            atom_list_.push_back(pot);
            pot.str.clear();
            state = -1;
            continue;
        }
        else if (state == -1)
        {
            auto ints = strfunc::findNumbers<int>(line);
            if (ints.size() == 1)
            {
                state = 0;
            }
        }
        else if (state == 0)
        {
            auto ints = strfunc::findNumbers<int>(line);
            if (ints.size() >= 8 && lineno <= lines.size() - 1)
            {
                int btimeh = ints[0], btimem = ints[1], btimes = ints[2], btimems = ints[3];
                int etimeh = ints[4], etimem = ints[5], etimes = ints[6], etimems = ints[7];
                int totalbegintime = btimems + 1000 * btimes + 1000 * 60 * btimem + 1000 * 3600 * btimeh;
                int totalendtime = etimems + 1000 * etimes + 1000 * 60 * etimem + 1000 * 3600 * etimeh;
                pot.begintime = totalbegintime;
                pot.endtime = totalendtime;
            }
            state = 1;
        }
        else if (state == 1 || state == 2)
        {
            pot.str += line;
            state = 2;
        }
    }
    return 0;
}

bool PotSubtitleSrt::openSubtitle(const std::string& filename)
{
    exist_ = true;
    content_ = filefunc::readFileToString(filename);
    if (content_.empty())
    {
        return false;
    }
    else
    {
        readIndex();
        return true;
    }
}

bool PotSubtitleSrt::isUTF8(const void* pBuffer, long size)
{
    bool IsUTF8 = true;
    unsigned char* start = (unsigned char*)pBuffer;
    unsigned char* end = (unsigned char*)pBuffer + size;
    while (start < end)
    {
        if (*start < 0x80)    // (10000000): ֵС��0x80��ΪASCII�ַ�
        {
            start++;
        }
        else if (*start < (0xC0))    // (11000000): ֵ����0x80��0xC0֮���Ϊ��ЧUTF-8�ַ�
        {
            IsUTF8 = false;
            break;
        }
        else if (*start < (0xE0))    // (11100000): �˷�Χ��Ϊ2�ֽ�UTF-8�ַ�
        {
            if (start >= end - 1)
            {
                break;
            }
            if ((start[1] & (0xC0)) != 0x80)
            {
                IsUTF8 = false;
                break;
            }
            start += 2;
        }
        else if (*start < (0xF0))    // (11110000): �˷�Χ��Ϊ3�ֽ�UTF-8�ַ�
        {
            if (start >= end - 2)
            {
                break;
            }
            if ((start[1] & (0xC0)) != 0x80 || (start[2] & (0xC0)) != 0x80)
            {
                IsUTF8 = false;
                break;
            }
            start += 3;
        }
        else
        {
            IsUTF8 = false;
            break;
        }
    }
    return IsUTF8;
}

int PotSubtitleSrt::show(int time)
{
    for (int i = 0; i < atom_list_.size(); i++)
    {
        auto tmplist = atom_list_[i];
        if (tmplist.begintime <= time && tmplist.endtime >= time)
        {
            //engine_->renderCopy()
            int w, h;
            engine_->getWindowSize(w, h);
            Font::getInstance()->drawSubtitle(subfilename_, tmplist.str, w * 16 / 720, w / 2, h * 9 / 10 - 5, 255, 1);
            break;
        }
    }
    return 1;
}

void PotSubtitleSrt::closeSubtitle()
{
    //throw std::logic_error("The method or operation is not implemented.");
}

/*void BigPotSubtitleSrt::tryOpenSubtitle(string open_filename)
{
    openSubtitle(open_filename);
    //throw std::logic_error("The method or operation is not implemented.");
}*/
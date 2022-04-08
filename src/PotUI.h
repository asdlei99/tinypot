#pragma once
#include "PotBase.h"

class PotUI : public PotBase
{
public:
    PotUI();
    virtual ~PotUI();

    enum
    {
        ButtonNone = 0,
        ButtonPause,
        ButtonNext,
        ButtonFullScreen,
        ButtonSubtitle,
        ButtonLeft,
        ButtonRight,
        ButtonVolume,
        ButtonNone2,
    };

private:
    BP_Texture *square_ = nullptr, *ball_ = nullptr, *triangle_ = nullptr, *triangle2_ = nullptr, *hollow_ = nullptr;
    BP_Texture *to_full_screen_ = nullptr, *to_window_ = nullptr;
    BP_Texture *frame_ = nullptr;
    int win_w_, win_h_;

    //int x = 0, y = 0, w = 0, h = 0;
    uint8_t alpha_ = 128, alpha_count_ = 192;
    std::string fontname_;
    std::string text_;
    int text_count_ = 32;

    int button_x_ = 10, button_y_, button_w_ = 20, button_h_ = 20;

public:
    void init();
    void drawText(const std::string& text);
    void drawUI(int time, int totoal_time, int volume, bool pause);
    void destory();
    std::string convertTimeToString(int time);
    void setText(std::string t);
    double inProcess();
    int inButton();
};

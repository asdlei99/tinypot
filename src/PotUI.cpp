#include "PotUI.h"
#include "Config.h"
#include "File.h"
#include "Font.h"
#include "convert.h"
#include "math.h"

PotUI::PotUI()
{
}

PotUI::~PotUI()
{
}

void PotUI::drawText(const std::string& text)
{
    Font::getInstance()->drawText(fontname_.c_str(), text, 22, win_w_ - 20 - button_w_, win_h_ - 48, alpha_, BP_ALIGN_RIGHT);
    //engine_->drawText(_fontname.c_str(), std::to_string(_volume / 128.0)+"%", 20, _win_w - 10, 35, _alpha, BP_ALIGN_RIGHT);
}

void PotUI::drawUI(int time, int totoal_time, int volume, bool pause)
{
    engine_->getWindowSize(win_w_, win_h_);
    int in_button = inButton();
    double process = inProcess();
    if (text_count_ > 0)
    {
        text_count_--;
    }
    if (text_count_ == 0)
    {
        text_ = "";
    }
    if (in_button >= 0 || process > 0)
    {
        alpha_count_ = 192;
    }
    else
    {
        if (text_count_ == 0 && alpha_count_ > 0)
        {
            alpha_count_ -= 4;
        }
    }
    if (alpha_count_ == 0)
    {
        text_ = "";
        return;
    }

    alpha_ = (std::min)(uint8_t(128), alpha_count_);
    //������
    int x, y;
    y = win_h_ - 12;
    engine_->setColor(square_, { 255, 255, 255 }, alpha_ / 2);
    engine_->renderCopy(square_, 0, y - 1, win_w_, 4);
    engine_->setColor(square_, { 255, 0, 0 }, alpha_);
    engine_->renderCopy(square_, 0, y - 1, 1.0 * time / totoal_time * win_w_, 4);

    //��ť
    engine_->setColor(square_, { 255, 255, 255 }, alpha_);
    engine_->setColor(triangle_, { 255, 255, 255 }, alpha_);
    engine_->setColor(to_full_screen_, { 255, 255, 255 }, alpha_);
    engine_->setColor(to_window_, { 255, 255, 255 }, alpha_);
    engine_->setColor(hollow_, { 255, 255, 255 }, alpha_);
    button_y_ = win_h_ - 45;

    for (int i = 1; i < ButtonNone2; i++)
    {
        int button_x = button_x_ + (i - 1) * (button_w_ + 10);
        int button_y = button_y_;
        switch (i)
        {
        case ButtonPause:
            if (pause)
            {
                engine_->renderCopy(triangle_, button_x, button_y, button_w_, button_h_);
            }
            else
            {
                engine_->renderCopy(square_, button_x, button_y, 8, button_h_);
                engine_->renderCopy(square_, button_x + 12, button_y, 8, button_h_);
            }
            break;
        case ButtonNext:
            engine_->renderCopy(triangle_, button_x, button_y, button_w_ / 2, button_h_);
            engine_->renderCopy(square_, button_x + 14, button_y, 6, button_h_);
            break;
        case ButtonFullScreen:
            if (engine_->isFullScreen())
            {
                engine_->renderCopy(to_window_, button_x, button_y, button_w_, button_h_);
            }
            else
            {
                engine_->renderCopy(to_full_screen_, button_x, button_y, button_w_, button_h_);
            }
            break;
        case ButtonSubtitle:
            engine_->renderCopy(hollow_, button_x, button_y, button_w_, button_h_);
            engine_->renderCopy(square_, button_x + button_w_* 0.2, button_y + button_h_ * 0.6, button_w_ * 0.6, button_h_ * 0.2);
            break;
        }
    }

    int button_x = win_w_ - 10 - button_w_;
    int one_square = BP_AUDIO_MIX_MAXVOLUME / 8;
    int v = volume;
    for (int i_v = 0; i_v < 8; i_v++)
    {
        int h = (i_v + 1) * 2;
        v -= one_square;
        double r = 1;
        if (v < 0)
        {
            r = 1.0 * (one_square + v) / one_square;
        }
        int hc = r * h;
        engine_->renderCopy(square_, button_x + i_v * 3, button_y_ + button_h_ - hc, 2, hc);
        if (v < 0)
        {
            break;
        }
    }

    //����
    if (text_ == "v")
    {
        text_ = convert::formatString("Volume %5.1f", 100.0 * volume / BP_AUDIO_MIX_MAXVOLUME);
        drawText(text_);
    }
    else if (in_button <= 0)
    {
        if (text_ == "")
        {
            drawText(convertTimeToString(time) + "/" + convertTimeToString(totoal_time));
        }
        else
        {
            drawText(text_);
        }
    }
    else
    {
        std::string text;
        if (text_.empty())
        {
            switch (in_button)
            {
            case ButtonPause:
                if (pause)
                {
                    text = "Play";
                }
                else
                {
                    text = "Pause";
                }
                break;
            case ButtonNext:
                text = "Next";
                break;
            case ButtonFullScreen:
                if (engine_->isFullScreen())
                {
                    text = "Window";
                }
                else
                {
                    text = "Full Screen";
                }
                break;
            case ButtonSubtitle:
                text = "Switch subtitles";
                break;
                //case ButtonVolume:
                //    text = convert::formatString("Volume %5.1f", 100.0 * volume / BP_AUDIO_MIX_MAXVOLUME);
                //    break;
            }
        }
        else
        {
            text = text_;
        }
        drawText(text);
    }
}

std::string PotUI::convertTimeToString(int time)
{
    char s[256];
    sprintf(s, "%d:%02d:%02d", time / 3600000, time % 3600000 / 60000, time % 60000 / 1000);
    return s;
}

void PotUI::setText(std::string t)
{
    //һ��ʱ��������Զ����ʱ��
    text_count_ = 32;
    alpha_count_ = 192;
    text_ = t;
}

double PotUI::inProcess()
{
    int x, y;
    engine_->getMouseState(x, y);
    if (y > win_h_ - 24)
    {
        double p = 1.0 * x / win_w_;
        if (p >= 0 && p <= 1)
        {
            return p;
        }
    }
    return -1;
}

int PotUI::inButton()
{
    int x, y;
    engine_->getMouseState(x, y);
    if (y >= button_y_ && y <= button_y_ + button_h_)
    {
        int button_x = button_x_;
        for (int i = 1; i < ButtonNone2; i++)
        {
            if (x >= button_x && x <= button_x + button_w_)
            {
                return i;
            }
            button_x += button_w_ + 10;
        }
        return 0;
    }
    return -1;
}

void PotUI::init()
{
    square_ = engine_->createSquareTexture(40);
    ball_ = engine_->createSpecialTexture(50);
    triangle_ = engine_->createSpecialTexture(200, 1);
    to_full_screen_ = engine_->createSpecialTexture(20, 4);
    to_window_ = engine_->createSpecialTexture(20, 5);
    hollow_ = engine_->createSpecialTexture(20, 6);
    fontname_ = Config::getInstance()->getString("ui_font");
    if (!File::fileExist(fontname_))
    {
#ifdef _WIN32
        fontname_ = "C:/Windows/Fonts/Cambria.ttc";
        if (!File::fileExist(fontname_))
        {
            fontname_ = "C:/Windows/Fonts/Cambria.ttf";
        }
#else
        fontname_ = "/System/Library/Fonts/Palatino.ttc";
#endif
    }
}

void PotUI::destory()
{
    if (Config::getInstance()->getString("ui_font") == "")
    {
        Config::getInstance()->setString("ui_font", fontname_);
    }
}

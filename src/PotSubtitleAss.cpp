#include "PotSubtitleAss.h"
#include "PotConv.h"
#include "strfunc.h"
#include "others/text_encoding_detect.h"

PotSubtitleAss::PotSubtitleAss()
{
    init();
}

PotSubtitleAss::~PotSubtitleAss()
{
    destroyAllTex();
    closeSubtitle();
    destroy();
}

void PotSubtitleAss::init()
{
    library_ = ass_library_init();
    renderer_ = ass_renderer_init(library_);
    ass_set_fonts(renderer_, fontname_.c_str(), "Sans", 1, "", 0);
}

bool PotSubtitleAss::openSubtitle(const std::string& filename)
{
    //�����Ĳ�����char*,Ϊ�����⸴��һ��
    auto s = strfunc::readStringFromFile(filename);

    TextEncodingDetect textDetect;
    TextEncodingDetect::Encoding encoding = textDetect.DetectEncoding((const uint8_t*)s.c_str(), s.size());
    std::string encode_str = "utf-8";

    if (encoding == TextEncodingDetect::None)
    {
        encode_str = "utf-8";
    }
    else if (encoding == TextEncodingDetect::ASCII)
    {
        encode_str = "utf-8";
    }
    else if (encoding == TextEncodingDetect::ANSI)
    {
        encode_str = "cp936";
    }
    else if (encoding == TextEncodingDetect::UTF8_BOM || encoding == TextEncodingDetect::UTF8_NOBOM)
    {
        encode_str = "utf-8";
    }
    else if (encoding == TextEncodingDetect::UTF16_LE_BOM || encoding == TextEncodingDetect::UTF16_LE_NOBOM)
    {
        encode_str = "utf-16le";
    }
    else if (encoding == TextEncodingDetect::UTF16_BE_BOM || encoding == TextEncodingDetect::UTF16_BE_NOBOM)
    {
        encode_str = "utf-16be";
    }
    auto s1 = PotConv::conv(s, encode_str.c_str(), "utf-8");
    //if (checkFileExt(filename))
    track_ = ass_read_memory(library_, (char*)s1.c_str(), s1.size(), NULL);
    exist_ = (track_ != nullptr);
    if (exist_)
    {
        subfilename_ = filename;
    }
    return exist_;
}

int PotSubtitleAss::show(int time)
{
    int a = 0;
    image_ = ass_render_frame(renderer_, track_, time, &a);
    //cout << engine_->getTicks() << endl;
    auto img = image_;
    if (a)
    {
        destroyAllTex();
        while (img)
        {
            auto t = engine_->transBitmapToTexture(img->bitmap, img->color, img->w, img->h, img->stride);
            engine_->renderCopy(t, img->dst_x, img->dst_y, img->w, img->h, 1);
            tex_vector_.push_back(t);
            img = img->next;
        }
    }
    else
    {
        int i = 0;
        while (img && i < tex_vector_.size())
        {
            engine_->renderCopy(tex_vector_[i++], img->dst_x, img->dst_y, img->w, img->h, 1);
            img = img->next;
        }
    }
    return a;
    //cout << engine_->getTicks() << endl;
}

void PotSubtitleAss::destroy()
{
    ass_renderer_done(renderer_);
    ass_library_done(library_);
}

void PotSubtitleAss::setFrameSize(int w, int h)
{
    //if (_track)
    ass_set_frame_size(renderer_, w, h);
}

void PotSubtitleAss::openSubtitleFromMem(const std::string& str)
{
    track_ = ass_read_memory(library_, (char*)str.c_str(), str.size(), NULL);
    exist_ = (track_ != nullptr);
}

void PotSubtitleAss::readOne(const std::string& str, int start_time, int end_time)
{
    if (contents_.count(str) == 0)
    {
        contents_.insert(str);
        ass_process_data(track_, (char*)str.c_str(), str.size());
#ifdef _DEBUG
        fmt1::print("{}\n", PotConv::conv(str, "utf-8", "cp936"));
#endif
    }
}

void PotSubtitleAss::clear()
{
    ass_flush_events(track_);
}

void PotSubtitleAss::closeSubtitle()
{
    if (track_)
    {
        ass_free_track(track_);
    }
    track_ = nullptr;
}

void PotSubtitleAss::destroyAllTex()
{
    for (auto t : tex_vector_)
    {
        engine_->destroyTexture(t);
    }
    tex_vector_.clear();
}

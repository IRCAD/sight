#ifndef __VIDEOORBBEC_COLORFRAMEWORKER_HPP__
#define __VIDEOORBBEC_COLORFRAMEWORKER_HPP__

#include "videoOrbbec/config.hpp"
#include <arData/FrameTL.hpp>

namespace arServices
{
class IGrabber;
} // namespace arServices

#include <QObject>
#include <QVideoFrame>

namespace videoOrbbec
{

class ColorFrameWorker : public QObject
{
Q_OBJECT;
public:
    ColorFrameWorker(::arServices::IGrabber const& parent);
    ~ColorFrameWorker();

    void init(::arData::FrameTL::sptr colorTL);
    void setPause(bool pause);

public Q_SLOTS:
    void presentFrame(QVideoFrame const& frame);

private:
    ::arServices::IGrabber const& m_parent;
    ::arData::FrameTL::sptr m_colorTL;
    bool m_pause;
};

} // namespace videoOrbbec

#endif // __VIDEOORBBEC_COLORFRAMEWORKER_HPP__

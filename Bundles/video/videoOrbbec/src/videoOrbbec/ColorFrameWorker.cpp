#include "videoOrbbec/ColorFrameWorker.hpp"

#include <arServices/IGrabber.hpp>

#include <thread>

namespace videoOrbbec
{

//------------------------------------------------------------------------------

ColorFrameWorker::ColorFrameWorker(::arServices::IGrabber const& parent) :
    QObject(),
    m_parent(parent),
    m_colorTL(nullptr),
    m_pause(false)
{
}

//------------------------------------------------------------------------------

ColorFrameWorker::~ColorFrameWorker()
{
}

//------------------------------------------------------------------------------

void ColorFrameWorker::init(::arData::FrameTL::sptr colorTL)
{
    m_colorTL = colorTL;
}

//------------------------------------------------------------------------------

void ColorFrameWorker::setPause(bool pause)
{
    m_pause = pause;
}

//------------------------------------------------------------------------------

void ColorFrameWorker::presentFrame(QVideoFrame const& frame)
{
    if(m_pause)
    {
        return;
    }
    if(frame.pixelFormat() == QVideoFrame::Format_Invalid)
    {
        SLM_ERROR("Dropping invalid frame");
    }

    // Prepare the destination buffer in the timeline
    const auto frameWidth   = frame.width(),
               frameHeight  = frame.height(),
               frameBufSize = frameWidth * frameHeight;
    FW_RAISE_IF("Timeline buffer has wrong size",
                m_colorTL->getWidth() != frameWidth || m_colorTL->getHeight() != frameHeight);
    const auto timestamp = ::fwCore::HiResClock::getTimeInMilliSec();
    auto buffer          = m_colorTL->createBuffer(timestamp);
    auto dstBuffer       = reinterpret_cast<uint32_t*>(buffer->addElement(0));

    // The input feed is mirrored (relative to the depth stream) by default so we need to correct that
    // Map the video frame for reading
    const auto format  = frame.pixelFormat();
    auto& mutableFrame = const_cast<QVideoFrame&>(frame);
    mutableFrame.map(QAbstractVideoBuffer::ReadOnly);
    const QImage mirroredFrame = QImage(mutableFrame.bits(), frameWidth, frameHeight,
                                        QVideoFrame::imageFormatFromPixelFormat(frame.pixelFormat()))
#ifdef WIN32
                                 .mirrored(false, true);
#else
                                 .mirrored(false, false);
#endif
    mutableFrame.unmap();
    auto frameBuffer = reinterpret_cast<const uint32_t*>(mirroredFrame.bits());

    // Copy the frame to the buffer
    for(auto idx = 0; idx < frameBufSize; ++idx, ++dstBuffer, ++frameBuffer)
    {
        const auto pixel = *frameBuffer;

        *dstBuffer = 0xFF000000 | (pixel & 0x0000FF00) |
                     (pixel & 0x000000FF) << 16 | (pixel & 0x00FF0000) >> 16;
    }

    // Push the frame to the timeline and clean up
    m_colorTL->pushObject(buffer);
    m_colorTL->signal< ::arData::TimeLine::ObjectPushedSignalType>(::arData::TimeLine::s_OBJECT_PUSHED_SIG)
    ->asyncEmit(timestamp);
    m_parent.signal< ::arServices::IGrabber::FramePresentedSignalType>(::arServices::IGrabber::s_FRAME_PRESENTED_SIG)
    ->asyncEmit();
}

} // namespace videoOrbbec


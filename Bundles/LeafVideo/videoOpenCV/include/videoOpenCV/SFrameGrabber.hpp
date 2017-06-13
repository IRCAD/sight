/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VIDEOOPENCV_SFRAMEGRABBER_HPP__
#define __VIDEOOPENCV_SFRAMEGRABBER_HPP__

#include "videoOpenCV/config.hpp"

#include <arServices/IGrabber.hpp>

#include <fwCom/Slot.hpp>
#include <fwCom/Slots.hpp>

#include <fwCore/mt/types.hpp>

#include <fwThread/Timer.hpp>

#include <fwTools/Failed.hpp>

#include <boost/filesystem/path.hpp>

#include <opencv2/videoio.hpp>

namespace arData
{
class Camera;
}

namespace videoOpenCV
{

/**
 * @brief   Defines the service which grab video frame.
 *
 * @note Only file source is currently managed.
 * @note You can load images in a folder like img_<timestamp>.<ext> (ex. img_642752427.jgp). The service uses
 * the timestamp to order the frames and to push them in the timeline.
 *
 * @section Signals Signals
 * - \b positionModified(std::int64_t) : Emitted when the position in the video is modified during playing.
 * - \b durationModified(std::int64_t) : Emitted when the duration of the video is modified.
 *
 * @section Slots Slots
 * - \b startCamera() : Start playing the camera or the video.
 * - \b stopCamera() : Stop playing the camera or the video.
 * - \b pauseCamera() : Pause the video, it has no effect when playing a camera.
 * - \b loopVideo() : Toggle the loop of the playing.
 * - \b setPositionVideo(int) : Force the current time in the video.
 * - \b nextImage() : Read the next image (in frame-by-frame mode).
 * - \b previousImage() : Read the previous image (in frame-by-frame mode).
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
        <service type="::videoOpenCV::SFrameGrabber">
            <in key="camera" uid="..." />
            <inout key="frameTL" uid="..." />
            <fps>30</fps>
            <oneShot>false</oneShot>
            <createTimestamp>false</createTimestamp>
            <useTimelapse>true</useTimelapse>
        </service>
   @endcode
 * @subsection Input Input
 * - \b camera [::arData::Camera]: camera used to display video.
 * @subsection In-Out In-Out
 * - \b frameTL [::arData::FrameTL]: timeline where to extract the video frames.
 * @subsection Configuration Configuration
 * - \b fps (optional) : target playback frame rate when playing an image sequence (default: 30).
 * - \b useTimelapse (optional): if true, the difference between two image's timestamps will be use as timer duration,
 *      the 'fps' value will be ignore.
 * - \b oneShot (optional) : Use frame by frame mode, using nextImage and previousImage
 *  (only available if reading set of images) (default: false).
 * - \b createTimestamp (optional) : create a new timestamp instead of using name of image
 * (only available if reading set of images) (default: false).
 */
class VIDEOOPENCV_CLASS_API SFrameGrabber : public ::arServices::IGrabber
{

public:

    fwCoreServiceClassDefinitionsMacro( (SFrameGrabber)(::arServices::IGrabber) );

    /// Constructor. Do nothing.
    VIDEOOPENCV_API SFrameGrabber() throw();

    /// Destructor. Do nothing.
    VIDEOOPENCV_API virtual ~SFrameGrabber() throw();

    /**
     * @name Slots API
     * @{
     */
    VIDEOOPENCV_API static const ::fwCom::Slots::SlotKeyType s_NEXT_IMAGE_SLOT;
    VIDEOOPENCV_API static const ::fwCom::Slots::SlotKeyType s_PREVIOUS_IMAGE_SLOT;
    ///@}

protected:

    /// Initialize the layout and the camera.
    VIDEOOPENCV_API virtual void starting() throw( ::fwTools::Failed );

    /// Destroy the layout.
    VIDEOOPENCV_API virtual void stopping() throw( ::fwTools::Failed );

    /// Do nothing.
    VIDEOOPENCV_API virtual void updating() throw(::fwTools::Failed);

    /// Do nothing.
    VIDEOOPENCV_API virtual void configuring() throw( ::fwTools::Failed );

    /// SLOT : Initialize and start camera (restart camera if is already started)
    virtual void startCamera();

    /// SLOT : Stop camera
    virtual void stopCamera();

    /// SLOT : Pause camera
    virtual void pauseCamera();

    /// SLOT : enable/disable loop in video
    virtual void toggleLoopMode();

    /// SLOT : set the new position in the video.
    virtual void setPosition(int64_t position);

    /// SLOT : read the next image (only in file mode, and if m_oneShot is enabled)
    void nextImage();

    /// SLOT : read the previous image (only in file mode, and if m_oneShot is enabled)
    void previousImage();

private:

    typedef std::vector< ::boost::filesystem::path > ImageFilesType;
    typedef std::vector< double > ImageTimestampsType;

    /// Initializes the video reader, start the timer
    void readVideo(const ::boost::filesystem::path& file);

    /// Initializes the image reader, start the timer
    void readImages(const ::boost::filesystem::path& folder, const std::string& extension);

    /// Reads the next video frame
    void grabVideo();

    /// Reads the next image
    void grabImage();

    /// state of the loop mode
    bool m_loopVideo;

    /// state of the timeline initialization
    bool m_isInitialized;

    /// fps used to read the video
    unsigned int m_fps;

    /// counter used by the image reader
    size_t m_imageCount;

    ::fwThread::Timer::sptr m_timer;

    /// Worker for the grabVideo or grabFrame timer
    ::fwThread::Worker::sptr m_worker;

    /// openCV video grabber
    ::cv::VideoCapture m_videoCapture;

    /// list of image paths to read
    ImageFilesType m_imageToRead;

    /// list of the image timestamps
    ImageTimestampsType m_imageTimestamps;

    /// Mutex to protect concurrent access for m_videoCapture and m_imageToRead
    mutable ::fwCore::mt::Mutex m_mutex;

    /// frame -by-frame mode (true if enabled, false otherwise)
    bool m_oneShot;

    /// if true: create a new timestamp when reading image, if false: use the name of the image file as timestamp.
    bool m_createNewTS;

    /// if true: the difference between two image's timestamps will be use as timer duration
    bool m_useTimelapse;
};

} // namespace videoOpenCV

#endif /*__VIDEOOPENCV_SFRAMEGRABBER_HPP__*/

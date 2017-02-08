/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __ARDATA_CAMERA_HPP__
#define __ARDATA_CAMERA_HPP__

#include "arData/config.hpp"

#include <boost/cstdint.hpp>
#include <boost/filesystem/path.hpp>

#include <fwCom/Signal.hpp>
#include <fwCom/Signals.hpp>

#include <fwData/Object.hpp>
#include <fwData/factory/new.hpp>

#include <array>
#include <utility>

fwCampAutoDeclareDataMacro((arData)(Camera), ARDATA_API);

namespace arData
{
/**
 * @brief   This class defines a camera object.
 */
class ARDATA_CLASS_API Camera : public ::fwData::Object
{
public:
    fwCampMakeFriendDataMacro((arData)(Camera));
    fwCoreClassDefinitionsWithFactoryMacro((Camera)(::fwData::Object), (()), ::fwData::factory::New< Camera >);

    typedef std::array<double, 5> DistArrayType;
    typedef std::array<double, 4> IntrinsecType;

    typedef enum
    {
        FILE,
        STREAM,
        DEVICE,
        UNKNOWN = 0xFF
    } SourceType;

    typedef enum
    {
        INVALID,
        ARGB32,
        ARGB32_PREMULTIPLIED,
        RGB32,
        RGB24,
        RGB565,
        RGB555,
        ARGB8565_PREMULTIPLIED,
        BGRA32,
        BGRA32_PREMULTIPLIED,
        BGR32,
        BGR24,
        BGR565,
        BGR555,
        BGRA5658_PREMULTIPLIED,

        AYUV444,
        AYUV444_PREMULTIPLIED,
        YUV444,
        YUV420P,
        YV12,
        UYVY,
        YUYV,
        NV12,
        NV21,
        IMC1,
        IMC2,
        IMC3,
        IMC4,
        Y8,
        Y16,

        JPEG,

        CAMERARAW,
        ADOBEDNG,

        RGBA32,

        USER = 1000
    } PixelFormat;

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    ARDATA_API Camera( ::fwData::Object::Key key );

    /// Destructor
    ARDATA_API virtual ~Camera() throw();

    ARDATA_API static PixelFormat getPixelFormat(const std::string& name);

    ARDATA_API static std::string getPixelFormatName(PixelFormat format);

    /// Defines shallow copy
    ARDATA_API void shallowCopy( const ::fwData::Object::csptr& _source );

    /// Defines deep copy
    ARDATA_API void cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType& cache);

    /**@name Signals API
     * @{
     */
    /// Signal when camera is calibrated
    ARDATA_API static const ::fwCom::Signals::SignalKeyType s_INTRINSIC_CALIBRATED_SIG;
    typedef ::fwCom::Signal<void ()> IntrinsicCalibratedSignalType;

    /// Signal when camera id is modified
    ARDATA_API static const ::fwCom::Signals::SignalKeyType s_ID_MODIFIED_SIG;
    typedef ::fwCom::Signal<void (std::string)> IdModifiedSignalType;
    /** @} */

    /// @return cx or u0 = x-coordinate of optical center
    double getCx() const
    {
        return m_intrinsic[2];
    }

    /// @return cy or v0 = y-coordinate of optical center
    double getCy() const
    {
        return m_intrinsic[3];
    }

    /// Sets cx or u0 = x-coordinate of optical center
    void setCx(double cx)
    {
        m_intrinsic[2] = cx;
    }

    /// Sets cy or v0 = y-coordinate of optical center
    void setCy(double cy)
    {
        m_intrinsic[3] = cy;
    }

    /// @return fx = x-coordinate of optical center
    double getFx() const
    {
        return m_intrinsic[0];
    }

    /// Sets fx = x-coordinate of optical center
    void setFx(double fx)
    {
        m_intrinsic[0] = fx;
    }

    /// @return fy = y-coordinate of optical center
    double getFy() const
    {
        return m_intrinsic[1];
    }

    /// Sets fy = y-coordinate of optical center
    void setFy(double fy)
    {
        m_intrinsic[1] = fy;
    }

    /// @return camera resolution in pixels
    size_t getWidth() const
    {
        return m_width;
    }

    /// Sets camera resolution in pixels
    void setWidth(size_t w)
    {
        m_width = w;
    }

    /// @return camera resolution in pixels
    size_t getHeight() const
    {
        return m_height;
    }

    /// Sets camera resolution in pixels
    void setHeight(size_t h)
    {
        m_height = h;
    }

    /// Sets the distortion coefficient
    ARDATA_API void setDistortionCoefficient(double k1, double k2, double p1, double p2, double k3);

    /**
     * @name Getters / Setters
     * @{ */
    /**
     * @brief distortion coefficient
     * @{ */
    const DistArrayType& getDistortionCoefficient () const
    {
        return m_distortionCoefficient;
    }
    //------------------------------------------------------------------------------

    void setDistortionCoefficient (const DistArrayType& val)
    {
        m_distortionCoefficient = val;
    }
    /**  @} */

    /**
     * @brief the skew coefficient (angle between the x and y pixel axes)
     * @{ */
    double  getSkew () const
    {
        return m_skew;
    }

    //------------------------------------------------------------------------------

    void setSkew (double val)
    {
        m_skew = val;
    }
    /**  @} */

    /**
     * @brief the flag to tell if the camera is calibrated
     * @{ */
    bool  getIsCalibrated () const
    {
        return m_isCalibrated;
    }
    //------------------------------------------------------------------------------

    void setIsCalibrated (bool val)
    {
        m_isCalibrated = val;
    }
    /**  @} */

    /// Gets the camera source (file, stream or device).
    SourceType getCameraSource() const
    {
        return m_cameraSource;
    }

    /// Sets the camera source (file, stream or device).
    void setCameraSource(SourceType cameraSource)
    {
        m_cameraSource = cameraSource;
    }

    /// Gets the human-readable description of the camera (only available in SourceType DEVICE mode).
    std::string getDescription() const
    {
        return m_description;
    }

    /// Sets the human-readable description of the camera (only available in SourceType DEVICE mode).
    void setDescription(const std::string& description)
    {
        m_description = description;
    }

    /// Gets the device name of the camera (only available in SourceType DEVICE mode).
    std::string getCameraID() const
    {
        return m_cameraID;
    }

    /// Sets the device name of the camera (only available in SourceType DEVICE mode).
    void setCameraID(const std::string& cameraID)
    {
        m_cameraID = cameraID;
    }

    /// Gets the minimum frame rate in frames per second (only available in SourceType DEVICE mode).
    float getMaximumFrameRate() const
    {
        return m_maxFrameRate;
    }

    /// Sets the minimum frame rate in frames per second (only available in SourceType DEVICE mode).
    void setMaximumFrameRate(float maxFrameRate)
    {
        m_maxFrameRate = maxFrameRate;
    }

    /// Gets the color format of a video frame (only available in SourceType DEVICE mode).
    PixelFormat getPixelFormat() const
    {
        return m_pixelFormat;
    }

    /// Sets the color format of a video frame (only available in SourceType DEVICE mode).
    void setPixelFormat(PixelFormat pixelFormat)
    {
        m_pixelFormat = pixelFormat;
    }

    /// Gets the video source file (only available in SourceType FILE mode).
    ::boost::filesystem::path getVideoFile() const
    {
        return m_videoFile;
    }

    /// Sets the video source file (only available in SourceType FILE mode).
    void setVideoFile(const ::boost::filesystem::path& videoFile)
    {
        m_videoFile = videoFile;
    }

    /// Gets the video source stream (only available in SourceType STREAM mode).
    std::string getStreamUrl() const
    {
        return m_streamUrl;
    }

    /// Sets the video source stream (only available in SourceType STREAM mode).
    void setStreamUrl(const std::string& streamUrl)
    {
        m_streamUrl = streamUrl;
    }

protected:

    //! Width video resolution
    size_t m_width;

    //! Height video resolution
    size_t m_height;

    /// Intrinsic parameters [fx, fy, cx, cy]
    IntrinsecType m_intrinsic;

    /// Image distortion coefficients (radial and tangential distortions, [k1, k2, p1, p2, k3])
    DistArrayType m_distortionCoefficient;

    /// Skew coefficient (angle between the x and y pixel axes)
    double m_skew;

    //! Flag if data is calibrated
    bool m_isCalibrated;

    //! Human-readable description of the camera.
    std::string m_description;
    //! Device name of the camera, unique ID to identify the camera and may not be human-readable.
    std::string m_cameraID;
    //! Maximum frame rate in frames per second.
    float m_maxFrameRate;
    //! Color format of a video frame.
    PixelFormat m_pixelFormat;

    //! Video source file
    ::boost::filesystem::path m_videoFile;

    //! Video source stream
    std::string m_streamUrl;

    //! Camera source (file, stream or device)
    SourceType m_cameraSource;
};

} // namespace arData

#endif // __ARDATA_CAMERA_HPP__

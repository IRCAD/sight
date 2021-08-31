/************************************************************************
 *
 * Copyright (C) 2014-2021 IRCAD France
 * Copyright (C) 2014-2020 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#pragma once

#include "data/config.hpp"
#include <data/factory/new.hpp>
#include <data/Object.hpp>

#include <core/com/Signal.hpp>
#include <core/com/Signals.hpp>

#include <array>
#include <filesystem>
#include <string>
#include <utility>

SIGHT_DECLARE_DATA_REFLECTION((sight) (data) (Camera));

namespace sight::data
{

/**
 * @brief   This class defines a camera object.
 */
class DATA_CLASS_API Camera : public data::Object
{
public:

    SIGHT_MAKE_FRIEND_REFLECTION((sight) (data) (Camera));
    SIGHT_DECLARE_CLASS(Camera, data::Object, data::factory::New<Camera>);

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

    /// Default constructor.
    DATA_API Camera();

    DATA_API Camera(const data::Camera& _cam);

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    DATA_API Camera(data::Object::Key key);

    /// Destructor
    DATA_API virtual ~Camera() noexcept;

    DATA_API static PixelFormat getPixelFormat(const std::string& name);

    DATA_API static std::string getPixelFormatName(PixelFormat format);

    /// Defines shallow copy
    DATA_API void shallowCopy(const data::Object::csptr& _source) override;

    /// Defines deep copy
    DATA_API void cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType& cache) override;

    /**@name Signals API
     * @{
     */
    /// Signal when camera is calibrated
    DATA_API static const core::com::Signals::SignalKeyType s_INTRINSIC_CALIBRATED_SIG;
    typedef core::com::Signal<void ()> IntrinsicCalibratedSignalType;

    /// Signal when camera id is modified
    DATA_API static const core::com::Signals::SignalKeyType s_ID_MODIFIED_SIG;
    typedef core::com::Signal<void (std::string)> IdModifiedSignalType;
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
    DATA_API void setDistortionCoefficient(double k1, double k2, double p1, double p2, double k3);

    /**
     * @name Getters / Setters
     * @{ */
    /**
     * @brief distortion coefficient
     * @{ */
    const DistArrayType& getDistortionCoefficient() const
    {
        return m_distortionCoefficient;
    }

    //------------------------------------------------------------------------------

    void setDistortionCoefficient(const DistArrayType& val)
    {
        m_distortionCoefficient = val;
    }

    /**  @} */

    /**
     * @brief the skew coefficient (angle between the x and y pixel axes)
     * @{ */
    double getSkew() const
    {
        return m_skew;
    }

    //------------------------------------------------------------------------------

    void setSkew(double val)
    {
        m_skew = val;
    }

    /**  @} */

    /**
     * @brief the flag to tell if the camera is calibrated
     * @{ */
    bool getIsCalibrated() const
    {
        return m_isCalibrated;
    }

    //------------------------------------------------------------------------------

    void setIsCalibrated(bool val)
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
    std::filesystem::path getVideoFile() const
    {
        return m_videoFile;
    }

    /// Sets the video source file (only available in SourceType FILE mode).
    void setVideoFile(const std::filesystem::path& videoFile)
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

    /**
     * @brief setScale set the scale factor of the camera
     * Used for RGBD cameras, where depth sensor and RGB sensor are up to a scale factor.
     * @param scale double (default 1.0)
     */
    void setScale(double scale);
    /**
     * @brief getScale get the scale factor of the camera (see: setScale)
     * @return the scale factor in double.
     */
    double getScale() const;

    /**
     * @brief getIndex returns index of the device as Qt give us in module::ui::qt::video::CameraDeviceDlg.
     * The index is the first character of m_description. (ex: "1. Microsoft HD Camera")
     * @return an integer of the index, -1 if unvalid (if SourceType isn't DECVICE)
     */
    int getIndex() const;

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
    std::filesystem::path m_videoFile;

    //! Video source stream
    std::string m_streamUrl;

    //! Camera source (file, stream or device)
    SourceType m_cameraSource;

    //! Used for depth sensor: scale of the depth values (default: 1.)
    double m_scale;
};

//-----------------------------------------------------------------------------

inline void Camera::setScale(double scale)
{
    m_scale = scale;
}

//-----------------------------------------------------------------------------

inline double Camera::getScale() const
{
    return m_scale;
}

//-----------------------------------------------------------------------------

inline int Camera::getIndex() const
{
    int index = -1;
    if(m_cameraSource == SourceType::DEVICE)
    {
        try
        {
            // Limited to [0-9] range
            index = std::stoi(&m_description.at(0));
        }
        catch(std::exception& _e)
        {
            SIGHT_ERROR("Cannot get index of: " + m_description + " " + _e.what());
        }
    }

    return index;
}

//-----------------------------------------------------------------------------

} // namespace sight::data

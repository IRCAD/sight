/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
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
#include "data/factory/new.hpp"
#include "data/object.hpp"

#include <core/com/signal.hpp>
#include <core/com/signals.hpp>

#include <data/factory/new.hpp>
#include <data/object.hpp>

#include <array>
#include <filesystem>
#include <string>
#include <utility>

namespace sight::data
{

/**
 * @brief   This class defines a camera object.
 */
class DATA_CLASS_API camera : public object
{
public:

    SIGHT_DECLARE_CLASS(camera, object);

    typedef std::array<double, 5> dist_array_t;
    typedef std::array<double, 4> intrinsic_t;

    typedef enum
    {
        FILE,
        STREAM,
        DEVICE,
        UNKNOWN = 0xFF
    } source_t;

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
    DATA_API camera();
    DATA_API camera(const camera& _cam);

    /// Destructor
    DATA_API ~camera() noexcept override = default;

    DATA_API static PixelFormat getPixelFormat(const std::string& _name);

    DATA_API static std::string getPixelFormatName(PixelFormat _format);

    /**@name Signals API
     * @{
     */
    /// Signal when camera is calibrated
    DATA_API static const core::com::signals::key_t INTRINSIC_CALIBRATED_SIG;
    typedef core::com::signal<void ()> intrinsic_calibrated_signal_t;

    /// Signal when camera id is modified
    DATA_API static const core::com::signals::key_t ID_MODIFIED_SIG;
    typedef core::com::signal<void (std::string)> id_modified_signal_t;
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
    void setCx(double _cx)
    {
        m_intrinsic[2] = _cx;
    }

    /// Sets cy or v0 = y-coordinate of optical center
    void setCy(double _cy)
    {
        m_intrinsic[3] = _cy;
    }

    /// @return fx = x-coordinate of optical center
    double getFx() const
    {
        return m_intrinsic[0];
    }

    /// Sets fx = x-coordinate of optical center
    void setFx(double _fx)
    {
        m_intrinsic[0] = _fx;
    }

    /// @return fy = y-coordinate of optical center
    double getFy() const
    {
        return m_intrinsic[1];
    }

    /// Sets fy = y-coordinate of optical center
    void setFy(double _fy)
    {
        m_intrinsic[1] = _fy;
    }

    /// @return camera resolution in pixels
    std::size_t getWidth() const
    {
        return m_width;
    }

    /// Sets camera resolution in pixels
    void setWidth(std::size_t _w)
    {
        m_width = _w;
    }

    /// @return camera resolution in pixels
    std::size_t getHeight() const
    {
        return m_height;
    }

    /// Sets camera resolution in pixels
    void setHeight(std::size_t _h)
    {
        m_height = _h;
    }

    /// Sets the distortion coefficient
    DATA_API void setDistortionCoefficient(double _k1, double _k2, double _p1, double _p2, double _k3);

    /**
     * @name Getters / Setters
     * @{ */
    /**
     * @brief distortion coefficient
     * @{ */
    const dist_array_t& getDistortionCoefficient() const
    {
        return m_distortionCoefficient;
    }

    //------------------------------------------------------------------------------

    void setDistortionCoefficient(const dist_array_t& _val)
    {
        m_distortionCoefficient = _val;
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

    void setSkew(double _val)
    {
        m_skew = _val;
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

    void setIsCalibrated(bool _val)
    {
        m_isCalibrated = _val;
    }

    /**  @} */

    /// Gets the camera source (file, stream or device).
    source_t getCameraSource() const
    {
        return m_cameraSource;
    }

    /// Sets the camera source (file, stream or device).
    void setCameraSource(source_t _camera_source)
    {
        m_cameraSource = _camera_source;
    }

    /// Gets the device name of the camera (only available in source_t DEVICE mode).
    std::string getCameraID() const
    {
        return m_cameraID;
    }

    /// Sets the device name of the camera (only available in source_t DEVICE mode).
    void setCameraID(const std::string& _camera_id)
    {
        m_cameraID = _camera_id;
    }

    /// Gets the minimum frame rate in frames per second (only available in source_t DEVICE mode).
    float getMaximumFrameRate() const
    {
        return m_maxFrameRate;
    }

    /// Sets the minimum frame rate in frames per second (only available in source_t DEVICE mode).
    void setMaximumFrameRate(float _max_frame_rate)
    {
        m_maxFrameRate = _max_frame_rate;
    }

    /// Gets the color format of a video frame (only available in source_t DEVICE mode).
    PixelFormat getPixelFormat() const
    {
        return m_pixelFormat;
    }

    /// Sets the color format of a video frame (only available in source_t DEVICE mode).
    void setPixelFormat(PixelFormat _pixel_format)
    {
        m_pixelFormat = _pixel_format;
    }

    /// Gets the video source file (only available in source_t FILE mode).
    std::filesystem::path getVideoFile() const
    {
        return m_videoFile;
    }

    /// Sets the video source file (only available in source_t FILE mode).
    void setVideoFile(const std::filesystem::path& _video_file)
    {
        m_videoFile = _video_file;
    }

    /// Gets the video source stream (only available in source_t STREAM mode).
    std::string getStreamUrl() const
    {
        return m_streamUrl;
    }

    /// Sets the video source stream (only available in source_t STREAM mode).
    void setStreamUrl(const std::string& _stream_url)
    {
        m_streamUrl = _stream_url;
    }

    /**
     * @brief setScale set the scale factor of the camera
     * Used for RGBD cameras, where depth sensor and RGB sensor are up to a scale factor.
     * @param scale double (default 1.0)
     */
    void setScale(double _scale);
    /**
     * @brief getScale get the scale factor of the camera (see: setScale)
     * @return the scale factor in double.
     */
    double getScale() const;

    /**
     * @brief getIndex returns index of the device as Qt give us in module::ui::qt::video::CameraDeviceDlg.
     * The index is the first character of m_description. (ex: "1. Microsoft HD camera")
     * @return an integer of the index, -1 if invalid (if source_t isn't DEVICES)
     */
    int getIndex() const;

    /// Equality comparison operators
    /// @{
    DATA_API bool operator==(const camera& _other) const noexcept;
    DATA_API bool operator!=(const camera& _other) const noexcept;
    /// @}

    /// Defines shallow copy
    /// @throws data::exception if an errors occurs during copy
    /// @param[in] source the source object to copy
    DATA_API void shallow_copy(const object::csptr& _source) override;

    /// Defines deep copy
    /// @throws data::exception if an errors occurs during copy
    /// @param source source object to copy
    /// @param cache cache used to deduplicate pointers
    DATA_API void deep_copy(
        const object::csptr& _source,
        const std::unique_ptr<deep_copy_cache_t>& _cache = std::make_unique<deep_copy_cache_t>()
    ) override;

protected:

    //! Width video resolution
    std::size_t m_width {0};

    //! Height video resolution
    std::size_t m_height {0};

    /// Intrinsic parameters [fx, fy, cx, cy]
    intrinsic_t m_intrinsic {};

    /// Image distortion coefficients (radial and tangential distortions, [k1, k2, p1, p2, k3])
    dist_array_t m_distortionCoefficient {};

    /// Skew coefficient (angle between the x and y pixel axes)
    double m_skew {0.};

    //! Flag if data is calibrated
    bool m_isCalibrated {false};

    //! Device name of the camera, unique ID to identify the camera and may not be human-readable.
    std::string m_cameraID;

    //! Maximum frame rate in frames per second.
    float m_maxFrameRate {30.F};

    //! Color format of a video frame.
    PixelFormat m_pixelFormat {INVALID};

    //! Video source file
    std::filesystem::path m_videoFile;

    //! Video source stream
    std::string m_streamUrl;

    //! camera source (file, stream or device)
    source_t m_cameraSource {UNKNOWN};

    //! Used for depth sensor: scale of the depth values (default: 1.)
    double m_scale {1.};
};

//-----------------------------------------------------------------------------

inline void camera::setScale(double _scale)
{
    m_scale = _scale;
}

//-----------------------------------------------------------------------------

inline double camera::getScale() const
{
    return m_scale;
}

//-----------------------------------------------------------------------------

inline int camera::getIndex() const
{
    int index = -1;
    if(m_cameraSource == source_t::DEVICE)
    {
        try
        {
            // Limited to [0-9] range
            index = std::stoi(&m_description.at(0));
        }
        catch(std::exception& e)
        {
            SIGHT_ERROR("Cannot get index of: " + m_description + " " + e.what());
        }
    }

    return index;
}

//-----------------------------------------------------------------------------

} // namespace sight::data

/************************************************************************
 *
 * Copyright (C) 2014-2024 IRCAD France
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

#include <sight/data/config.hpp>

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
class SIGHT_DATA_CLASS_API camera : public object
{
public:

    SIGHT_DECLARE_CLASS(camera, object);

    using dist_array_t = std::array<double, 5>;
    using intrinsic_t  = std::array<double, 4>;

    enum source_t
    {
        file,
        stream,
        device,
        unknown = 0xFF
    };

    enum pixel_format
    {
        invalid,
        argb32,
        argb32_premultiplied,
        rgb32,
        rgb24,
        rgb565,
        rgb555,
        argb8565_premultiplied,
        bgra32,
        bgra32_premultiplied,
        bgr32,
        bgr24,
        bgr565,
        bgr555,
        bgra5658_premultiplied,
        ayuv444,
        ayuv444_premultiplied,
        yuv444,
        yuv420_p,
        yv12,
        uyvy,
        yuyv,
        nv12,
        nv21,
        imc1,
        imc2,
        imc3,
        imc4,
        y8,
        y16,
        jpeg,
        cameraraw,
        adobedng,
        rgba32,
        user = 1000
    };

    /// Default constructor.
    SIGHT_DATA_API camera();
    SIGHT_DATA_API camera(const camera& _cam);

    /// Destructor
    SIGHT_DATA_API ~camera() noexcept override = default;

    SIGHT_DATA_API static enum pixel_format pixel_format(const std::string& _name);

    SIGHT_DATA_API static std::string get_pixel_format_name(enum pixel_format _format);

    /**@name Signals API
     * @{
     */
    /// Signal when camera is calibrated
    SIGHT_DATA_API static const core::com::signals::key_t INTRINSIC_CALIBRATED_SIG;
    using intrinsic_calibrated_signal_t = core::com::signal<void ()>;

    /// Signal when camera id is modified
    SIGHT_DATA_API static const core::com::signals::key_t ID_MODIFIED_SIG;
    using id_modified_signal_t = core::com::signal<void (std::string)>;
    /** @} */

    /// @return cx or u0 = x-coordinate of optical center
    double get_cx() const
    {
        return m_intrinsic[2];
    }

    /// @return cy or v0 = y-coordinate of optical center
    double get_cy() const
    {
        return m_intrinsic[3];
    }

    /// Sets cx or u0 = x-coordinate of optical center
    void set_cx(double _cx)
    {
        m_intrinsic[2] = _cx;
    }

    /// Sets cy or v0 = y-coordinate of optical center
    void set_cy(double _cy)
    {
        m_intrinsic[3] = _cy;
    }

    /// @return fx = x-coordinate of optical center
    double get_fx() const
    {
        return m_intrinsic[0];
    }

    /// Sets fx = x-coordinate of optical center
    void set_fx(double _fx)
    {
        m_intrinsic[0] = _fx;
    }

    /// @return fy = y-coordinate of optical center
    double get_fy() const
    {
        return m_intrinsic[1];
    }

    /// Sets fy = y-coordinate of optical center
    void set_fy(double _fy)
    {
        m_intrinsic[1] = _fy;
    }

    /// @return camera resolution in pixels
    std::size_t get_width() const
    {
        return m_width;
    }

    /// Sets camera resolution in pixels
    void set_width(std::size_t _w)
    {
        m_width = _w;
    }

    /// @return camera resolution in pixels
    std::size_t get_height() const
    {
        return m_height;
    }

    /// Sets camera resolution in pixels
    void set_height(std::size_t _h)
    {
        m_height = _h;
    }

    /// Sets the distortion coefficient
    SIGHT_DATA_API void set_distortion_coefficient(double _k1, double _k2, double _p1, double _p2, double _k3);

    /**
     * @name Getters / Setters
     * @{ */
    /**
     * @brief distortion coefficient
     * @{ */
    const dist_array_t& get_distortion_coefficient() const
    {
        return m_distortion_coefficient;
    }

    //------------------------------------------------------------------------------

    void set_distortion_coefficient(const dist_array_t& _val)
    {
        m_distortion_coefficient = _val;
    }

    /**  @} */

    /**
     * @brief the skew coefficient (angle between the x and y pixel axes)
     * @{ */
    double get_skew() const
    {
        return m_skew;
    }

    //------------------------------------------------------------------------------

    void set_skew(double _val)
    {
        m_skew = _val;
    }

    /**  @} */

    /**
     * @brief the flag to tell if the camera is calibrated
     * @{ */
    bool get_is_calibrated() const
    {
        return m_is_calibrated;
    }

    //------------------------------------------------------------------------------

    void set_is_calibrated(bool _val)
    {
        m_is_calibrated = _val;
    }

    /**  @} */

    /// Gets the camera source (file, stream or device).
    source_t get_camera_source() const
    {
        return m_camera_source;
    }

    /// Sets the camera source (file, stream or device).
    void set_camera_source(source_t _camera_source)
    {
        m_camera_source = _camera_source;
    }

    /// Gets the device name of the camera (only available in source_t DEVICE mode).
    std::string get_camera_id() const
    {
        return m_camera_id;
    }

    /// Sets the device name of the camera (only available in source_t DEVICE mode).
    void set_camera_id(const std::string& _camera_id)
    {
        m_camera_id = _camera_id;
    }

    /// Gets the minimum frame rate in frames per second (only available in source_t DEVICE mode).
    float get_maximum_frame_rate() const
    {
        return m_max_frame_rate;
    }

    /// Sets the minimum frame rate in frames per second (only available in source_t DEVICE mode).
    void set_maximum_frame_rate(float _max_frame_rate)
    {
        m_max_frame_rate = _max_frame_rate;
    }

    /// Gets the color format of a video frame (only available in source_t DEVICE mode).
    enum pixel_format pixel_format() const
    {
        return m_pixel_format;
    }

    /// Sets the color format of a video frame (only available in source_t DEVICE mode).
    void set_pixel_format(enum pixel_format _pixel_format)
    {
        m_pixel_format = _pixel_format;
    }

    /// Gets the video source file (only available in source_t FILE mode).
    std::filesystem::path get_video_file() const
    {
        return m_video_file;
    }

    /// Sets the video source file (only available in source_t FILE mode).
    void set_video_file(const std::filesystem::path& _video_file)
    {
        m_video_file = _video_file;
    }

    /// Gets the video source stream (only available in source_t STREAM mode).
    std::string get_stream_url() const
    {
        return m_stream_url;
    }

    /// Sets the video source stream (only available in source_t STREAM mode).
    void set_stream_url(const std::string& _stream_url)
    {
        m_stream_url = _stream_url;
    }

    /**
     * @brief setScale set the scale factor of the camera
     * Used for RGBD cameras, where depth sensor and RGB sensor are up to a scale factor.
     * @param _scale double (default 1.0)
     */
    void set_scale(double _scale);
    /**
     * @brief getScale get the scale factor of the camera (see: setScale)
     * @return the scale factor in double.
     */
    double get_scale() const;

    /**
     * @brief set the calibration error
     * @param _error double (default 0.0)
     */
    void set_calibration_error(double _error);
    /**
     * @brief get the calibration error of the camera
     * @return the calibration error in double.
     */
    double calibration_error() const;

    /**
     * @brief getIndex returns index of the device as Qt give us in module::ui::qt::video::CameraDeviceDlg.
     * The index is the first character of m_description. (ex: "1. Microsoft HD camera")
     * @return an integer of the index, -1 if invalid (if source_t isn't DEVICES)
     */
    int get_index() const;

    /// Equality comparison operators
    /// @{
    SIGHT_DATA_API bool operator==(const camera& _other) const noexcept;
    SIGHT_DATA_API bool operator!=(const camera& _other) const noexcept;
    /// @}

    /// Defines shallow copy
    /// @throws data::exception if an errors occurs during copy
    /// @param[in] _source the source object to copy
    SIGHT_DATA_API void shallow_copy(const object::csptr& _source) override;

    /// Defines deep copy
    /// @throws data::exception if an errors occurs during copy
    /// @param _source source object to copy
    /// @param _cache cache used to deduplicate pointers
    SIGHT_DATA_API void deep_copy(
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
    dist_array_t m_distortion_coefficient {};

    /// Skew coefficient (angle between the x and y pixel axes)
    double m_skew {0.};

    //! Flag if data is calibrated
    bool m_is_calibrated {false};

    //! Device name of the camera, unique ID to identify the camera and may not be human-readable.
    std::string m_camera_id;

    //! Maximum frame rate in frames per second.
    float m_max_frame_rate {30.F};

    //! Color format of a video frame.
    enum pixel_format m_pixel_format
    {
        invalid
    };

    //! Video source file
    std::filesystem::path m_video_file;

    //! Video source stream
    std::string m_stream_url;

    //! camera source (file, stream or device)
    source_t m_camera_source {unknown};

    //! Used for depth sensor: scale of the depth values (default: 1.)
    double m_scale {1.};

    /// Error of the computed calibration
    double m_calibration_error {0.};
};

//-----------------------------------------------------------------------------

inline void camera::set_scale(double _scale)
{
    m_scale = _scale;
}

//-----------------------------------------------------------------------------

inline double camera::get_scale() const
{
    return m_scale;
}

//-----------------------------------------------------------------------------

inline void camera::set_calibration_error(double _calibration_error)
{
    m_calibration_error = _calibration_error;
}

//-----------------------------------------------------------------------------

inline double camera::calibration_error() const
{
    return m_calibration_error;
}

//-----------------------------------------------------------------------------

inline int camera::get_index() const
{
    int index = -1;
    if(m_camera_source == source_t::device)
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

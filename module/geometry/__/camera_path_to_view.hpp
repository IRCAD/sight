/************************************************************************
 *
 * Copyright (C) 2026 IRCAD France
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

#include <core/thread/timer.hpp>

#include <data/image_series.hpp>
#include <data/integer.hpp>
#include <data/matrix4.hpp>
#include <data/real.hpp>

#include <service/base.hpp>

#include <chrono>
#include <cstdint>
#include <string>

namespace sight::module::geometry
{

/**
 * @brief Plays an ordered fiducial path and produces its camera transform.
 *
 * The output transform follows the camera adaptor's transform convention. Camera orientation is interpolated between
 * adjacent path points, and distance wraps over the total path length.
 *
 * @section Slots Slots
 * - \b play(): starts path playback.
 * - \b pause(): pauses path playback.
 *
 * @section XML XML Configuration
 * @code{.xml}
 * <service uid="camera_view_generator" type="sight::module::geometry::camera_path_to_view">
 *     <data image="..." transform_out="..." />
 *     <config path_group="Path" distance="..." />
 * </service>
 * @endcode
 *
 * @section Data Data
 * @subsection Input Input
 * - \b data.image [sight::data::image_series] (required): image series containing path fiducials.
 *
 * @subsection InOut InOut
 * - \b data.transform_out [sight::data::matrix4] (required): output camera pose.
 *
 * @subsection Configuration Configuration
 * - \b config.path_group [string] (optional): fiducial group; defaults to `Path`.
 * - \b config.distance [sight::data::real] (optional): current path distance in millimetres; defaults to 0.
 * - \b config.speed_mm_s [sight::data::integer] (optional): playback speed; defaults to 10 mm/s.
 * - \b config.direction [sight::data::integer] (optional): backward (0) or forward (1); defaults to forward.
 */
class camera_path_to_view final : public service::base
{
public:

    enum direction : std::uint8_t
    {
        backward = 0,
        forward  = 1
    };

    struct slots
    {
        static inline const slot_key_t PLAY  = "play";
        static inline const slot_key_t PAUSE = "pause";
    };

    SIGHT_DECLARE_SERVICE(camera_path_to_view, service::base);

    camera_path_to_view();
    ~camera_path_to_view() final = default;

protected:

    void configuring() final;
    void starting() final;
    void stopping() final;
    void updating() final;
    connections_t auto_connections() const final;

private:

    void play();
    void pause();
    void advance();

    std::string m_path_group {"Path"};
    core::thread::timer::sptr m_timer;
    std::chrono::steady_clock::time_point m_last_tick {};

    ptr_in<data::image_series> m_image {this, "data.image"};
    ptr_inout<data::matrix4> m_transform {this, "data.transform_out"};
    ptr_inout<data::real> m_distance {this, "config.distance", 0.0};
    ptr_in<data::integer> m_speed_mm_s {this, "config.speed_mm_s", 10};
    ptr_in<data::integer> m_direction {this, "config.direction", direction::forward};
};

} // namespace sight::module::geometry

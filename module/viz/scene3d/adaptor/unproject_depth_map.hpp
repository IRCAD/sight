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

#include <data/image.hpp>
#include <viz/scene3d/adaptor.hpp>

namespace sight::module::viz::scene3d::adaptor
{

/**
 * @brief Unproject a screen-space depth map into the related scene view-space.
 *
 * @section XML XML Configuration
 * @code{.xml}
        <service uid="..." type="sight::module::viz::scene3d::adaptor::unproject_depth_map">
            <in key="depthMapIn" uid="..." >
            <out key="depthMapOut" uid="..." >
            <config layer="..."/>
       </service>
   @endcode
 *
 * @subsection Input Input
 * - \b depthMapIn [sight::data::image](mandatory): The depth map to unproject; they must be in screen-space; have
 * unsigned char pixels; be in two dimension and have at least three components.
 *
 * @subsection Output Output
 * - \b depthMapOut [sight::data::image](mandatory): projected depth map; it's in view-space; have float pixels; be
 * in two dimensional and have one component.
 *
 * @subsection Configuration Configuration:
 * - \b layer (mandatory): layer where the un-projection will be done.
 */
class unproject_depth_map final : public sight::viz::scene3d::adaptor
{
public:

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(unproject_depth_map, sight::viz::scene3d::adaptor);

    /// Initialize the slot and the signal.
    unproject_depth_map() noexcept = default;

    /// Destroys the service.
    ~unproject_depth_map() noexcept final = default;

protected:

    /// Does nothing.
    void configuring() final;

    /// Does nothing.
    void starting() final;

    /**
     * @brief Proposals to connect service slots to associated object signals.
     * @return A map of each proposed connection.
     *
     * Connects data::signals::MODIFIED of s_DEPTH_MAP_INPUT to service::slots::UPDATE
     * Connects data::image::signals::BUFFER_MODIFIED of s_DEPTH_MAP_INPUT to service::slots::UPDATE
     */
    sight::service::connections_t auto_connections() const final;

    /// Un-projects the depth map.
    void updating() final;

    /// Does nothing.
    void stopping() final;

private:

    static constexpr std::string_view DEPTH_MAP_IN  = "data.depthMapIn";
    static constexpr std::string_view DEPTH_MAP_OUT = "data.depthMapOut";

    sight::data::ptr<sight::data::image, sight::data::access::in> m_depth_map_in {this, DEPTH_MAP_IN};
    sight::data::ptr<sight::data::image, sight::data::access::out> m_depth_map_out {this, DEPTH_MAP_OUT};
};

} // namespace sight::module::viz::scene3d::adaptor

/************************************************************************
 *
 * Copyright (C) 2018-2024 IRCAD France
 * Copyright (C) 2018-2020 IHU Strasbourg
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

#include "modules/viz/scene3d/adaptor/frustum.hpp"

#include <data/camera.hpp>
#include <data/matrix4.hpp>

#include <viz/scene3d/adaptor.hpp>
#include <viz/scene3d/transformable.hpp>

#include <boost/circular_buffer.hpp>

namespace sight::module::viz::scene3d::adaptor
{

/**
 * @brief This adaptor displays a new Frustum each time the transform is updated.
 * The number of Frustum is fixed, if the maximum number of Frustum is reached the oldest one will be replaced.
 *
 * @section Slots Slots
 * - \b update_visibility(bool): sets whether frustums are shown or not.
 * - \b toggle_visibility(): toggles whether frustums are shown or not.
 * - \b show(): shows frustums.
 * - \b hide(): hides frustums.
 * - \b update(bool): adds a frustum in the list and displays it.
 * - \b clear(): clears frustum list.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
        <service type="sight::module::viz::scene3d::adaptor::frustum_list">
            <in key="camera" uid="..." />
            <in key="transform" uid="..." />
            <config near="0.1" far="300" color="#f8e119" transform="..." />
       </service>
   @endcode
 *
 * @subsection Input Input:
 * - \b camera [sight::data::camera]: data::camera that handles calibration parameters
 * - \b transform [sight::data::matrix4]: each time this transform is modified, a frustum is created.
 *
 * @subsection Configuration Configuration:
 * - \b near (optional, float, default=1.0): near clipping distance of the Ogre::Camera
 * - \b far (optional, float, default=20.0): far clipping distance of the Ogre::Camera
 * - \b color (optional, hexadecimal, default=0x0000FF): frustum's color
 * - \b transform (optional, string, default=""): transform applied to the frustumList's scene node
 * - \b visible (optional, bool, default=true): the visibility of the adaptor.
 */
class frustum_list final :
    public sight::viz::scene3d::adaptor,
    public sight::viz::scene3d::transformable
{
public:

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(frustum_list, sight::viz::scene3d::adaptor);

    /// Creates slots.
    frustum_list() noexcept;

    /// Destroys the adaptor.
    ~frustum_list() noexcept override = default;

protected:

    /// Configures.
    void configuring() override;

    /// Initializes the material.
    void starting() override;

    /**
     * @brief Proposals to connect service slots to associated object signals.
     * @return A map of each proposed connection.
     *
     * Connect data::matrix4::MODIFIED_SIG of s_TRANSFORM_INPUT to adaptor::slots::LAZY_UPDATE
     */
    service::connections_t auto_connections() const override;

    /// Updates the adaptor by attaching new cameras to scene nodes.
    void updating() override;

    /// Clears data.
    void stopping() override;

    /**
     * @brief Sets the frustum list visibility.
     * @param _visible the visibility status of the frustum list.
     */
    void set_visible(bool _visible) override;

private:

    /// SLOT: clears frustum list.
    void clear();

    /// SLOT: adds a frustum in the list and displays it.
    void add_frustum();

    /// Transform the data::Transform into position / oriention of the scene node.
    void set_transfrom_to_node(Ogre::SceneNode* _node);

    /// Defines the near clipping distance.
    float m_near {1.F};

    /// Defines the far clipping distance.
    float m_far {20.F};

    /// Defines the color of frustum.
    std::string m_color {"#0000FF"};

    /// Defines the maximum capacity of frustum list.
    unsigned int m_capacity {50};

    using frustum_t = std::pair<Ogre::ManualObject*, Ogre::SceneNode*>;

    /// Stores a circular list of frustum adaptors.
    boost::circular_buffer<frustum_t> m_frustum_list {};

    /// Uses to generate unique ID for each Ogre::Camera.
    std::size_t m_current_cam_index {0};

    /// Contains the Ogre material adaptor.
    module::viz::scene3d::adaptor::material::sptr m_material_adaptor {nullptr};

    /// Contains the material data.
    data::material::sptr m_material {nullptr};

    static constexpr std::string_view TRANSFORM_INPUT = "transform";

    sight::data::ptr<sight::data::camera, sight::data::access::in> m_camera {this, "camera"};
    sight::data::ptr<sight::data::matrix4, sight::data::access::in> m_transform {this, TRANSFORM_INPUT};
};

} // namespace sight::module::viz::scene3d::adaptor.

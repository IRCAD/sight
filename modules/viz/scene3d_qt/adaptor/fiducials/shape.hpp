/************************************************************************
 *
 * Copyright (C) 2024 IRCAD France
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

#include <data/fiducials_series.hpp>
#include <data/helper/medical_image.hpp>
#include <data/image_series.hpp>

#include <viz/scene3d/adaptor.hpp>
#include <viz/scene3d/interactor/base.hpp>
#include <viz/scene3d/material.hpp>

namespace sight::module::viz::scene3d_qt::adaptor::fiducials
{

/**
 * @brief This adaptor displays shapes retrieved from the fiducials.
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service uid="..." type="sight::module::viz::scene3d::adaptor::fiducials::shape" auto_connect="true" >
        <inout key="image" uid="..." />
        <config fontSource="DejaVuSans.ttf" fontSize="32" radius="4.5" priority="2" />
    </service>
   @endcode
 *
 * @subsection Input Input:
 * - \b image [sight::data::image_series]: image containing the fiducials.
 *
 * @subsection Configuration Configuration:
 * - \b orientation (optional, string, default="z_axis"): orientation of the image.
 * - \b show_on_all_slices (optional, bool, default=false): show shapes on all slices.
 * - \b fontSize (optional, int, default=12): font size in points.
 * - \b radius (optional, float, default=3.0): radius of spheres.
 * - \b interactive (optional, bool, default=true): enable interactions with shapes.
 * - \b priority (optional, int, default=2): priority of the interactor.
 * - \b queryMask (optional, int, default=0xFFFFFFFF): mask used to filter out entities when the shape is auto snapped.
 * - \b queryFlags (optional, int, default=Ogre::SceneManager::ENTITY_TYPE_MASK): mask used to filter shapes, it
 * optimizes the ray launched to retrieve the picked shape.
 * - \b color (optional, string, default=""): color applied to the shape, generated if empty.
 *
 * @section Slots Slots
 * - \b activate_shape_tool(): activates the shape tool by changing the cursor and updating a boolean.
 */
class shape final : public sight::viz::scene3d::adaptor,
                    public sight::viz::scene3d::interactor::base
{
public:

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(shape, sight::viz::scene3d::adaptor);

    /// Creates the adaptor an initialize slots.
    shape() noexcept;

    /// Destroys the adaptor.
    ~shape() noexcept final = default;

    struct  slots final
    {
        using key_t = sight::core::com::slots::key_t;

        inline static const key_t ACTIVATE_SHAPE_TOOL = "activate_shape_tool";
    };

    /// SLOT: Activates the shape tool by changing the cursor and updating a boolean
    void activate_shape_tool(bool _activate);

protected:

    /// Configures the adaptor.
    void configuring() final;

    /// Creates the material for the bounding boxes.
    void starting() final;

    sight::service::connections_t auto_connections() const final;

    /// Computes and renders the bounding boxes.
    void updating() final;

    /// Removes the actor from the renderer.
    void stopping() final;

    /**
     * @brief Sets the entity visibility.
     * @param _visible the visibility status.
     */
    void set_visible(bool _visible) final;

private:

    struct private_slots final
    {
        using key_t = sight::core::com::slots::key_t;
        inline static const key_t SHOW_ON_CURRENT_SLICE = "show_on_current_slice";
    };

    /// SLOT: Hide / show shapes when current slice change.
    void show_on_current_slice();

    struct ogre_shape;

    /// Ogre shapes per frame vector.
    /// The indexes match the indexes of the fiducial_set in the fiducial_series from the image_series.
    std::vector<std::vector<std::shared_ptr<ogre_shape> > > m_shapes;

    using orientation_t = sight::data::helper::medical_image::orientation_t;

    orientation_t m_orientation {
        orientation_t::z_axis
    };

    /// Defines whether or not interactions are enabled with shapes.
    bool m_show_on_all_slices {false};

    /// Defines the radius of spheres.
    float m_sphere_radius {3.0F};

    /// Defines the font size in points.
    std::size_t m_font_size {12};

    /// Defines whether or not interactions are enabled with shapes.
    bool m_interactive {true};

    /// Defines the priority of the interactor.
    int m_priority {2};

    /// Counter to swap color at each new shape
    int m_color_index {0};

    /// Color extracted from the configuration
    std::string m_color;

    /// Defines the mask used to filter out entities when the shape is auto snapped.
    std::uint32_t m_query_mask {0xFFFFFFFF};

    /// Defines the mask used to filter shapes, it optimizes the ray launched to retrieve the picked shape.
    std::uint32_t m_query_flag {Ogre::SceneManager::ENTITY_TYPE_MASK};

    /// Defines the material name with no depth check for spheres.
    std::string m_sphere_material_name;

    /// Defines the material name with depth check for lines.
    std::string m_line_material_name;

    /// Defines the material name with no depth check for dashed lines.
    std::string m_dashed_line_material_name;

    /// Contains the material with no depth check for spheres.
    sight::viz::scene3d::material::uptr m_sphere_material;

    /// Contains the material with depth check for lines.
    sight::viz::scene3d::material::uptr m_line_material;

    /// Contains the material with no depth check for dashed lines.
    sight::viz::scene3d::material::uptr m_dashed_line_material;

    static constexpr std::string_view s_IMAGE_INOUT = "image";
    sight::data::ptr<sight::data::image_series, sight::data::access::inout> m_image {this, s_IMAGE_INOUT};
};

} // namespace sight::module::viz::scene3d_qt::adaptor::fiducials

/************************************************************************
 *
 * Copyright (C) 2014-2025 IRCAD France
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

#include <data/helper/medical_image.hpp>
#include <data/string.hpp>

#include <viz/scene3d/picking_cross.hpp>
#include <viz/scene3d/plane.hpp>
#include <viz/scene3d/texture.hpp>
#include <viz/scene3d/transfer_function.hpp>
#include <viz/scene3d/transformable.hpp>

#include <module/viz/scene3d/adaptor/negato.hpp>

namespace sight::module::viz::scene3d::adaptor
{

/**
 * @brief This adaptor displays a 2D negato.
 *
 * @section Signals Signals
 * - \b sliceIndexChanged(): emitted when the slice index changed.
 *
 * @section Slots Slots
 * - \b sliceType(int, int): updates image slice index .
 * - \b sliceIndex(int, int, int): updates image slice type.
 * - \b update_visibility(bool): shows or hides the negato.
 * - \b toggle_visibility(): toggle whether the negato is shown or not.
 * - \b show(): shows the negato.
 * - \b hide(): hides the negato.
 * - \b update_slices_from_world(double, double, double): updates image slices indexes according to a 3d world point
 * or landmark.
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service type="sight::module::viz::scene3d::adaptor::negato2d">
        <in key="image" uid="..." />
        <in key="mask" uid="..." />
        <inout key="tf" uid="..." />
        <config sliceIndex="axial" filtering="none" tf_alpha="true" />
        <properties classification="pre" visible="true" />
   </service>
   @endcode
 *
 * @subsection Input Input:
 * - \b image [sight::data::image]: image to display.
 * - \b mask [sight::data::image] (optional): mask to apply onto the image. Values < 0.5 are considered masked.
 *
 * @subsection In-Out In-Out
 * - \b tf [sight::data::transfer_function] (optional): the current TransferFunction. If it is not defined, we use the
 *      image's default transferFunction (CT-GreyLevel).
 *
 * @subsection Configuration Configuration:
 * - \b sliceIndex (optional, axial/frontal/sagittal, default=axial): orientation of the negato.
 * - \b filtering (optional, none/linear/anisotropic, default=none): texture filter type of the negato.
 * - \b tf_alpha (optional, bool, default=false): if true, the alpha channel of the transfer function is used.
 * - \b border (optional, bool, default=true): displays a border around the plane.
 * - \b slicesCross (optional, bool, default=true): display the two other slices location as two lines.
 * - \b transform (optional, string, default=""): the name of the Ogre transform node where to attach the negato, as it
 *      was specified in the transform adaptor.
 * - \b interactive (optional, bool, default=false): enables interactions on the negato.
 *
 * @subsection Properties Properties:
 * - \b classification (optional, pre/post, default=pre): classification of voxels. "pre" means the filtering is applied
 * after the sampling of the transfer function, and "post" after. When using labelled images, it is highly recommended
 * to use "pre", otherwise it is likely that class of objects can be confounded.
 * - \b visible (optional, bool, default=true): the visibility of the adaptor.
 * - \b depth_bias: useful to superimpose multiple negatos on top of each other
 */
class negato2d final : public sight::module::viz::scene3d::adaptor::negato
{
public:

    using axis_t = data::helper::medical_image::axis_t;

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(negato2d, sight::module::viz::scene3d::adaptor::negato);

    /// Creates the service and initializes slots.
    negato2d() noexcept;

    /// Destroys the adaptor.
    ~negato2d() noexcept final = default;

protected:

    /// Configures the service.
    void configuring(const config_t& _config) final;

    /**
     * @brief Sets the negato visibility.
     * @param _visible the visibility status of the negato.
     */
    void set_visible(bool _visible) final;

private:

    /**
     * @brief Interacts with the negato if it was picked by pressing any mouse button.
     *
     * Interactions will take place while holding down the button. The following actions are available:
     * - Left mouse click: shows a cross widget to select a voxel and retrieve its intensity.
     * @param _button mouse button pressed.
     * @param _x current width coordinate of the mouse cursor.
     * @param _y current height coordinate of the mouse cursor.
     * @param _dx the cursor's width displacement since the last event.
     * @param _dy the cursor's height displacement since the last event.
     */
    void mouse_move_event(mouse_button _button, modifier /*_mods*/, int _x, int _y, int _dx, int _dy) final;

    /**
     * @brief Attempts to pick the negato and starts interactions if picking was successful.
     * @param _button mouse button pressed.
     * @param _x current width coordinate of the mouse cursor.
     * @param _y current height coordinate of the mouse cursor.
     */
    void button_press_event(mouse_button _button, modifier /*_mods*/, int _x, int _y) final;

    /// Ends all interactions, regardless of the input.
    void button_release_event(mouse_button /*_button*/, modifier /*_mods*/, int /*_x*/, int /*_y*/) final;

    /// Sets the filtering type.
    void set_filtering(sight::viz::scene3d::plane::filter_t _filtering);

    /** Uploads the input image into the texture buffer and recomputes the negato geometry.
     * @param _new true if the image was reallocated, false if only pixel values changed.
     */
    void update_image(bool _new);

    /**
     * @brief SLOT: updates the image slice type.
     * @param _from origin of the orientation.
     * @param _to destination of the orientation.
     */
    void change_slice_type(int _from, int _to) override;

    /**
     * @brief Picks the intensity value at the (_x, _y) screen position.
     * @param _x current width coordinate of the mouse cursor.
     * @param _y current height coordinate of the mouse cursor.
     */
    void pick_intensity(int _x, int _y);

    /// True if the plane is being picked
    bool m_picked {false};
};

} // namespace sight::module::viz::scene3d::adaptor.

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

#include <data/string.hpp>

#include <viz/scene3d/picking_cross.hpp>
#include <viz/scene3d/plane.hpp>
#include <viz/scene3d/texture.hpp>
#include <viz/scene3d/transfer_function.hpp>

#include <module/viz/scene3d/adaptor/negato.hpp>

#include <OGRE/OgreManualObject.h>

#include <optional>

namespace sight::module::viz::scene3d::adaptor
{

/**
 * @brief This adaptor displays a 3D negato.
 *
 * @section Signals Signals
 * - \b picked_voxel(string): sends the coordinates and intensity of the voxel picked by the cross widget.
 *
 * @section Slots Slots
 * - \b update_image(): update the image display to show the new content.
 * - \b sliceType(int, int): update image slice index .
 * - \b sliceIndex(int, int, int): update image slice type.
 * - \b set_transparency(double): sets the global transparency of the three image planes.
 * - \b update_visibility(bool): sets whether the negato is shown or not.
 * - \b toggle_visibility(): toggle whether the negato is shown or not.
 * - \b show(): shows the negato.
 * - \b hide(): hides the negato.
 * - \b update_slices_from_world(double, double, double): updates image slices indexes according to a 3d world point
 * or landmark.
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service type="sight::module::viz::scene3d::adaptor::negato3d">
        <in key="image" uid="..." />
        <in key="mask" uid="..." />
        <inout key="tf" uid="..." />
        <config orientation="axial" filtering="none" tfAlpha="true" />
        <properties classification="pre" />
    </service>
   @endcode
 *
 * @subsection Input Input:
 * - \b image [sight::data::image]: image to display.
 * - \b mask [sight::data::image] (optional): mask to apply onto the image. Values < 0.5 are considered masked.
 *
 * @subsection In-Out In-Out:
 * - \b tf [sight::data::transfer_function]: the current TransferFunction. If it is not defined, we use the
 *      image's default transferFunction (CT-GreyLevel).
 *
 * @subsection Configuration Configuration:
 * - \b sliceIndex (optional, axial/frontal/sagittal, default=axial): orientation of the negato.
 * - \b filtering (optional, none/linear/anisotropic, default=none): texture filter type of the negato.
 * - \b tfAlpha (optional, bool, default=false): if true, the alpha channel of the transfer function is used.
 * - \b interactive (optional, bool, default=false): enables interactions on the negato.
 * - \b priority (optional, int, default=1): interaction priority of the negato.
 * - \b transform (optional, string, default=""): the name of the Ogre transform node where to attach the negato, as it
 *      was specified in the transform adaptor.
 * - \b query_flags (optional, uint32, default=0x40000000): Mask set to planes for picking request.
 * - \b border (optional, bool, default=true): allows to display plane borders.
 *
 * @subsection Properties Properties:
 * - \b classification (optional, pre/post, default=pre): classification of voxels. "pre" means the filtering is applied
 * after the sampling of the transfer function, and "post" after. When using labelled images, it is highly recommended
 * to use "pre", otherwise it is likely that class of objects can be confounded.
 * - \b visible (optional, bool, default=true): set the initial visibility of the 3D negato.
 * - \b depth_bias: useful to superimpose multiple negatos on top of each other
 */
class negato3d final : public sight::module::viz::scene3d::adaptor::negato
{
public:

    using axis_t = data::helper::medical_image::axis_t;

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(negato3d, sight::module::viz::scene3d::adaptor::negato);

    struct slots
    {
        static inline const core::com::slots::key_t SET_TRANSPARENCY = "set_transparency";
    };

    /// Creates slots.
    negato3d() noexcept;

    /// Destroys the adaptor.
    ~negato3d() noexcept final = default;

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
     * - Middle mouse click: move the slice's intersection to where the mouse cursor is.
     * - Right mouse click: adjust the transfer function level and window by moving
     *                      the mouse up/down and left/right respectively.
     *
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

    /**
     * @brief Sets the slice intersection at the (_x, _y) screen position if possible.
     * @param _x current width coordinate of the mouse cursor.
     * @param _y current height coordinate of the mouse cursor.
     */
    void move_slices(int _x, int _y);

    /**
     * @brief Picks the intensity value at the (_x, _y) screen position.
     * @param _x current width coordinate of the mouse cursor.
     * @param _y current height coordinate of the mouse cursor.
     */
    void pick_intensity(int _x, int _y);

    /** Uploads the input image into the texture buffer and recomputes the negato geometry.
     * @param _new true if the image was reallocated, false if only pixel values changed.
     */
    void update_image(bool _new);

    /// SLOT: updates the image slice type.
    void change_slice_type(int /*unused*/, int /*unused*/) override;

    /// SLOT: sets the planes's opacity.
    void set_transparency(double _transparency);

    /// Sets the picking flags on all three negato planes.
    void set_planes_query_flags(std::uint32_t _flags);

    /// Attemps to pick the negato planes, returns the world space position of the intersection if successful.
    std::optional<Ogre::Vector3> get_picked_slices(int _x, int _y);
};

//------------------------------------------------------------------------------

} // namespace sight::module::viz::scene3d::adaptor.

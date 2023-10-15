/************************************************************************
 *
 * Copyright (C) 2020-2023 IRCAD France
 * Copyright (C) 2020 IHU Strasbourg
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

#include "modules/viz/scene3d/config.hpp"

#include <data/helper/MedicalImage.hpp>

#include <viz/scene3d/adaptor.hpp>

namespace sight::module::viz::scene3d::adaptor
{

/**
 * @brief This adaptor performs picking queries on an image voxel using slices.
 *
 * This picker has a different behaviour than module::viz::scene3d::adaptor::picker, in fact,
 * module::viz::scene3d::adaptor::picker performs
 * a picking on all scene meshes. This one picks only on an image and computes exactly the intersection between the
 * picking coordinates, and image slices.
 *
 * In 3D mode, it's useless to connect data::image::SLICE_TYPE_MODIFIED_SIG of s_IMAGE_INPUT to SLICETYPE_SLOT.
 * (auto connection to true), it's only used in 2D mode.
 *
 * @section Signals Signals
 * - \b picked(data::tools::picking_info): emitted when a picking query is successful.
 *
 * @section Slots Slots
 * - \b sliceType(int, int): updates image slice index .
 *
 * @section XML XML Configuration
 * @code{.xml}
        <service type="sight::module::viz::scene3d::adaptor::voxel_picker">
            <in key="image" uid="..." />
            <config priority="2" orientation="sagittal" mode="2D" layerOrderDependant="true"
                moveOnPick="false" />
       </service>
   @endcode
 *
 * @subsection Input Input:
 * - \b image [sight::data::image]: image to pick.
 *
 * @subsection Configuration Configuration:
 * - \b priority (optional, int, default=0): picking priority, higher priority interactions are performed first.
 * - \b layerOrderDependant (optional, bool, default=true): define if interaction must take into account above layers.
 * - \b orientation (optional, sagittal/frontal/axial, default=sagittal): orientation of the image.
 * - \b mode (optional, 2D/3D, default=2D): in 2D, the ray is intersected with the current image slice.
 *                                          In 3D, the ray intersects each slices and returns the nearest one.
 * - \b moveOnPick (optional, bool, default=false): move image slices indexes on the picked position.
 */
class MODULE_VIZ_SCENE3D_CLASS_API voxel_picker final :
    public sight::viz::scene3d::adaptor,
    public sight::viz::scene3d::interactor::base
{
public:

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(voxel_picker, sight::viz::scene3d::adaptor);

    /// Initializes the adaptor.
    MODULE_VIZ_SCENE3D_API voxel_picker() noexcept;

    /// Destroys the adaptor.
    MODULE_VIZ_SCENE3D_API ~voxel_picker() noexcept final = default;

    /// Runs a picking query when a mouse button is pressed.
    MODULE_VIZ_SCENE3D_API void buttonPressEvent(MouseButton _button, Modifier _mod, int _x, int _y) final;

    /// Runs a picking query when a mouse button is released.
    MODULE_VIZ_SCENE3D_API void buttonReleaseEvent(MouseButton _button, Modifier _mod, int _x, int _y) final;

protected:

    /// Configures the picker.
    MODULE_VIZ_SCENE3D_API void configuring() final;

    /// Adds the interactor to the layer.
    MODULE_VIZ_SCENE3D_API void starting() final;

    /**
     * @brief Proposals to connect service slots to associated object signals.
     * @return A map of each proposed connection.
     *
     * Connect data::image::SLICE_TYPE_MODIFIED_SIG of s_IMAGE_INPUT to SLICETYPE_SLOT
     */
    MODULE_VIZ_SCENE3D_API service::connections_t auto_connections() const final;

    /// Does nothing.
    MODULE_VIZ_SCENE3D_API void updating() noexcept final;

    /// Removes the interactor from the layer.
    MODULE_VIZ_SCENE3D_API void stopping() final;

private:

    /// Runs a picking query.
    void pick(MouseButton _button, Modifier _mod, int _x, int _y, bool _pressed);

    /// Defines the orientation of the image.
    typedef data::helper::MedicalImage::orientation_t OrientationMode;

    /**
     * @brief SLOT: updates the image slice type.
     * @param _from origin of the orientation.
     * @param _to destination of the orientation.
     */
    void changeSliceType(int _from, int _to);

    /**
     * @brief Computes the intersection between the given ray, and the given image.
     *
     * The computed intersection depends on the picker mode, if it's 2D, the ray is intersected with the current image
     * slice. In 3D, the ray intersects each slices and returns the nearest one.
     *
     * @param _ray the ray used to compute the intersection.
     * @param _image used to retrieve current slices index.
     * @param _origin image origin.
     * @param _spacing image spacing.
     */
    std::pair<bool, Ogre::Vector3> computeRayImageIntersection(
        const Ogre::Ray& _ray,
        const data::image::csptr _image,
        const Ogre::Vector3& _origin,
        const Ogre::Vector3& _spacing
    );

    /// Defines the orientation of the image used to pick on the current slice.
    OrientationMode m_orientation {OrientationMode::X_AXIS};

    /// Defines the picker mode.
    bool m_mode2D {true};

    /// Determines the execution order of the picking interactor.
    int m_priority {2};

    /// Defines if the interaction must take into account above layers.
    bool m_layerOrderDependant {true};

    /// Defines if the image slices indexes will be updated with the picked position.
    bool m_moveOnPick {false};

    /// Defines the signal sent on picking events.
    core::com::signal<void(data::tools::picking_info)>::sptr m_pickedSig;

    static constexpr std::string_view s_IMAGE_INPUT = "image";
    sight::data::ptr<sight::data::image, sight::data::Access::in> m_image {this, s_IMAGE_INPUT};
};

} // namespace sight::module::viz::scene3d::adaptor.

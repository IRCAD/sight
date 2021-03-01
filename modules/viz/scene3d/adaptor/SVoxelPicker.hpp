/************************************************************************
 *
 * Copyright (C) 2020-2021 IRCAD France
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

#include <viz/scene3d/IAdaptor.hpp>

namespace sight::module::viz::scene3d::adaptor
{

/**
 * @brief This adaptor performs picking queries on an image voxel using slices.
 *
 * This picker has a different behaviour than module::viz::scene3d::adaptor::SPicker, in fact,
 * module::viz::scene3d::adaptor::SPicker performs
 * a picking on all scene meshes. This one picks only on an image and computes exactly the intersection between the
 * picking coordinates, and image slices.
 *
 * In 3D mode, it's useless to connect data::Image::s_SLICE_TYPE_MODIFIED_SIG of s_IMAGE_INPUT to s_SLICETYPE_SLOT.
 * (auto connection to true), it's only used in 2D mode.
 *
 * @section Signals Signals
 * - \b picked(data::tools::PickingInfo): emitted when a picking query is successful.
 *
 * @section Slots Slots
 * - \b sliceType(int, int): updates image slice index .
 *
 * @section XML XML Configuration
 * @code{.xml}
        <service type="::sight::module::viz::scene3d::adaptor::SVoxelPicker">
            <in key="image" uid="..." />
            <config layer="..." priority="2" orientation="sagittal" mode="2D" layerOrderDependant="true"
                moveOnPick="false" />
       </service>
   @endcode
 *
 * @subsection Input Input:
 * - \b image [sight::data::Image]: image to pick.
 *
 * @subsection Configuration Configuration:
 * - \b layer (mandatory, string): layer on which the adaptor picks points.
 * - \b priority (optional, int, default=0): picking priority, higher priority interactions are performed first.
 * - \b layerOrderDependant (optional, bool, default=true): define if interaction must take into account above layers.
 * - \b orientation (optional, sagittal/frontal/axial, default=sagittal): orientation of the image.
 * - \b mode (optional, 2D/3D, default=2D): in 2D, the ray is intersected with the current image slice.
 *                                          In 3D, the ray intersects each slices and returns the nearest one.
 * - \b moveOnPick (optional, bool, default=false): move image slices indexes on the picked position.
 */
class MODULE_VIZ_SCENE3D_CLASS_API SVoxelPicker final :
    public sight::viz::scene3d::IAdaptor,
    public sight::viz::scene3d::interactor::IInteractor
{

public:

    /// Generates default methods as New, dynamicCast, ...
    fwCoreServiceMacro(SVoxelPicker, ::sight::viz::scene3d::IAdaptor)

    /// Initializes the adaptor.
    MODULE_VIZ_SCENE3D_API SVoxelPicker() noexcept;

    /// Destroys the adaptor.
    MODULE_VIZ_SCENE3D_API ~SVoxelPicker() noexcept override;

    /// Runs a picking query when a mouse button is pressed.
    MODULE_VIZ_SCENE3D_API void buttonPressEvent(MouseButton _button, Modifier _mod, int _x, int _y) override;

    /// Runs a picking query when a mouse button is released.
    MODULE_VIZ_SCENE3D_API void buttonReleaseEvent(MouseButton _button, Modifier _mod, int _x, int _y) override;

protected:

    /// Configures the picker.
    MODULE_VIZ_SCENE3D_API void configuring() override;

    /// Adds the interactor to the layer.
    MODULE_VIZ_SCENE3D_API void starting() override;

    /**
     * @brief Proposals to connect service slots to associated object signals.
     * @return A map of each proposed connection.
     *
     * Connect data::Image::s_SLICE_TYPE_MODIFIED_SIG of s_IMAGE_INPUT to s_SLICETYPE_SLOT
     */
    MODULE_VIZ_SCENE3D_API service::IService::KeyConnectionsMap getAutoConnections() const override;

    /// Does nothing.
    MODULE_VIZ_SCENE3D_API void updating() noexcept override;

    /// Removes the interactor from the layer.
    MODULE_VIZ_SCENE3D_API void stopping() override;

private:

    /// Runs a picking query.
    void pick(MouseButton _button, Modifier _mod, int _x, int _y, bool _pressed);

    /// Defines the orientation of the image.
    typedef data::helper::MedicalImage::Orientation OrientationMode;

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
    std::pair< bool, ::Ogre::Vector3 > computeRayImageIntersection(const ::Ogre::Ray& _ray,
                                                                   const data::Image::csptr _image,
                                                                   const ::Ogre::Vector3& _origin,
                                                                   const ::Ogre::Vector3& _spacing);

    /// Defines the orientation of the image used to pick on the current slice.
    OrientationMode m_orientation;

    /// Defines the picker mode.
    bool m_mode2D { true };

    /// Determines the execution order of the picking interactor.
    int m_priority { 2 };

    /// Defines if the interaction must take into account above layers.
    bool m_layerOrderDependant { true };

    /// Defines if the image slices indexes will be updated with the picked position.
    bool m_moveOnPick { false };

    /// Defines the signal sent on picking events.
    core::com::Signal< void ( data::tools::PickingInfo ) >::sptr m_pickedSig;

};

} // namespace sight::module::viz::scene3d::adaptor.

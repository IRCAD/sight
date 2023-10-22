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

#include "modules/viz/scene3d/adaptor/voxel_picker.hpp"

#include <core/com/signal.hxx>
#include <core/com/signals.hpp>
#include <core/com/slots.hxx>

#include <data/helper/medical_image.hpp>

#include <viz/scene3d/helper/camera.hpp>
#include <viz/scene3d/utils.hpp>

namespace sight::module::viz::scene3d::adaptor
{

const core::com::slots::key_t SLICETYPE_SLOT = "sliceType";

static const core::com::signals::key_t PICKED_SIG = "picked";

//-----------------------------------------------------------------------------

voxel_picker::voxel_picker() noexcept
{
    new_slot(SLICETYPE_SLOT, &voxel_picker::changeSliceType, this);

    m_pickedSig = new_signal<core::com::signal<void(data::tools::picking_info)> >(PICKED_SIG);
}

//-----------------------------------------------------------------------------

void voxel_picker::configuring()
{
    this->configureParams();

    const config_t config = this->get_config();

    static const std::string s_PRIORITY_CONFIG              = s_CONFIG + "priority";
    static const std::string s_ORIENTATION_CONFIG           = s_CONFIG + "orientation";
    static const std::string s_MODE_CONFIG                  = s_CONFIG + "mode";
    static const std::string s_LAYER_ORDER_DEPENDANT_CONFIG = s_CONFIG + "layerOrderDependant";
    static const std::string s_MOVE_ON_PICK_CONFIG          = s_CONFIG + "moveOnPick";

    m_priority            = config.get<int>(s_PRIORITY_CONFIG, m_priority);
    m_layerOrderDependant = config.get<bool>(s_LAYER_ORDER_DEPENDANT_CONFIG, m_layerOrderDependant);

    const std::string orientation = config.get<std::string>(s_ORIENTATION_CONFIG, "sagittal");
    SIGHT_ASSERT(
        "Orientation mode must be 'axial', 'frontal' or 'sagittal'.",
        orientation == "axial" || orientation == "frontal" || orientation == "sagittal"
    );
    if(orientation == "axial")
    {
        m_orientation = OrientationMode::Z_AXIS;
    }
    else if(orientation == "frontal")
    {
        m_orientation = OrientationMode::Y_AXIS;
    }
    else if(orientation == "sagittal")
    {
        m_orientation = OrientationMode::X_AXIS;
    }

    const std::string mode = config.get<std::string>(s_MODE_CONFIG, "2D");
    SIGHT_ASSERT("Mode must be '2D' or '3D'.", mode == "2D" || mode == "3D");
    m_mode2D = mode == "2D";

    m_moveOnPick = config.get<bool>(s_MOVE_ON_PICK_CONFIG, m_moveOnPick);
}

//-----------------------------------------------------------------------------

void voxel_picker::starting()
{
    this->initialize();

    const auto interactor = std::dynamic_pointer_cast<sight::viz::scene3d::interactor::base>(this->get_sptr());
    this->getLayer()->addInteractor(interactor, m_priority);
}

//-----------------------------------------------------------------------------

service::connections_t voxel_picker::auto_connections() const
{
    service::connections_t connections;
    connections.push(s_IMAGE_INPUT, data::image::SLICE_TYPE_MODIFIED_SIG, SLICETYPE_SLOT);

    return connections;
}

//-----------------------------------------------------------------------------

void voxel_picker::updating() noexcept
{
}

//-----------------------------------------------------------------------------

void voxel_picker::stopping()
{
    const auto interactor = std::dynamic_pointer_cast<sight::viz::scene3d::interactor::base>(this->get_sptr());
    this->getLayer()->removeInteractor(interactor);
}

//-----------------------------------------------------------------------------

void voxel_picker::buttonPressEvent(MouseButton _button, Modifier _mod, int _x, int _y)
{
    this->pick(_button, _mod, _x, _y, true);
}

//-----------------------------------------------------------------------------

void voxel_picker::buttonReleaseEvent(MouseButton _button, Modifier _mod, int _x, int _y)
{
    this->pick(_button, _mod, _x, _y, false);
}

//-----------------------------------------------------------------------------

void voxel_picker::pick(MouseButton _button, Modifier _mod, int _x, int _y, bool _pressed)
{
    if(_button == MouseButton::LEFT)
    {
        if(auto layer = m_layer.lock())
        {
            if(!sight::module::viz::scene3d::adaptor::voxel_picker::isInLayer(_x, _y, layer, m_layerOrderDependant))
            {
                return;
            }
        }

        // Get the scene manager.
        const auto* const scene_manager = this->getSceneManager();

        // Create the ray from picking positions.
        const auto* const camera = scene_manager->getCamera(sight::viz::scene3d::layer::s_DEFAULT_CAMERA_NAME);
        const auto vp_pos        =
            sight::viz::scene3d::helper::camera::convertFromWindowToViewportSpace(*camera, _x, _y);
        const Ogre::Ray vp_ray = camera->getCameraToViewportRay(vp_pos.x, vp_pos.y);

        // Get image information.
        const auto image = m_image.lock();
        const auto [spacing, origin] = sight::viz::scene3d::utils::convertSpacingAndOrigin(image.get_shared());

        const std::pair<bool, Ogre::Vector3> result =
            this->computeRayImageIntersection(vp_ray, image.get_shared(), origin, spacing);

        if(result.first)
        {
            // Compute the ray/slice intersection.
            const Ogre::Vector3 intersection = result.second;

            // Create the picking information.
            data::tools::picking_info info;
            info.m_worldPos[0] = static_cast<double>(intersection.x);
            info.m_worldPos[1] = static_cast<double>(intersection.y);
            info.m_worldPos[2] = static_cast<double>(intersection.z);

            using picking_event_t = data::tools::picking_info::Event;
            switch(_button)
            {
                case MouseButton::LEFT:
                    info.m_eventId = _pressed ? picking_event_t::MOUSE_LEFT_DOWN : picking_event_t::MOUSE_LEFT_UP;
                    break;

                case MouseButton::RIGHT:
                    info.m_eventId = _pressed ? picking_event_t::MOUSE_RIGHT_DOWN : picking_event_t::MOUSE_RIGHT_UP;
                    break;

                case MouseButton::MIDDLE:
                    info.m_eventId = _pressed ? picking_event_t::MOUSE_MIDDLE_DOWN : picking_event_t::MOUSE_MIDDLE_UP;
                    break;

                default:
                    SIGHT_ERROR("Unknown mouse button");
                    break;
            }

            if(static_cast<bool>(_mod & Modifier::CONTROL))
            {
                info.m_modifierMask |= data::tools::picking_info::CTRL;

                if(m_moveOnPick && info.m_eventId == data::tools::picking_info::Event::MOUSE_LEFT_UP)
                {
                    // Emit slices positions
                    const int sagittal_idx = static_cast<int>((info.m_worldPos[0] - origin[0]) / spacing[0]);
                    const int frontal_idx  = static_cast<int>((info.m_worldPos[1] - origin[1]) / spacing[1]);
                    const int axial_idx    = static_cast<int>((info.m_worldPos[2] - origin[2]) / spacing[2]);
                    const auto sig         = image->signal<data::image::SliceIndexModifiedSignalType>(
                        data::image::SLICE_INDEX_MODIFIED_SIG
                    );
                    sig->async_emit(axial_idx, frontal_idx, sagittal_idx);
                }
            }

            if(static_cast<bool>(_mod & Modifier::SHIFT))
            {
                info.m_modifierMask |= data::tools::picking_info::SHIFT;
            }

            // Emit the picking info.
            m_pickedSig->async_emit(info);

            // Cancel further interactors on the same layer.
            this->getLayer()->cancelFurtherInteraction();
        }
    }
}

//-----------------------------------------------------------------------------

void voxel_picker::changeSliceType(int _from, int _to)
{
    const auto to_orientation   = static_cast<OrientationMode>(_to);
    const auto from_orientation = static_cast<OrientationMode>(_from);

    m_orientation = m_orientation == to_orientation ? from_orientation
                                                    : m_orientation
                    == from_orientation ? to_orientation : m_orientation;
}

//-----------------------------------------------------------------------------

std::pair<bool, Ogre::Vector3> voxel_picker::computeRayImageIntersection(
    const Ogre::Ray& _ray,
    const data::image::csptr _image,
    const Ogre::Vector3& _origin,
    const Ogre::Vector3& _spacing
)
{
    namespace imHelper = data::helper::medical_image;
    const auto axial_idx    = imHelper::get_slice_index(*_image, imHelper::orientation_t::AXIAL).value_or(0);
    const auto frontal_idx  = imHelper::get_slice_index(*_image, imHelper::orientation_t::FRONTAL).value_or(0);
    const auto sagittal_idx = imHelper::get_slice_index(*_image, imHelper::orientation_t::SAGITTAL).value_or(0);

    const auto axial_index    = static_cast<Ogre::Real>(axial_idx);
    const auto frontal_index  = static_cast<Ogre::Real>(frontal_idx);
    const auto sagittal_index = static_cast<Ogre::Real>(sagittal_idx);

    const auto size = _image->size();

    // Function to check if an intersection is inside an image.
    std::function<bool(OrientationMode, Ogre::Vector3)> is_inside_image =
        [&](OrientationMode _orientation, const Ogre::Vector3 _inter) -> bool
        {
            switch(_orientation)
            {
                case OrientationMode::X_AXIS:
                    return _inter.y >= _origin.y
                           && _inter.z >= _origin.z
                           && _inter.y <= _origin.y + _spacing.y * static_cast<Ogre::Real>(size[1])
                           && _inter.z <= _origin.z + _spacing.z * static_cast<Ogre::Real>(size[2]);

                case OrientationMode::Y_AXIS:
                    return _inter.x >= _origin.x
                           && _inter.z >= _origin.z
                           && _inter.x <= _origin.x + _spacing.x * static_cast<Ogre::Real>(size[0])
                           && _inter.z <= _origin.z + _spacing.z * static_cast<Ogre::Real>(size[2]);

                case OrientationMode::Z_AXIS:
                    return _inter.x >= _origin.x
                           && _inter.y >= _origin.y
                           && _inter.x <= _origin.x + _spacing.x * static_cast<Ogre::Real>(size[0])
                           && _inter.y <= _origin.y + _spacing.y * static_cast<Ogre::Real>(size[1]);

                default:
                    SIGHT_ERROR("Unknown orientation mode");
                    return false;
            }
        };

    // Function to cast the non depth coordinate into image spacing.
    std::function<Ogre::Vector3(OrientationMode, Ogre::Vector3)> cast_to_voxel =
        [&](OrientationMode _orientation, Ogre::Vector3 _inter) -> Ogre::Vector3
        {
            switch(_orientation)
            {
                case OrientationMode::X_AXIS:
                {
                    const int y_idx = static_cast<int>(_inter.y / _spacing.y);
                    const int z_idx = static_cast<int>(_inter.z / _spacing.z);
                    _inter.y = static_cast<float>(y_idx) * _spacing.y;
                    _inter.z = static_cast<float>(z_idx) * _spacing.z;
                    break;
                }

                case OrientationMode::Y_AXIS:
                {
                    const int x_idx = static_cast<int>(_inter.x / _spacing.x);
                    const int z_idx = static_cast<int>(_inter.z / _spacing.z);
                    _inter.x = static_cast<float>(x_idx) * _spacing.x;
                    _inter.z = static_cast<float>(z_idx) * _spacing.z;
                    break;
                }

                case OrientationMode::Z_AXIS:
                {
                    const int x_idx = static_cast<int>(_inter.x / _spacing.x);
                    const int y_idx = static_cast<int>(_inter.y / _spacing.y);
                    _inter.x = static_cast<float>(x_idx) * _spacing.x;
                    _inter.y = static_cast<float>(y_idx) * _spacing.y;
                    break;
                }

                default:
                    SIGHT_ERROR("Unknown orientation mode");
                    break;
            }

            return _inter;
        };

    // If it's a 2D mode, the intersection is computed between the ray and the current image slice.
    if(m_mode2D)
    {
        // Create the plane from image information.
        Ogre::Plane plane;
        switch(m_orientation)
        {
            case OrientationMode::X_AXIS:
                plane = Ogre::Plane(Ogre::Vector3::UNIT_X, _origin.x + sagittal_index * _spacing.x);
                break;

            case OrientationMode::Y_AXIS:
                plane = Ogre::Plane(Ogre::Vector3::UNIT_Y, _origin.y + frontal_index * _spacing.y);
                break;

            case OrientationMode::Z_AXIS:
                plane = Ogre::Plane(Ogre::Vector3::UNIT_Z, _origin.z + axial_index * _spacing.z);
                break;

            default:
                SIGHT_ERROR("Unknown orientation mode");
                break;
        }

        // Check the intersection.
        const Ogre::RayTestResult result = _ray.intersects(plane);

        if(result.first)
        {
            Ogre::Vector3 intersection = _ray.getPoint(result.second);
            return std::make_pair(
                is_inside_image(m_orientation, intersection),
                cast_to_voxel(m_orientation, intersection)
            );
        }
    }
    // Else, the intersection is computed between each slice. The nearest one is returned.
    else
    {
        const Ogre::Plane sagittal_plane =
            Ogre::Plane(Ogre::Vector3::UNIT_X, _origin.x + sagittal_index * _spacing.x);
        const Ogre::Plane frontal_plane =
            Ogre::Plane(Ogre::Vector3::UNIT_Y, _origin.y + frontal_index * _spacing.y);
        const Ogre::Plane axial_plane = Ogre::Plane(Ogre::Vector3::UNIT_Z, _origin.z + axial_index * _spacing.z);

        Ogre::RayTestResult sagittal_inter = _ray.intersects(sagittal_plane);
        Ogre::RayTestResult frontal_inter  = _ray.intersects(frontal_plane);
        Ogre::RayTestResult axial_inter    = _ray.intersects(axial_plane);

        if(sagittal_inter.first)
        {
            Ogre::Vector3 intersection = _ray.getPoint(sagittal_inter.second);
            sagittal_inter.first = is_inside_image(OrientationMode::X_AXIS, intersection);
        }

        if(frontal_inter.first)
        {
            Ogre::Vector3 intersection = _ray.getPoint(frontal_inter.second);
            frontal_inter.first = is_inside_image(OrientationMode::Y_AXIS, intersection);
        }

        if(axial_inter.first)
        {
            Ogre::Vector3 intersection = _ray.getPoint(axial_inter.second);
            axial_inter.first = is_inside_image(OrientationMode::Z_AXIS, intersection);
        }

        OrientationMode orientation = OrientationMode::X_AXIS;
        Ogre::RayTestResult result  = std::make_pair(false, std::numeric_limits<Ogre::Real>::max());
        if(sagittal_inter.first)
        {
            orientation   = OrientationMode::X_AXIS;
            result.second = sagittal_inter.second;
            result.first  = true;
        }

        if(frontal_inter.first && frontal_inter.second < result.second)
        {
            orientation   = OrientationMode::Y_AXIS;
            result.second = frontal_inter.second;
            result.first  = true;
        }

        if(axial_inter.first && axial_inter.second < result.second)
        {
            orientation   = OrientationMode::Z_AXIS;
            result.second = axial_inter.second;
            result.first  = true;
        }

        if(result.first)
        {
            const Ogre::Vector3 intersection = _ray.getPoint(result.second);
            return std::make_pair(true, cast_to_voxel(orientation, intersection));
        }
    }

    return std::make_pair(false, Ogre::Vector3 {0.0, 0.0, 0.0});
}

//-----------------------------------------------------------------------------

} // namespace sight::module::viz::scene3d::adaptor.

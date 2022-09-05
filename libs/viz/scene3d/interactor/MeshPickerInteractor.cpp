/************************************************************************
 *
 * Copyright (C) 2014-2022 IRCAD France
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

#include "viz/scene3d/interactor/MeshPickerInteractor.hpp"

#include <core/com/Signal.hxx>

#include <viz/scene3d/interactor/IInteractor.hpp>
#include <viz/scene3d/Layer.hpp>
#include <viz/scene3d/registry/macros.hpp>

namespace sight::viz::scene3d::interactor
{

MeshPickerInteractor::MeshPickerInteractor(Layer::sptr _layer, bool _layerOrderDependant) noexcept :
    IInteractor(_layer, _layerOrderDependant)
{
    if(_layer)
    {
        m_picker.setSceneManager(_layer->getSceneManager());
    }
}

//------------------------------------------------------------------------------

MeshPickerInteractor::~MeshPickerInteractor() noexcept =
    default;

//------------------------------------------------------------------------------

void MeshPickerInteractor::setPointClickedSig(const PointClickedSigType::sptr& _sig)
{
    m_pointClickedSig = _sig;
}

//------------------------------------------------------------------------------

void MeshPickerInteractor::setQueryMask(std::uint32_t _queryMask)
{
    m_queryMask = _queryMask;
}

//------------------------------------------------------------------------------

void MeshPickerInteractor::pick(MouseButton _button, Modifier _mod, int _x, int _y, bool _pressed)
{
    if(m_picker.hasSceneManager())
    {
        if(auto layer = m_layer.lock())
        {
            if(!isInLayer(_x, _y, layer, m_layerOrderDependant))
            {
                return;
            }
        }

        if(m_picker.executeRaySceneQuery(_x, _y, m_queryMask))
        {
            Ogre::Vector3 click = m_picker.getIntersectionInWorldSpace();

            data::tools::PickingInfo info;
            info.m_worldPos[0] = static_cast<double>(click.x);
            info.m_worldPos[1] = static_cast<double>(click.y);
            info.m_worldPos[2] = static_cast<double>(click.z);

            using PickingEventType = data::tools::PickingInfo::Event;
            switch(_button)
            {
                case MouseButton::LEFT:
                    info.m_eventId = _pressed ? PickingEventType::MOUSE_LEFT_DOWN : PickingEventType::MOUSE_LEFT_UP;
                    break;

                case MouseButton::RIGHT:
                    info.m_eventId = _pressed ? PickingEventType::MOUSE_RIGHT_DOWN : PickingEventType::MOUSE_RIGHT_UP;
                    break;

                case MouseButton::MIDDLE:
                    info.m_eventId = _pressed ? PickingEventType::MOUSE_MIDDLE_DOWN : PickingEventType::MOUSE_MIDDLE_UP;
                    break;

                default:
                    SIGHT_ERROR("Unknow button");
                    break;
            }

            if(static_cast<bool>(_mod & Modifier::CONTROL))
            {
                info.m_modifierMask |= data::tools::PickingInfo::CTRL;
            }

            if(static_cast<bool>(_mod & Modifier::SHIFT))
            {
                info.m_modifierMask |= data::tools::PickingInfo::SHIFT;
            }

            if(m_pointClickedSig)
            {
                m_pointClickedSig->asyncEmit(info);
            }
            else
            {
                SIGHT_ERROR(
                    "You must first set the signal sent using 'MeshPickerInteractor::setPointClickedSig'"
                    " for this interactor to work."
                );
            }
        }
    }
    else
    {
        SIGHT_ERROR("The picker scene hasn't been initialized, you are not using this interactor correctly");
    }
}

//------------------------------------------------------------------------------

void MeshPickerInteractor::buttonPressEvent(MouseButton _button, Modifier _mod, int _x, int _y)
{
    this->pick(_button, _mod, _x, _y, true);
}

//------------------------------------------------------------------------------

void MeshPickerInteractor::buttonReleaseEvent(MouseButton _button, Modifier _mod, int _x, int _y)
{
    this->pick(_button, _mod, _x, _y, false);
}

//------------------------------------------------------------------------------

} //namespace sight::viz::scene3d::interactor.

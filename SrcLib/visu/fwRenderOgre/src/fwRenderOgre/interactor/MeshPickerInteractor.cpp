/************************************************************************
 *
 * Copyright (C) 2014-2019 IRCAD France
 * Copyright (C) 2014-2019 IHU Strasbourg
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

#include "fwRenderOgre/interactor/MeshPickerInteractor.hpp"

#include <fwRenderOgre/interactor/IInteractor.hpp>
#include <fwRenderOgre/Layer.hpp>
#include <fwRenderOgre/registry/macros.hpp>

#include <fwCom/Signal.hxx>

fwRenderOgreRegisterInteractorMacro( ::fwRenderOgre::interactor::MeshPickerInteractor );

namespace fwRenderOgre::interactor
{

MeshPickerInteractor::MeshPickerInteractor(Layer::sptr _layer) noexcept :
    IInteractor(_layer)
{
    if(_layer)
    {
        m_picker.setSceneManager(_layer->getSceneManager());
    }
}

//------------------------------------------------------------------------------

MeshPickerInteractor::~MeshPickerInteractor() noexcept
{
}

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
        if(m_picker.executeRaySceneQuery(_x, _y, m_width, m_height, m_queryMask))
        {
            ::Ogre::Vector3 click = m_picker.getIntersectionInWorldSpace();

            ::fwDataTools::PickingInfo info;
            info.m_worldPos[0] = static_cast<double>(click.x);
            info.m_worldPos[1] = static_cast<double>(click.y);
            info.m_worldPos[2] = static_cast<double>(click.z);

            using PickingEventType = ::fwDataTools::PickingInfo::Event;
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
                    SLM_ERROR("Unknow button");
                    break;
            }

            if(static_cast<bool>(_mod & Modifier::CONTROL))
            {
                info.m_modifierMask |= ::fwDataTools::PickingInfo::CTRL;
            }
            if(static_cast<bool>(_mod & Modifier::SHIFT))
            {
                info.m_modifierMask |= ::fwDataTools::PickingInfo::SHIFT;
            }

            if(m_pointClickedSig)
            {
                m_pointClickedSig->asyncEmit(info);
            }
            else
            {
                SLM_ERROR("You must first set the signal sent using 'MeshPickerInteractor::setPointClickedSig'"
                          " for this interactor to work.");
            }
        }
    }
    else
    {
        SLM_ERROR("The picker scene hasn't been initialized, you are not using this interactor correctly");
    }
}

//------------------------------------------------------------------------------

void MeshPickerInteractor::resizeEvent(int _x, int _y)
{
    m_width  = _x;
    m_height = _y;
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

} //namespace fwRenderOgre::interactor

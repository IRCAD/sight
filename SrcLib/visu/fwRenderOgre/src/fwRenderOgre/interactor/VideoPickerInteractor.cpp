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

#include <fwRenderOgre/interactor/IInteractor.hpp>
#include <fwRenderOgre/interactor/VideoPickerInteractor.hpp>
#include <fwRenderOgre/registry/macros.hpp>

#include <fwCom/Signal.hxx>

fwRenderOgreRegisterInteractorMacro( ::fwRenderOgre::interactor::VideoPickerInteractor );

namespace fwRenderOgre
{

namespace interactor
{

VideoPickerInteractor::VideoPickerInteractor() noexcept
{
}

//------------------------------------------------------------------------------

VideoPickerInteractor::~VideoPickerInteractor() noexcept
{
}

//------------------------------------------------------------------------------

void VideoPickerInteractor::resizeEvent(int _x, int _y)
{
    m_width  = _x;
    m_height = _y;
}

//------------------------------------------------------------------------------

void VideoPickerInteractor::buttonPressEvent(MouseButton _button, int _x, int _y)
{
    if(m_picker->hasSceneManager())
    {
        if(m_picker->executeRaySceneQuery(_x, _y, m_width, m_height, m_queryFlags))
        {
            ::Ogre::Vector3 click = m_picker->getIntersectionInWorldSpace();

            ::fwDataTools::PickingInfo info;
            info.m_worldPos[0] = static_cast<double>(click.x);
            info.m_worldPos[1] = static_cast<double>(click.y);
            info.m_worldPos[2] = static_cast<double>(click.z);

            switch(_button)
            {
                case MouseButton::LEFT:
                    info.m_eventId = ::fwDataTools::PickingInfo::Event::MOUSE_LEFT_DOWN;
                    break;
                case MouseButton::RIGHT:
                    info.m_eventId = ::fwDataTools::PickingInfo::Event::MOUSE_RIGHT_DOWN;
                    break;
                case MouseButton::MIDDLE:
                    info.m_eventId = ::fwDataTools::PickingInfo::Event::MOUSE_MIDDLE_DOWN;
                    break;
                default:
                    SLM_ERROR("Unknow button");
                    break;
            }

            if(m_control)
            {
                info.m_modifierMask |= ::fwDataTools::PickingInfo::CTRL;
            }

            m_picked->asyncEmit(info);
        }
    }
    else
    {
        SLM_WARN("The picker scene hasn't been initialized, you are not using this interactor correctly");
    }
}

//------------------------------------------------------------------------------

void VideoPickerInteractor::mouseMoveEvent(MouseButton, int, int, int, int)
{
}

//------------------------------------------------------------------------------

void VideoPickerInteractor::wheelEvent(int, int, int)
{
}

//------------------------------------------------------------------------------

void VideoPickerInteractor::buttonReleaseEvent(MouseButton _button, int _x, int _y)
{
    if(m_picker->hasSceneManager())
    {
        if(m_picker->executeRaySceneQuery(_x, _y, m_width, m_height, m_queryFlags))
        {
            ::Ogre::Vector3 click = m_picker->getIntersectionInWorldSpace();

            ::fwDataTools::PickingInfo info;
            info.m_worldPos[0] = static_cast<double>(click.x);
            info.m_worldPos[1] = static_cast<double>(click.y);
            info.m_worldPos[2] = static_cast<double>(click.z);

            switch(_button)
            {
                case MouseButton::LEFT:
                    info.m_eventId = ::fwDataTools::PickingInfo::Event::MOUSE_LEFT_UP;
                    break;
                case MouseButton::RIGHT:
                    info.m_eventId = ::fwDataTools::PickingInfo::Event::MOUSE_RIGHT_UP;
                    break;
                case MouseButton::MIDDLE:
                    info.m_eventId = ::fwDataTools::PickingInfo::Event::MOUSE_MIDDLE_UP;
                    break;
                default:
                    SLM_ERROR("Unknow button");
                    break;
            }

            if(m_control)
            {
                info.m_modifierMask |= ::fwDataTools::PickingInfo::CTRL;
            }

            m_picked->asyncEmit(info);
        }
    }
    else
    {
        SLM_WARN("The picker scene hasn't been initialized, you are not using this interactor correctly");
    }
}

//------------------------------------------------------------------------------

void VideoPickerInteractor::keyPressEvent(int _k)
{
    if(_k == Modifier::CONTROL)
    {
        m_control = true;
    }
}

//------------------------------------------------------------------------------

void VideoPickerInteractor::keyReleaseEvent(int _k)
{
    if(_k == Modifier::CONTROL)
    {
        m_control = false;
    }
}

//------------------------------------------------------------------------------

void VideoPickerInteractor::focusInEvent()
{
}

//------------------------------------------------------------------------------

void VideoPickerInteractor::focusOutEvent()
{
    m_control = false;
}

//------------------------------------------------------------------------------

} //namespace interactor
} //namespace fwRenderOgre

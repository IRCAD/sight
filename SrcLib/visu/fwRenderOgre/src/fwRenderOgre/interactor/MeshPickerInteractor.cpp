/************************************************************************
 *
 * Copyright (C) 2014-2018 IRCAD France
 * Copyright (C) 2014-2018 IHU Strasbourg
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
#include <fwRenderOgre/registry/macros.hpp>

#include <fwCom/Signal.hxx>

fwRenderOgreRegisterInteractorMacro( ::fwRenderOgre::interactor::MeshPickerInteractor );

namespace fwRenderOgre
{

namespace interactor
{

MeshPickerInteractor::MeshPickerInteractor() noexcept :
    m_control(false)
{
}

//------------------------------------------------------------------------------

MeshPickerInteractor::~MeshPickerInteractor() noexcept
{
}

//------------------------------------------------------------------------------

void MeshPickerInteractor::resizeEvent(int x, int y)
{
    m_width  = x;
    m_height = y;
}

//------------------------------------------------------------------------------

void MeshPickerInteractor::buttonPressEvent(MouseButton button, int x, int y)
{
    if(m_picker->hasSceneManager())
    {
        if(m_control && m_picker->executeRaySceneQuery(x, y, m_width, m_height, m_queryFlags))
        {
            ::Ogre::Vector3 click = m_picker->getIntersectionInWorldSpace();

            ::fwData::Point::sptr point                = fwData::Point::New();
            ::fwData::Point::PointCoordArrayType cords =
            {{static_cast<double>(click.x), static_cast<double>(click.y), static_cast<double>(click.z)}};
            point->setCoord(cords);

            if(button == MouseButton::LEFT)
            {
                m_sigAddPoint->asyncEmit(::fwData::Object::dynamicCast(point));
            }
            else
            {
                m_sigRemovePoint->asyncEmit(::fwData::Object::dynamicCast(point));
            }
        }
    }
    else
    {
        SLM_WARN("The picker scene hasn't been initialized, you are not using this interactor correctly");
    }
}

//------------------------------------------------------------------------------

void MeshPickerInteractor::mouseMoveEvent(MouseButton, int, int, int, int)
{
}

//------------------------------------------------------------------------------

void MeshPickerInteractor::wheelEvent(int, int, int)
{
}

//------------------------------------------------------------------------------

void MeshPickerInteractor::buttonReleaseEvent(MouseButton, int, int)
{
}

//------------------------------------------------------------------------------

void MeshPickerInteractor::keyPressEvent(int k)
{
    if(k == Modifier::CONTROL)
    {
        m_control = true;
    }
}

//------------------------------------------------------------------------------

void MeshPickerInteractor::keyReleaseEvent(int)
{
    m_control = false;
}

//------------------------------------------------------------------------------

void MeshPickerInteractor::focusInEvent()
{
}

//------------------------------------------------------------------------------

void MeshPickerInteractor::focusOutEvent()
{
    m_control = false;
}

//------------------------------------------------------------------------------

} //namespace interactor
} //namespace fwRenderOgre

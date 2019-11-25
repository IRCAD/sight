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

#include "fwRenderOgre/interactor/IInteractor.hpp"

#include "fwRenderOgre/Layer.hpp"
#include "fwRenderOgre/Utils.hpp"

#include <fwCom/Signal.hxx>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hxx>

#include <fwServices/macros.hpp>

namespace fwRenderOgre
{
namespace interactor
{

const ::fwCom::Signals::SignalKeyType fwRenderOgre::interactor::IInteractor::s_RENDER_REQUESTED_SIG = "renderRequested";

// ----------------------------------------------------------------------------

IInteractor::IInteractor(Layer::sptr _layer)  :
    m_layer(_layer)
{
    m_ogreRoot = ::fwRenderOgre::Utils::getOgreRoot();

    if(_layer)
    {
        m_sceneManager = _layer->getSceneManager();
    }
}

// ----------------------------------------------------------------------------

IInteractor::~IInteractor()
{
}

// ----------------------------------------------------------------------------

void IInteractor::setSceneID(const std::string& sceneID)
{
    m_sceneManager = m_ogreRoot->getSceneManager(sceneID);
}

// ----------------------------------------------------------------------------

void IInteractor::setSceneLength(float)
{
}

//------------------------------------------------------------------------------

void IInteractor::mouseMoveEvent(MouseButton, Modifier, int, int, int, int)
{
}

//------------------------------------------------------------------------------

void IInteractor::wheelEvent(Modifier, int, int, int)
{
}

//------------------------------------------------------------------------------

void IInteractor::buttonReleaseEvent(MouseButton, Modifier, int, int)
{
}

//------------------------------------------------------------------------------

void IInteractor::buttonPressEvent(MouseButton, Modifier, int, int)
{
}

//------------------------------------------------------------------------------

void IInteractor::keyPressEvent(int, Modifier)
{
}

//------------------------------------------------------------------------------

void IInteractor::keyReleaseEvent(int, Modifier)
{
}

//------------------------------------------------------------------------------

void IInteractor::mouseMoveEvent(MouseButton, int, int, int, int)
{

}

//------------------------------------------------------------------------------

void IInteractor::wheelEvent(int, int, int)
{

}

//------------------------------------------------------------------------------

void IInteractor::resizeEvent(int, int)
{

}

//------------------------------------------------------------------------------

void IInteractor::keyPressEvent(int)
{

}

//------------------------------------------------------------------------------

void IInteractor::keyReleaseEvent(int)
{

}

//------------------------------------------------------------------------------

void IInteractor::buttonReleaseEvent(MouseButton, int, int)
{

}

//------------------------------------------------------------------------------

void IInteractor::buttonPressEvent(MouseButton, int, int)
{

}

//------------------------------------------------------------------------------

void IInteractor::focusInEvent()
{

}

//------------------------------------------------------------------------------

void IInteractor::focusOutEvent()
{

}

// ----------------------------------------------------------------------------

} // namespace interactor
} // namespace fwRenderOgre

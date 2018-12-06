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

#include <fwRenderOgre/interactor/IInteractor.hpp>
#include <fwRenderOgre/Utils.hpp>

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

IInteractor::IInteractor()  :
    m_sceneManager(nullptr)
{
    m_ogreRoot = ::fwRenderOgre::Utils::getOgreRoot();
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

} // namespace interactor
} // namespace fwRenderOgre

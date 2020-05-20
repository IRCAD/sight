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

#include "fwRenderOgre/interactor/IPickerInteractor.hpp"

#include <fwRenderOgre/Utils.hpp>

#include <fwCom/Signal.hxx>

namespace fwRenderOgre
{

namespace interactor
{

const ::fwCom::Signals::SignalKeyType fwRenderOgre::interactor::IPickerInteractor::s_PICKED_SIG = "picked";

// ----------------------------------------------------------------------------

IPickerInteractor::IPickerInteractor()
{
    m_ogreRoot = ::fwRenderOgre::Utils::getOgreRoot();
    m_picker   = new ::fwRenderOgre::picker::IPicker();

    m_picked = newSignal< PointClickedSigType >( s_PICKED_SIG );

    m_sigRenderRequested = newSignal< RenderRequestedSigType >( s_RENDER_REQUESTED_SIG );
}

// ----------------------------------------------------------------------------

IPickerInteractor::~IPickerInteractor()
{
    delete m_picker;
}

// ----------------------------------------------------------------------------

bool IPickerInteractor::isPickerInitialized() const
{
    return m_picker->hasSceneManager();
}

// ----------------------------------------------------------------------------

void IPickerInteractor::initPicker()
{
    OSLM_ASSERT("The scene manager should always be initialized before the picker (as one needs the other)"
                , m_sceneManager);
    m_picker->setSceneManager(m_sceneManager);
}

// ----------------------------------------------------------------------------

void IPickerInteractor::setQueryMask(uint32_t _queryMask)
{
    m_queryMask = _queryMask;
}

//-----------------------------------------------------------------------------

} // namespace interactor
} // namespace fwRenderOgre

/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwRenderOgre/interactor/IPickerInteractor.hpp"

#include <fwRenderOgre/Utils.hpp>

#include <fwCom/Signal.hxx>

namespace fwRenderOgre
{

namespace interactor
{

// ----------------------------------------------------------------------------

IPickerInteractor::IPickerInteractor()
{
    m_ogreRoot = ::fwRenderOgre::Utils::getOgreRoot();
    m_picker   = new ::fwRenderOgre::picker::IPicker();

    m_sigPointClicked = newSignal< PointClickedSigType >( s_POINT_CLICKED_SIG );
}

// ----------------------------------------------------------------------------

IPickerInteractor::~IPickerInteractor()
{
    delete m_picker;
}

// ----------------------------------------------------------------------------

bool IPickerInteractor::isPickerInitialized()
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

void IPickerInteractor::setQueryFlags(uint32_t _queryFlags)
{
    m_queryFlags = _queryFlags;
}

//-----------------------------------------------------------------------------

} // namespace interactor
} // namespace fwRenderOgre

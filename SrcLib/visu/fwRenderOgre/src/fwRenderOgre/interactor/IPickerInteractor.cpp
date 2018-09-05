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

const ::fwCom::Signals::SignalKeyType fwRenderOgre::interactor::IPickerInteractor::s_ADD_POINT_SIG    = "addPoint";
const ::fwCom::Signals::SignalKeyType fwRenderOgre::interactor::IPickerInteractor::s_REMOVE_POINT_SIG = "removePoint";

// ----------------------------------------------------------------------------

IPickerInteractor::IPickerInteractor()
{
    m_ogreRoot = ::fwRenderOgre::Utils::getOgreRoot();
    m_picker   = new ::fwRenderOgre::picker::IPicker();

    m_sigAddPoint    = newSignal< PointClickedSigType >( s_ADD_POINT_SIG );
    m_sigRemovePoint = newSignal< PointClickedSigType >( s_REMOVE_POINT_SIG );

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

void IPickerInteractor::setQueryFlags(uint32_t _queryFlags)
{
    m_queryFlags = _queryFlags;
}

//-----------------------------------------------------------------------------

} // namespace interactor
} // namespace fwRenderOgre

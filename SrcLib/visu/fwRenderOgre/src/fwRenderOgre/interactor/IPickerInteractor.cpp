/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwRenderOgre/interactor/IPickerInteractor.hpp"

#include <fwRenderOgre/Utils.hpp>

#include <fwCom/Signal.hxx>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hxx>

#include <fwServices/macros.hpp>

namespace fwRenderOgre
{

namespace interactor
{

// ----------------------------------------------------------------------------

IPickerInteractor::IPickerInteractor() :
    m_isPickerInitialized(false)
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
    return m_isPickerInitialized;
}

// ----------------------------------------------------------------------------

void IPickerInteractor::initPicker()
{

    OSLM_ASSERT("The scene manager should always be initialized before the picker (as one needs the other)"
                , m_sceneManager);
    m_picker->setSceneManager(m_sceneManager);

}

//-----------------------------------------------------------------------------

} // namespace interactor
} // namespace fwRenderOgre

/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwRenderOgre/interactor/IInteractor.hpp>
#include <fwRenderOgre/Utils.hpp>

#include <fwCom/Signal.hxx>
#include <fwCom/Slots.hxx>
#include <fwCom/Slot.hxx>

#include <fwServices/macros.hpp>

namespace fwRenderOgre
{
namespace interactor
{

const ::fwCom::Signals::SignalKeyType fwRenderOgre::interactor::IInteractor::s_POINT_CLICKED_SIG = "pointClicked";
// ----------------------------------------------------------------------------

IInteractor::IInteractor()  :
    m_sceneManager(nullptr),
    m_mouseX(0),
    m_mouseY(0)
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

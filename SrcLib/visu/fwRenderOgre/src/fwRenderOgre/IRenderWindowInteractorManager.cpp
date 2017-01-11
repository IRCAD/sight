/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwRenderOgre/IRenderWindowInteractorManager.hpp"

namespace fwRenderOgre
{

//-----------------------------------------------------------------------------

const IRenderWindowInteractorManager::FactoryRegistryKeyType IRenderWindowInteractorManager::REGISTRY_KEY =
    "::fwRenderOgre::IRenderWindowInteractorManager::REGISTRY_KEY";

//-----------------------------------------------------------------------------

IRenderWindowInteractorManager::sptr IRenderWindowInteractorManager::createManager()
{
    IRenderWindowInteractorManager::sptr manager = ::fwRenderOgre::factory::New(
        ::fwRenderOgre::IRenderWindowInteractorManager::REGISTRY_KEY );
    SLM_ASSERT(
        "The factory process to create an IRenderWindowInteractorManager, failed. Missing Qt or Wx component ?",
        manager );
    return manager;
}

//-----------------------------------------------------------------------------

IRenderWindowInteractorManager::IRenderWindowInteractorManager()
{
}

//-----------------------------------------------------------------------------

IRenderWindowInteractorManager::~IRenderWindowInteractorManager()
{
}

//-----------------------------------------------------------------------------

} // namespace fwRenderOgre




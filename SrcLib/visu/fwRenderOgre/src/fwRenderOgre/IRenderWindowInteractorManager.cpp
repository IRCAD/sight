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

#include "fwRenderOgre/IRenderWindowInteractorManager.hpp"

namespace fwRenderOgre
{

//-----------------------------------------------------------------------------

const IRenderWindowInteractorManager::FactoryRegistryKeyType IRenderWindowInteractorManager::REGISTRY_KEY =
    "::fwRenderOgre::IRenderWindowInteractorManager::REGISTRY_KEY";

const IRenderWindowInteractorManager::FactoryRegistryKeyType IRenderWindowInteractorManager::OFFSCREEN_REGISTRY_KEY =
    "::fwRenderOgre::IRenderWindowInteractorManager::OFFSCREEN_REGISTRY_KEY";

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

IRenderWindowInteractorManager::sptr IRenderWindowInteractorManager::createOffscreenManager(unsigned int _w,
                                                                                            unsigned int _h)
{
    IRenderWindowInteractorManager::sptr manager = ::fwRenderOgre::offscreenInteractorMgrFactory::New(
        ::fwRenderOgre::IRenderWindowInteractorManager::OFFSCREEN_REGISTRY_KEY, std::make_pair(_w, _h));
    SLM_ASSERT(
        "The factory process to create an IRenderWindowInteractorManager, failed. Missing Qt or Wx component ?",
        manager);
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

void IRenderWindowInteractorManager::setEnabledOverlays(const IRenderWindowInteractorManager::OverlaySetType&)
{

}

//-----------------------------------------------------------------------------

} // namespace fwRenderOgre

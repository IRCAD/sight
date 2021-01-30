/************************************************************************
 *
 * Copyright (C) 2014-2021 IRCAD France
 * Copyright (C) 2014-2021 IHU Strasbourg
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

#include "viz/ogre/IWindowInteractor.hpp"

namespace sight::viz::ogre
{

//-----------------------------------------------------------------------------

const IWindowInteractor::FactoryRegistryKeyType IWindowInteractor::REGISTRY_KEY =
    "::viz::ogre::IWindowInteractor::REGISTRY_KEY";

const IWindowInteractor::FactoryRegistryKeyType IWindowInteractor::OFFSCREEN_REGISTRY_KEY =
    "::viz::ogre::IWindowInteractor::OFFSCREEN_REGISTRY_KEY";

//-----------------------------------------------------------------------------

IWindowInteractor::sptr IWindowInteractor::createManager()
{
    IWindowInteractor::sptr manager = viz::ogre::factory::New(
        viz::ogre::IWindowInteractor::REGISTRY_KEY );
    SLM_ASSERT(
        "The factory process to create an IWindowInteractor, failed. Missing Qt or Wx component ?",
        manager );
    return manager;
}

//-----------------------------------------------------------------------------

IWindowInteractor::sptr IWindowInteractor::createOffscreenManager(unsigned int _w,
                                                                  unsigned int _h)
{
    IWindowInteractor::sptr manager = viz::ogre::offscreenInteractorMgrFactory::New(
        viz::ogre::IWindowInteractor::OFFSCREEN_REGISTRY_KEY, std::make_pair(_w, _h));
    SLM_ASSERT(
        "The factory process to create an IWindowInteractor, failed. Missing Qt or Wx component ?",
        manager);
    return manager;
}

//-----------------------------------------------------------------------------

IWindowInteractor::IWindowInteractor()
{
}

//-----------------------------------------------------------------------------

IWindowInteractor::~IWindowInteractor()
{
}

//-----------------------------------------------------------------------------

void IWindowInteractor::setFullscreen(bool, int)
{
    SLM_ERROR("Fullscreen not supported.");
}

//-----------------------------------------------------------------------------

} // namespace sight::viz::ogre

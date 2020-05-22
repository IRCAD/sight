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

#include "fwRenderOgre/factory/new.hpp"

#include "fwRenderOgre/ILight.hpp"
#include "fwRenderOgre/IRenderWindowInteractorManager.hpp"

namespace fwRenderOgre
{

namespace factory
{

//------------------------------------------------------------------------------

::fwRenderOgre::IRenderWindowInteractorManager::sptr New( const ::fwRenderOgre::registry::KeyType& classname )
{
    return ::fwRenderOgre::registry::get()->create(classname);
}

} // namespace factory

namespace offscreenInteractorMgrFactory
{

//------------------------------------------------------------------------------

::fwRenderOgre::IRenderWindowInteractorManager::sptr New(const ::fwRenderOgre::registry::KeyType& classname,
                                                         std::pair<unsigned int, unsigned int> _dims)
{
    return ::fwRenderOgre::registry::getOffscreenMgr()->create(classname, _dims);
}

} // namespace offscreenInteractorMgrFactory

namespace lightFactory
{

//------------------------------------------------------------------------------

::fwRenderOgre::ILight::sptr New( const ::fwRenderOgre::registry::KeyType& classname)
{
    return ::fwRenderOgre::registry::getLightRegistry()->create(classname);
}

} // namespace lightFactory

namespace interactorFactory
{

SPTR(::fwRenderOgre::interactor::IInteractor) New( const ::fwRenderOgre::registry::KeyType& classname,
                                                   const std::string& sceneManagerName)
{
    auto interactor = ::fwRenderOgre::registry::getInteractorRegistry()->create(classname);
    interactor->setSceneID(sceneManagerName);
    return interactor;
}

} // namespace interactorFactory

} // namespace fwRenderOgre

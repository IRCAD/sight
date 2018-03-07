/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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

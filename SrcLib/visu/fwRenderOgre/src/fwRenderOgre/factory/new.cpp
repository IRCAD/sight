/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwRenderOgre/factory/new.hpp"

#include "fwRenderOgre/ICamera.hpp"
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

namespace cameraFactory
{

//------------------------------------------------------------------------------

::fwRenderOgre::ICamera::sptr New( const ::fwRenderOgre::registry::KeyType& classname )
{
    return ::fwRenderOgre::registry::getCameraRegistry()->create(classname);
}

} // namespace cameraFactory

namespace lightFactory
{

//------------------------------------------------------------------------------

::fwRenderOgre::ILight::sptr New( const ::fwRenderOgre::registry::KeyType& classname )
{
    return ::fwRenderOgre::registry::getLightRegistry()->create(classname);
}

} // namespace lightFactory

namespace interactorFactory
{

SPTR(::fwRenderOgre::interactor::IInteractor) New( const ::fwRenderOgre::registry::KeyType & classname)
{
    return ::fwRenderOgre::registry::getInteractorRegistry()->create(classname);
}

} // namespace interactorFactory

} // namespace fwRenderOgre


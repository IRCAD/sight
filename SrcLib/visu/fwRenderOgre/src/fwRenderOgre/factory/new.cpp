/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwRenderOgre/IRenderWindowInteractorManager.hpp"
#include "fwRenderOgre/factory/new.hpp"


namespace fwRenderOgre
{

namespace factory
{

::fwRenderOgre::IRenderWindowInteractorManager::sptr New( const ::fwRenderOgre::registry::KeyType& classname )
{
    return ::fwRenderOgre::registry::get()->create(classname);
}

} // namespace factory

namespace interactorFactory
{

SPTR(::fwRenderOgre::interactor::IInteractor) New( const ::fwRenderOgre::registry::KeyType & classname)
{
    return ::fwRenderOgre::registry::getInteractorRegistry()->create(classname);
}

} // namespace interactorFactory

} // namespace fwRenderOgre



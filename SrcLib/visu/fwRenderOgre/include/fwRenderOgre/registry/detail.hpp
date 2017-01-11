/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWRENDEROGRE_REGISTRY_DETAIL_HPP__
#define __FWRENDEROGRE_REGISTRY_DETAIL_HPP__

#include <string>

#include <fwCore/macros.hpp>
#include <fwCore/util/FactoryRegistry.hpp>

#include "fwRenderOgre/config.hpp"

namespace fwRenderOgre
{

class IRenderWindowInteractorManager;

namespace interactor
{
class IInteractor;
}

namespace registry
{

typedef std::string KeyType;

typedef ::fwCore::util::FactoryRegistry< SPTR(::fwRenderOgre::IRenderWindowInteractorManager) (), KeyType > Type;
typedef ::fwCore::util::FactoryRegistry< SPTR(::fwRenderOgre::interactor::IInteractor) (),
                                         KeyType > InteractorFactoryType;

FWRENDEROGRE_API SPTR(Type) get();
FWRENDEROGRE_API SPTR(InteractorFactoryType) getInteractorRegistry();

} // namespace registry

} // namespace fwRenderOgre

#endif /* __FWRENDEROGRE_REGISTRY_DETAIL_HPP__ */



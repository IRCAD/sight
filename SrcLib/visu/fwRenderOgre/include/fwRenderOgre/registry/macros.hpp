/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWRENDEROGRE_REGISTRY_MACROS_HPP__
#define __FWRENDEROGRE_REGISTRY_MACROS_HPP__

#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/stringize.hpp>

#include "fwRenderOgre/IRenderWindowInteractorManager.hpp"
#include "fwRenderOgre/registry/detail.hpp"

namespace fwRenderOgre
{
namespace registry
{

#define fwRenderOgreRegisterMacro( OgreRenderInteractorMngClassname, FunctorKey )                               \
    static ::fwRenderOgre::IRenderWindowInteractorManager::Registrar< OgreRenderInteractorMngClassname >    \
    BOOST_PP_CAT( s__factory__record__, __LINE__) ( FunctorKey );

#define fwRenderOgreRegisterInteractorMacro( OgreInteractorClassname )                                          \
    static ::fwRenderOgre::interactor::IInteractor::Registrar< OgreInteractorClassname >                                \
    BOOST_PP_CAT( s__factory__record__, __LINE__) ( BOOST_PP_STRINGIZE(OgreInteractorClassname) );

} // end namespace registry
} // end namespace fwRenderOgre

#endif /*__FWRENDEROGRE_REGISTRY_MACROS_HPP__*/

/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWATOMSPATCH_REGISTRY_MACRO_HPP__
#define __FWATOMSPATCH_REGISTRY_MACRO_HPP__

#include <boost/preprocessor/cat.hpp>

#include "fwAtomsPatch/patcher/registry/detail.hpp"

namespace fwAtomsPatch
{


namespace patcher
{

namespace registry
{

#define patcherRegisterMacro( classname, objectKey )                                  \
    static ::fwAtomsPatch::patcher::IPatcher::Registrar< classname >                  \
    BOOST_PP_CAT(s__factory__record__object__, __LINE__) ( objectKey ) ;

} // namespace registry

} // namespace pacther

} // namespace fwAtomsPatch

#endif /*__FWATOMSPATCH_REGISTRY_MACRO_HPP__*/

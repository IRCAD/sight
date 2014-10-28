/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWATOMSFILTER_REGISTRY_MACRO_HPP__
#define __FWATOMSFILTER_REGISTRY_MACRO_HPP__

#include <boost/preprocessor/cat.hpp>

#include "fwAtomsFilter/registry/detail.hpp"

namespace fwAtomsFilter
{

namespace registry
{

#define  fwAtomsFilterRegisterMacro( classname, objectKey )                                  \
    static ::fwAtomsFilter::IFilter::Registrar< classname >                  \
    BOOST_PP_CAT(s__factory__record__object__, __LINE__) ( objectKey ) ;

} // namespace registry

} // namespace fwAtomsFilter

#endif /*__FWATOMSFILTER_REGISTRY_MACRO_HPP__*/


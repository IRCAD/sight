/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWMETACONVERSION_CUSTOM_REGISTRY_MACRO_HPP__
#define __FWMETACONVERSION_CUSTOM_REGISTRY_MACRO_HPP__

#include <boost/preprocessor/stringize.hpp>
#include <boost/preprocessor/cat.hpp>

#include "fwMetaConversion/custom/Mapper.hpp"
#include "fwMetaConversion/custom/registry/detail.hpp"

namespace fwMetaConversion
{
namespace custom
{
namespace registry
{

#define fwMetaConversionRegisterMacro( classname, functorKey )         \
    static ::fwMetaConversion::custom::Mapper::Registrar< classname >  \
    BOOST_PP_CAT( s__factory__record__, __LINE__) ( BOOST_PP_STRINGIZE(functorKey) );

} // end namespace registry
} // end namespace custom
} // end namespace fwMetaConversion

#endif /*__FWMETACONVERSION_CUSTOM_REGISTRY_MACRO_HPP__*/

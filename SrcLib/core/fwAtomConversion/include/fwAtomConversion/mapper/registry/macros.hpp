/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWATOMCONVERSION_MAPPER_REGISTRY_MACROS_HPP__
#define __FWATOMCONVERSION_MAPPER_REGISTRY_MACROS_HPP__

#include <boost/preprocessor/stringize.hpp>
#include <boost/preprocessor/cat.hpp>

#include "fwAtomConversion/mapper/Base.hpp"
#include "fwAtomConversion/mapper/registry/detail.hpp"

namespace fwAtomConversion
{
namespace mapper
{
namespace registry
{

#define fwAtomConversionRegisterMacro( classname, functorKey )         \
    static ::fwAtomConversion::mapper::Base::Registrar< classname >  \
    BOOST_PP_CAT( s__factory__record__, __LINE__) ( BOOST_PP_STRINGIZE(functorKey) );

} // end namespace registry
} // end namespace mapper
} // end namespace fwAtomConversion

#endif /*__FWATOMCONVERSION_MAPPER_REGISTRY_MACROS_HPP__*/

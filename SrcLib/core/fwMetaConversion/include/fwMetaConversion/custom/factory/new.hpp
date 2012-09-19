/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWMETACONVERSION_CUSTOM_FACTORY_NEW_HPP__
#define __FWMETACONVERSION_CUSTOM_FACTORY_NEW_HPP__

#include <string>
#include <boost/make_shared.hpp>

#include <fwTools/macros.hpp>

#include "fwMetaConversion/config.hpp"
#include "fwMetaConversion/custom/registry/detail.hpp"

namespace fwMetaConversion
{
namespace custom
{

class Mapper;

namespace factory
{

template<class CLASSNAME > SPTR( CLASSNAME ) New();

FWMETACONVERSION_API SPTR(::fwMetaConversion::custom::Mapper) New(
        const ::fwMetaConversion::custom::registry::KeyType & classname );


template<class CLASSNAME > SPTR( CLASSNAME ) New()
{
    SPTR(CLASSNAME) obj = ::boost::make_shared< CLASSNAME >();
    return obj;
}

} // namespace custom
} // namespace factory
} // namespace fwMetaConversion

#endif /* __FWMETACONVERSION_CUSTOM_FACTORY_NEW_HPP__ */



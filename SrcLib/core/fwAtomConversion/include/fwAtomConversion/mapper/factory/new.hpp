/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWATOMCONVERSION_MAPPER_FACTORY_NEW_HPP__
#define __FWATOMCONVERSION_MAPPER_FACTORY_NEW_HPP__

#include "fwAtomConversion/config.hpp"
#include "fwAtomConversion/mapper/registry/detail.hpp"

#include <string>

namespace fwAtomConversion
{
namespace mapper
{

class Mapper;

namespace factory
{

template<class CLASSNAME > SPTR( CLASSNAME ) New();

FWATOMCONVERSION_API SPTR(::fwAtomConversion::mapper::Base) New(
    const ::fwAtomConversion::mapper::registry::KeyType & classname );

template<class CLASSNAME > SPTR( CLASSNAME ) New()
{
    SPTR(CLASSNAME) obj = std::make_shared< CLASSNAME >();
    return obj;
}

} // namespace mapper
} // namespace factory
} // namespace fwAtomConversion

#endif /* __FWATOMCONVERSION_MAPPER_FACTORY_NEW_HPP__ */


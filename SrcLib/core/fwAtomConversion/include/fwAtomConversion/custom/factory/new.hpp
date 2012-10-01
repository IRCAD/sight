/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWATOMCONVERSION_CUSTOM_FACTORY_NEW_HPP__
#define __FWATOMCONVERSION_CUSTOM_FACTORY_NEW_HPP__

#include <string>
#include <boost/make_shared.hpp>

#include <fwTools/macros.hpp>

#include "fwAtomConversion/config.hpp"
#include "fwAtomConversion/custom/registry/detail.hpp"

namespace fwAtomConversion
{
namespace custom
{

class Mapper;

namespace factory
{

template<class CLASSNAME > SPTR( CLASSNAME ) New();

FWATOMCONVERSION_API SPTR(::fwAtomConversion::custom::Mapper) New(
        const ::fwAtomConversion::custom::registry::KeyType & classname );


template<class CLASSNAME > SPTR( CLASSNAME ) New()
{
    SPTR(CLASSNAME) obj = ::boost::make_shared< CLASSNAME >();
    return obj;
}

} // namespace custom
} // namespace factory
} // namespace fwAtomConversion

#endif /* __FWATOMCONVERSION_CUSTOM_FACTORY_NEW_HPP__ */



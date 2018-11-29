/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

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


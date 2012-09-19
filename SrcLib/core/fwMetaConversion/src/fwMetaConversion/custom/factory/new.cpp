/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwMetaConversion/custom/Mapper.hpp"

#include "fwMetaConversion/custom/factory/new.hpp"


namespace fwMetaConversion
{
namespace custom
{
namespace factory
{

SPTR(::fwMetaConversion::custom::Mapper) New( const ::fwMetaConversion::custom::registry::KeyType & classname )
{
    return ::fwMetaConversion::custom::registry::get()->create(classname);
}

} // namespace factory
} // namespace custom
} // namespace fwMetaConversion



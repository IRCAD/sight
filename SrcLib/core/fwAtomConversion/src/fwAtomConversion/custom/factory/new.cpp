/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwAtomConversion/custom/Mapper.hpp"

#include "fwAtomConversion/custom/factory/new.hpp"


namespace fwAtomConversion
{
namespace custom
{
namespace factory
{

SPTR(::fwAtomConversion::custom::Mapper) New( const ::fwAtomConversion::custom::registry::KeyType & classname )
{
    return ::fwAtomConversion::custom::registry::get()->create(classname);
}

} // namespace factory
} // namespace custom
} // namespace fwAtomConversion



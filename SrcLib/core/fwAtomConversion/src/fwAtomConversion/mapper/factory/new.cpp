/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwAtomConversion/mapper/Base.hpp"

#include "fwAtomConversion/mapper/factory/new.hpp"


namespace fwAtomConversion
{
namespace mapper
{
namespace factory
{

SPTR(::fwAtomConversion::mapper::Base) New( const ::fwAtomConversion::mapper::registry::KeyType & classname )
{
    return ::fwAtomConversion::mapper::registry::get()->create(classname);
}

} // namespace factory
} // namespace mapper
} // namespace fwAtomConversion



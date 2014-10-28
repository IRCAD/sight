/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwAtomsFilter/factory/new.hpp"

#include "fwAtomsFilter/IFilter.hpp"

namespace fwAtomsFilter
{

namespace factory
{

SPTR(::fwAtomsFilter::IFilter) New( const ::fwAtomsFilter::registry::KeyType & classname )
{
    return ::fwAtomsFilter::registry::get()->create(classname);
}

} // namespace factory

} // namespace fwAtomsFilter


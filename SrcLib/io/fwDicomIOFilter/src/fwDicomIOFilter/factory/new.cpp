/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwDicomIOFilter/IFilter.hpp"
#include "fwDicomIOFilter/factory/new.hpp"


namespace fwDicomIOFilter
{

namespace factory
{

::fwDicomIOFilter::IFilter::sptr New( const ::fwDicomIOFilter::registry::KeyType& classname )
{
    return ::fwDicomIOFilter::registry::get()->create(classname);
}

} // namespace factory

} // namespace fwDicomIOFilter



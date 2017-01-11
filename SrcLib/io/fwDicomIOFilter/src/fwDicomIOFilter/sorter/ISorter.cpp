/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwDicomIOFilter/sorter/ISorter.hpp"

namespace fwDicomIOFilter
{
namespace sorter
{

ISorter::ISorter()
{
}

//-----------------------------------------------------------------------------

ISorter::~ISorter()
{
}

//-----------------------------------------------------------------------------

IFilter::FilterType ISorter::getFilterType() const
{
    return IFilter::SORTER;
}

} // namespace sorter
} // namespace fwDicomIOFilter

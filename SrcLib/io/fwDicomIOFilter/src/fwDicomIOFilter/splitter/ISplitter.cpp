/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwDicomIOFilter/splitter/ISplitter.hpp"

namespace fwDicomIOFilter
{
namespace splitter
{

ISplitter::ISplitter()
{
}

//-----------------------------------------------------------------------------

ISplitter::~ISplitter()
{
}

//-----------------------------------------------------------------------------

IFilter::FilterType ISplitter::getFilterType() const
{
    return ISplitter::SPLITTER;
}

} // namespace splitter
} // namespace fwDicomIOFilter

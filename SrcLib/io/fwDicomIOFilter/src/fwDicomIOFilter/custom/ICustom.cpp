/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwDicomIOFilter/custom/ICustom.hpp"

namespace fwDicomIOFilter
{
namespace custom
{

ICustom::ICustom()
{
}

//-----------------------------------------------------------------------------

ICustom::~ICustom()
{
}

//-----------------------------------------------------------------------------

IFilter::FilterType ICustom::getFilterType() const
{
    return IFilter::CUSTOM;
}

} // namespace custom
} // namespace fwDicomIOFilter

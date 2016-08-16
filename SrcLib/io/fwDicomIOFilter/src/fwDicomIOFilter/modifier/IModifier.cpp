/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwDicomIOFilter/modifier/IModifier.hpp"

namespace fwDicomIOFilter
{
namespace modifier
{

IModifier::IModifier()
{
}

//-----------------------------------------------------------------------------

IModifier::~IModifier()
{
}

//-----------------------------------------------------------------------------

IFilter::FilterType IModifier::getFilterType() const
{
    return IFilter::MODIFIER;
}

} // namespace modifier
} // namespace fwDicomIOFilter

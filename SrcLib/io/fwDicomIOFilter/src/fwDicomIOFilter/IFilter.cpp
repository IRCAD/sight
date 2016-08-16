/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwDicomIOFilter/IFilter.hpp"

namespace fwDicomIOFilter
{

IFilter::IFilter()
{
}

//-----------------------------------------------------------------------------

IFilter::~IFilter()
{
}

//-----------------------------------------------------------------------------

bool IFilter::isConfigurationRequired() const
{
    return false;
}

//-----------------------------------------------------------------------------

bool IFilter::isConfigurableWithGUI() const
{
    return false;
}

//-----------------------------------------------------------------------------

void IFilter::configureWithGUI()
{
    SLM_WARN("You should override this function in order to configure your filter using a GUI frame.");
}

} // namespace fwDicomIOFilter

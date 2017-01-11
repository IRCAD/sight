/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwDicomIOFilter/splitter/TemporalPositionSplitter.hpp"
#include "fwDicomIOFilter/registry/macros.hpp"

#include <dcmtk/dcmdata/dcdeftag.h>

fwDicomIOFilterRegisterMacro( ::fwDicomIOFilter::splitter::TemporalPositionSplitter );

namespace fwDicomIOFilter
{
namespace splitter
{

const std::string TemporalPositionSplitter::s_FILTER_NAME        = "Temporal position splitter";
const std::string TemporalPositionSplitter::s_FILTER_DESCRIPTION =
    "Split instances according to temporal position identifier tag.";

//-----------------------------------------------------------------------------

TemporalPositionSplitter::TemporalPositionSplitter(::fwDicomIOFilter::IFilter::Key key) : TagValueSplitter(key)
{
    this->setTag(DCM_TemporalPositionIdentifier);
}

//-----------------------------------------------------------------------------

TemporalPositionSplitter::~TemporalPositionSplitter()
{
}

//-----------------------------------------------------------------------------

std::string TemporalPositionSplitter::getName() const
{
    return TemporalPositionSplitter::s_FILTER_NAME;
}

//-----------------------------------------------------------------------------

std::string TemporalPositionSplitter::getDescription() const
{
    return TemporalPositionSplitter::s_FILTER_DESCRIPTION;
}

//-----------------------------------------------------------------------------

bool TemporalPositionSplitter::isConfigurationRequired() const
{
    return false;
}

//-----------------------------------------------------------------------------

} // namespace splitter
} // namespace fwDicomIOFilter

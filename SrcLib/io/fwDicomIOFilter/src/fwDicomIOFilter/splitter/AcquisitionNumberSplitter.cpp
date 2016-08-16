/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwDicomIOFilter/splitter/AcquisitionNumberSplitter.hpp"
#include "fwDicomIOFilter/registry/macros.hpp"

#include <dcmtk/dcmdata/dcdeftag.h>

#include <string>

fwDicomIOFilterRegisterMacro( ::fwDicomIOFilter::splitter::AcquisitionNumberSplitter );

namespace fwDicomIOFilter
{
namespace splitter
{

const std::string AcquisitionNumberSplitter::s_FILTER_NAME        = "Acquisition number splitter";
const std::string AcquisitionNumberSplitter::s_FILTER_DESCRIPTION =
    "Split instances according to acquisition number tag.";

//-----------------------------------------------------------------------------

AcquisitionNumberSplitter::AcquisitionNumberSplitter(::fwDicomIOFilter::IFilter::Key key) : TagValueSplitter(key)
{
    this->setTag(DCM_AcquisitionNumber);
}

//-----------------------------------------------------------------------------

AcquisitionNumberSplitter::~AcquisitionNumberSplitter()
{
}

//-----------------------------------------------------------------------------

std::string AcquisitionNumberSplitter::getName() const
{
    return AcquisitionNumberSplitter::s_FILTER_NAME;
}

//-----------------------------------------------------------------------------

std::string AcquisitionNumberSplitter::getDescription() const
{
    return AcquisitionNumberSplitter::s_FILTER_DESCRIPTION;
}

//-----------------------------------------------------------------------------

bool AcquisitionNumberSplitter::isConfigurationRequired() const
{
    return false;
}

//-----------------------------------------------------------------------------

} // namespace splitter
} // namespace fwDicomIOFilter

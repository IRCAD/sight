/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwDicomIOFilter/sorter/InstanceNumberSorter.hpp"
#include "fwDicomIOFilter/registry/macros.hpp"
#include "fwDicomIOFilter/exceptions/FilterFailure.hpp"

#include <dcmtk/config/osconfig.h>
#include <dcmtk/dcmnet/diutil.h>
#include <dcmtk/dcmdata/dcfilefo.h>
#include <dcmtk/dcmdata/dcdeftag.h>
#include <dcmtk/dcmimgle/dcmimage.h>

fwDicomIOFilterRegisterMacro( ::fwDicomIOFilter::sorter::InstanceNumberSorter );

namespace fwDicomIOFilter
{
namespace sorter
{

const std::string InstanceNumberSorter::s_FILTER_NAME        = "Instance number sorter";
const std::string InstanceNumberSorter::s_FILTER_DESCRIPTION =
    "Sort instances using <i>InstanceNumber</i> tag.";

//-----------------------------------------------------------------------------

InstanceNumberSorter::InstanceNumberSorter(::fwDicomIOFilter::IFilter::Key key) :
    fwDicomIOFilter::sorter::TagValueSorter(key)
{
    this->setTag(DCM_InstanceNumber);
}

//-----------------------------------------------------------------------------

InstanceNumberSorter::~InstanceNumberSorter()
{
}

//-----------------------------------------------------------------------------

std::string InstanceNumberSorter::getName() const
{
    return InstanceNumberSorter::s_FILTER_NAME;
}

//-----------------------------------------------------------------------------

std::string InstanceNumberSorter::getDescription() const
{
    return InstanceNumberSorter::s_FILTER_DESCRIPTION;
}

//-----------------------------------------------------------------------------

bool InstanceNumberSorter::isConfigurationRequired() const
{
    return false;
}

} // namespace sorter
} // namespace fwDicomIOFilter

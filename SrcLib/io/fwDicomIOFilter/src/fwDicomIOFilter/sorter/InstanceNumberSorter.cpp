/************************************************************************
 *
 * Copyright (C) 2009-2016 IRCAD France
 * Copyright (C) 2012-2016 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

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

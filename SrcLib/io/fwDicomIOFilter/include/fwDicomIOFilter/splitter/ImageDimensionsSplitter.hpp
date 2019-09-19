/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#pragma once

#include "fwDicomIOFilter/config.hpp"
#include "fwDicomIOFilter/splitter/ISplitter.hpp"

#include <fwMedData/DicomSeries.hpp>

namespace fwDicomIOFilter
{
namespace splitter
{

/**
 * @brief Filter that uses the image dimensions to split the instances
 */
class FWDICOMIOFILTER_CLASS_API ImageDimensionsSplitter : public ISplitter
{
public:
    fwCoreClassMacro(ImageDimensionsSplitter, ISplitter, ::fwDicomIOFilter::factory::New< ImageDimensionsSplitter >);

    /// Constructor
    FWDICOMIOFILTER_API ImageDimensionsSplitter(::fwDicomIOFilter::IFilter::Key key);

    /// Destructor
    FWDICOMIOFILTER_API virtual ~ImageDimensionsSplitter();

    /// Override
    FWDICOMIOFILTER_API virtual DicomSeriesContainerType apply(
        const ::fwMedData::DicomSeries::sptr& series, const ::fwLog::Logger::sptr& logger) const override;

    /// Return the name of the filter
    FWDICOMIOFILTER_API virtual std::string getName() const override;

    /// Return the description of the filter
    FWDICOMIOFILTER_API virtual std::string getDescription() const override;

protected:
    /// Filter name
    static const std::string s_FILTER_NAME;

    /// Filter description
    static const std::string s_FILTER_DESCRIPTION;

};

} // namespace splitter
} // namespace fwDicomIOFilter

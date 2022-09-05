/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "filter/dicom/config.hpp"
#include "filter/dicom/custom/ICustom.hpp"

#include <data/DicomSeries.hpp>

namespace sight::filter::dicom::custom
{

/**
 * @brief Default DICOM Filter
 */
class FILTER_DICOM_CLASS_API DefaultDicomFilter : public ICustom
{
public:

    SIGHT_DECLARE_CLASS(DefaultDicomFilter, ICustom, filter::dicom::factory::New<DefaultDicomFilter>);

    /// Constructor
    FILTER_DICOM_API DefaultDicomFilter(filter::dicom::IFilter::Key key);

    /// Destructor
    FILTER_DICOM_API ~DefaultDicomFilter() override;

    /// Override
    FILTER_DICOM_API DicomSeriesContainerType apply(
        const data::DicomSeries::sptr& series,
        const core::log::Logger::sptr& logger
    ) const override;

    /// Return the name of the filter
    FILTER_DICOM_API std::string getName() const override;

    /// Return the description of the filter
    FILTER_DICOM_API std::string getDescription() const override;

protected:

    /// Filter name
    static const std::string s_FILTER_NAME;

    /// Filter description
    static const std::string s_FILTER_DESCRIPTION;
};

} // namespace sight::filter::dicom::custom

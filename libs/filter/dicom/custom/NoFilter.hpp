/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

namespace sight::filter::dicom
{

namespace custom
{

/**
 * @brief Filter that doesn't apply any modification.
 */
class FILTER_DICOM_CLASS_API NoFilter : public ICustom
{
public:

    SIGHT_DECLARE_CLASS(NoFilter, ICustom, filter::dicom::factory::New<NoFilter>);

    /// Constructor
    FILTER_DICOM_API NoFilter(filter::dicom::IFilter::Key key);

    /// Destructor
    FILTER_DICOM_API virtual ~NoFilter();

    /// Override
    FILTER_DICOM_API virtual DicomSeriesContainerType apply(
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

} // namespace custom

} // namespace sight::filter::dicom

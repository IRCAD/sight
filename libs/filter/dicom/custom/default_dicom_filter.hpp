/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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
#include "filter/dicom/custom/base.hpp"

#include <data/dicom_series.hpp>

namespace sight::filter::dicom::custom
{

/**
 * @brief Default DICOM filter
 */
class FILTER_DICOM_CLASS_API default_dicom_filter : public base
{
public:

    SIGHT_DECLARE_CLASS(default_dicom_filter, base, sight::filter::dicom::factory::make<default_dicom_filter>);

    /// Destructor
    FILTER_DICOM_API ~default_dicom_filter() override = default;

    /// Override
    FILTER_DICOM_API dicom_series_container_t apply(
        const data::dicom_series::sptr& _series,
        const core::log::logger::sptr& _logger
    ) const override;

    /// Return the name of the filter
    FILTER_DICOM_API std::string get_name() const override;

    /// Return the description of the filter
    FILTER_DICOM_API std::string get_description() const override;

protected:

    /// filter name
    static const std::string FILTER_NAME;

    /// filter description
    static const std::string FILTER_DESCRIPTION;
};

} // namespace sight::filter::dicom::custom

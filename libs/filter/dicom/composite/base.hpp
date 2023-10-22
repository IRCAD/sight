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
#include "filter/dicom/filter.hpp"

#include <data/dicom_series.hpp>

namespace sight::filter::dicom::composite
{

/**
 * @brief Base class for Dicom instance composite filter.
 */
class FILTER_DICOM_CLASS_API base : public sight::filter::dicom::filter
{
public:

    SIGHT_DECLARE_CLASS(base, sight::filter::dicom::filter);

    typedef std::vector<sight::filter::dicom::filter::sptr> filter_container_t;

    /// Destructor
    FILTER_DICOM_API ~base() override;

    /// Return filter type
    FILTER_DICOM_API filter_t get_filter_type() const override;

    /// Override
    FILTER_DICOM_API dicom_series_container_t apply(
        const data::dicom_series::sptr& _series,
        const core::log::logger::sptr& _logger
    ) const override;

    /**
     * @brief Apply every filters without throwing any exceptions.
     * If an exception is thrown, it is ignored and the next filter is applied.
     * @return Returns one or more Dicom Instance Group
     */
    FILTER_DICOM_API dicom_series_container_t forcedApply(
        const data::dicom_series::sptr& _series,
        const core::log::logger::sptr& _logger
    ) const;

    /// Add a filter to the composite
    FILTER_DICOM_API void addChild(
        const sight::filter::dicom::filter::sptr& _filter
    );

    /// Remove a filter from the composite
    FILTER_DICOM_API void removeChild(
        const sight::filter::dicom::filter::sptr& _filter
    );

    /// Return composite container
    FILTER_DICOM_API filter_container_t& getChildren();

protected:

    /// Constructor
    FILTER_DICOM_API base();

    /// Filter container
    filter_container_t m_filterContainer;
};

} // namespace sight::filter::dicom::composite

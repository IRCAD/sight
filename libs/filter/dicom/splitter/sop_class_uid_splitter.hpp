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
#include "filter/dicom/splitter/TagValueSplitter.hpp"

#include <data/dicom_series.hpp>

namespace sight::filter::dicom::splitter
{

/**
 * @brief Filter that uses the sop_classUID tag to split the instances
 */
class FILTER_DICOM_CLASS_API sop_class_uid_splitter : public sight::filter::dicom::splitter::TagValueSplitter
{
public:

    SIGHT_DECLARE_CLASS(
        sop_class_uid_splitter,
        sight::filter::dicom::splitter::TagValueSplitter,
        sight::filter::dicom::factory::make<sop_class_uid_splitter>
    );

    /// Constructor
    FILTER_DICOM_API sop_class_uid_splitter();

    /// Destructor
    FILTER_DICOM_API ~sop_class_uid_splitter() override;

    /// Override
    FILTER_DICOM_API dicom_series_container_t apply(
        const data::dicom_series::sptr& _series,
        const core::log::logger::sptr& _logger
    ) const override;

    /// Return the name of the filter
    FILTER_DICOM_API std::string getName() const override;

    /// Return the description of the filter
    FILTER_DICOM_API std::string getDescription() const override;

    /// Return true if a configuration is required
    FILTER_DICOM_API bool isConfigurationRequired() const override;

protected:

    /// Filter name
    static const std::string s_FILTER_NAME;

    /// Filter description
    static const std::string s_FILTER_DESCRIPTION;
};

} // namespace sight::filter::dicom::splitter

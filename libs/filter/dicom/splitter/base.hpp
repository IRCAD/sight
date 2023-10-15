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

namespace sight::filter::dicom::splitter
{

/**
 * @brief Base class for Dicom instance splitter.
 */
class FILTER_DICOM_CLASS_API base : public sight::filter::dicom::filter
{
public:

    SIGHT_DECLARE_CLASS(base, sight::filter::dicom::filter);

    /// Destructor
    FILTER_DICOM_API ~base() override;

    /// Return filter type
    FILTER_DICOM_API FilterType getFilterType() const override;

protected:

    /// Constructor
    FILTER_DICOM_API base();
};

} // namespace sight::filter::dicom::splitter

/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#pragma once

#include <sight/filter/dicom/config.hpp>

#include <core/exception.hpp>

namespace sight::filter::dicom::exceptions
{

/**
 * @brief filter_failure exceptions
 */
class SIGHT_FILTER_DICOM_CLASS_API filter_failure : public core::exception
{
public:

    /**
     * @brief Constructor
     * @param[in] _message Exception message
     */
    SIGHT_FILTER_DICOM_API filter_failure(const std::string& _message);

    /// Destructor
    SIGHT_FILTER_DICOM_API ~filter_failure() noexcept override;
};

} // namespace sight::filter::dicom::exceptions

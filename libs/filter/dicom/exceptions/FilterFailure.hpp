/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "filter/dicom/config.hpp"

#include <core/Exception.hpp>

namespace sight::filter::dicom
{
namespace exceptions
{

/**
 * @brief FilterFailure Exceptions
 */
class FILTER_DICOM_CLASS_API FilterFailure : public core::Exception
{
public:
    /**
     * @brief Constructor
     * @param[in] message Exception message
     */
    FILTER_DICOM_API FilterFailure(const std::string& message);

    /// Destructor
    FILTER_DICOM_API virtual ~FilterFailure() noexcept;

};

} // namespace exceptions
} // namespace sight::filter::dicom

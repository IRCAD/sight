/************************************************************************
 *
 * Copyright (C) 2017-2023 IRCAD France
 * Copyright (C) 2017 IHU Strasbourg
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

#include "io/dicom/config.hpp"

#include <core/exception.hpp>

namespace sight::io::dicom::exception
{

/**
 * @brief InvalidTag exception
 */
class IO_DICOM_CLASS_API InvalidTag : public core::exception
{
public:

    /**
     * @brief Constructor
     * @param[in] message Exception message
     */
    IO_DICOM_API InvalidTag(const std::string& _message);

    /// Destructor
    IO_DICOM_API ~InvalidTag() noexcept override;
};

} // namespace sight::io::dicom::exception

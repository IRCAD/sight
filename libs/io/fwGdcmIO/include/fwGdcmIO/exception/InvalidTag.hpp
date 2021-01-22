/************************************************************************
 *
 * Copyright (C) 2017-2021 IRCAD France
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

#include "fwGdcmIO/config.hpp"

#include <core/Exception.hpp>

namespace fwGdcmIO
{
namespace exception
{

/**
 * @brief InvalidTag exception
 */
class FWGDCMIO_CLASS_API InvalidTag : public core::Exception
{
public:
    /**
     * @brief Constructor
     * @param[in] message Exception message
     */
    FWGDCMIO_API InvalidTag(const std::string& message);

    /// Destructor
    FWGDCMIO_API virtual ~InvalidTag() noexcept;

};

} // namespace exception
} // namespace fwGdcmIO

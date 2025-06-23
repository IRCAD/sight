/************************************************************************
 *
 * Copyright (C) 2025 IRCAD France
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

#include <sight/core/config.hpp>

#include "core/runtime/types.hpp"

#include <filesystem>
#include <string>

namespace sight::core::runtime
{

/**
 * @brief   Implements a simple XML validator.
 * If you parse mutiple files with the same XSD, it is more efficient to instance this class only wants and iterate
 * over files with the validate() function.
 */
class SIGHT_CORE_CLASS_API validator
{
public:

    using sptr = std::shared_ptr<validator>;

    /**
     * @brief   Instantiates the validator.
     * @param   _xsd_path a path to the XSD scheam
     */
    SIGHT_CORE_API static validator::sptr make(const std::filesystem::path& _xsd_path);

    SIGHT_CORE_API virtual ~validator() = default;

    /**
     * @brief   Validates the given file.
     * @param   _xml_file a path to an xml file to validate
     * @return  true when the validation succeeds, false otherwise
     */
    SIGHT_CORE_API virtual bool validate(const std::filesystem::path& _xml_file) = 0;

    /**
     * @brief   Validates the given xml configuration.
     * @param   _config    xml configuration
     * @return  true when the validation succeeds, false otherwise
     */
    SIGHT_CORE_API virtual bool validate(const config_t& _config) = 0;

protected:

    validator() = default;
};

} // namespace sight::core::runtime

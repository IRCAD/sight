/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "core/config.hpp"
#include "core/runtime/detail/extension.hpp"
#include "core/runtime/detail/runtime.hpp"
#include "core/runtime/extension.hpp"
#include "core/runtime/module_element.hpp"

#include <core/base.hpp>

#include <algorithm>
#include <filesystem>
#include <iostream>
#include <ranges>
#include <string>

namespace sight::core::runtime
{

class module;

namespace detail
{

namespace io
{

class validator;

} // namespace io

/**
 * @brief   Defines the extension point class.
 */
class extension_point : public module_element
{
public:

    /**
     * @brief       constructor
     *
     * @param[in]   module  a shared pointer to the module where the extension
     *                      point is declared
     * @param[in]   id      a string containing the extension point identifier
     * @param[in]   schema  a path to a file containing an XML schema used to
     *                      validate extensions contributed to the point.
     */
    extension_point(
        const std::shared_ptr<core::runtime::module> _module,
        const std::string& _id,
        std::filesystem::path _schema
    );

    /**
     * @brief   Assignment operator.
     *
     * @remark  Assignment is forbidden for this class.
     */
    void operator=(const extension_point&) noexcept = delete;

    /**
     * @brief   Retrieves all extensions contributed to the point instance.
     *
     * @return  output  shared pointers to found extensions
     */
    std::vector<std::shared_ptr<core::runtime::extension> > get_all_extensions() const
    {
        std::vector<std::shared_ptr<core::runtime::extension> > container;
        const auto& runtime = detail::runtime::get();

        std::ranges::copy_if(
            runtime.extensions(),
            std::back_inserter(container),
            [this](const auto& _e)
                {
                    const auto extension = std::dynamic_pointer_cast<detail::extension>(_e);
                    return extension->point() == M_ID && extension->enabled() && extension->validate() == extension::valid;
                });

        return container;
    }

    /**
     * @brief   Retrieves the extension point identifier.
     *
     * @return  a string containing the extension point identifier
     */
    const std::string& identifier() const;

    /**
     * @brief   Retrieves the extension validator.
     *
     * @return  a shared pointer to the extension validator, or null when none
     */
    std::shared_ptr<io::validator> get_extension_validator() const;

private:

    const std::string M_ID; ///< a string containing the extension point
    // identifier
    const std::filesystem::path M_SCHEMA; ///< a path to the XML schema used to validate
    // contributed extensions
    mutable std::shared_ptr<io::validator> m_validator; ///< a shared pointer to the extension validator
};

} // namespace detail

} // namespace sight::core::runtime

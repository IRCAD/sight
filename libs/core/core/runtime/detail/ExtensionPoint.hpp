/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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
#include "core/runtime/detail/Extension.hpp"
#include "core/runtime/detail/Runtime.hpp"
#include "core/runtime/Extension.hpp"
#include "core/runtime/ModuleElement.hpp"

#include <core/base.hpp>

#include <algorithm>
#include <filesystem>
#include <iostream>
#include <ranges>
#include <string>

namespace sight::core::runtime
{

class Module;

namespace detail
{

namespace io
{

class Validator;

} // namespace io

/**
 * @brief   Defines the extension point class.
 */
class ExtensionPoint : public ModuleElement
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
    ExtensionPoint(
        const std::shared_ptr<core::runtime::Module> module,
        const std::string& id,
        std::filesystem::path schema
    );

    /**
     * @brief   Assignment operator.
     *
     * @remark  Assignment is forbidden for this class.
     */
    void operator=(const ExtensionPoint&) noexcept = delete;

    /**
     * @brief   Retrieves all extensions contributed to the point instance.
     *
     * @return  output  shared pointers to found extensions
     */
    std::vector<std::shared_ptr<core::runtime::Extension> > getAllExtensions() const
    {
        std::vector<std::shared_ptr<core::runtime::Extension> > container;
        const auto& runtime = detail::Runtime::get();

        std::ranges::copy_if(
            runtime.getExtensions(),
            std::back_inserter(container),
            [this](const auto& e)
                {
                    const auto extension = std::dynamic_pointer_cast<detail::Extension>(e);
                    return extension->getPoint() == m_id && extension->isEnabled() && extension->validate() == Extension::Valid;
                });

        return container;
    }

    /**
     * @brief   Retrieves the extension point identifier.
     *
     * @return  a string containing the extension point identifier
     */
    const std::string& getIdentifier() const;

    /**
     * @brief   Retrieves the extension validator.
     *
     * @return  a shared pointer to the extension validator, or null when none
     */
    std::shared_ptr<io::Validator> getExtensionValidator() const;

private:

    const std::string m_id; ///< a string containing the extension point
    // identifier
    const std::filesystem::path m_schema; ///< a path to the XML schema used to validate
    // contributed extensions
    mutable std::shared_ptr<io::Validator> m_validator; ///< a shared pointer to the extension validator
};

} // namespace detail

} // namespace sight::core::runtime

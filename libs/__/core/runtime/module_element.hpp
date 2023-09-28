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

#include <memory>

namespace sight::core::runtime
{

class module;

} // namespace sight::core::runtime

namespace sight::core::runtime
{

/**
 * @brief   Implements the base class for all element managed by a module.
 */
class CORE_CLASS_API module_element
{
public:

    virtual ~module_element() = default;

    /**
     * @brief   Retrieves the module that owns the extension.
     *
     * @return  a pointer to a module instance
     */
    [[nodiscard]] CORE_API std::shared_ptr<module> get_module() const;

    /**
     * @brief   Tells if the element is enabled or not.
     *
     * @remark  The element will asks its module enable state. if the module
     *          element is enabled then the program verifies if the module
     *          element is enabled too.
     *
     * @pre     The module must not be null.
     *
     * @return  true or false
     */
    [[nodiscard]] CORE_API bool enabled() const;

    /**
     * @brief       Set if the element is enabled or not.
     *
     * @see         isEnabled
     *
     * @param[in]   enable To know if module element is enabled
     */
    CORE_API void set_enable(bool enable);

protected:

    /**
     * @brief   Constructor
     *
     * @remark  The instance will try to guess the module that is being loaded and
     *          consider that module as its owning module
     *
     * @post    The module isn't null.
     * @see     core::runtime::module::getLoadingmodule()
     */
    CORE_API module_element();

    /**
     * @brief       Constructor
     *
     * @post        The module isn't null.
     * @param[in]   module  a shared pointer to the managing module
     */
    CORE_API module_element(std::shared_ptr<module> module);

private:

    std::weak_ptr<module> m_module; ///< Weak pointer to the module managing the element
    bool m_enable {true};           ///< to know if module element is enabled
};

} // namespace sight::core::runtime

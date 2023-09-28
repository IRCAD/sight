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
#include "core/runtime/detail/dl/native.hpp"
#include "core/runtime/runtime_exception.hpp"

#include <filesystem>
#include <memory>

namespace sight::core::runtime::detail::dl
{

/**
 * @brief   Defines the module class.
 * This class is only a bridge to a native module implementor.
 */
class library
{
public:

    /**
     * @brief       Constructor.
     *
     * @param[in]   modulePath      a path pointing the module to load without any extension information
     */
    library(const std::filesystem::path& module_path) noexcept;

    /**
     * @brief   Assignment operator.
     *
     * @remark  Assignment is forbidden for this class.
     */
    void operator=(const library&) noexcept = delete;

    /**
     * @brief   Destructor : does nothing.
     */
    ~library() noexcept =
        default;

    /**
     * @brief   Tells if the module is loaded.
     *
     * @return  true or false
     */
    [[nodiscard]] bool is_loaded() const noexcept
    {
        return m_implementor->is_loaded();
    }

    /**
     * @brief   Retrieves the file path of the library including the owning module's path.
     *
     * @return  a file path
     *
     * @see     getPath
     */
    [[nodiscard]] std::filesystem::path get_full_path() const
    {
        return m_implementor->get_full_path();
    }

    /**
     * @brief   Retrieves the file path of the library.
     *
     * @remark  This path is relative to the owning module's path.
     *
     * @return  a file path
     *
     * @see     getFullPath
     */
    [[nodiscard]] std::filesystem::path name() const
    {
        return m_implementor->name();
    }

    /**
     * @brief       Retrieves the address of a symbol specified by its name.
     *
     * @param[in]   name    a string containing the symbol name.
     *
     * @return      a pointer to the found symbol or null if none has been found
     */
    [[nodiscard]] void* get_symbol(const std::string& name) const
    {
        return m_implementor->get_symbol(name);
    }

    /**
     * @brief   Loads the module.
     */
    void load()
    {
        m_implementor->load();
    }

    /**
     * @brief       Set the initial path from which the library will be loaded.
     *
     * @param[in]   path  search path
     */
    void set_search_path(const std::filesystem::path& path) noexcept
    {
        m_implementor->set_search_path(path);
    }

    /**
     * @brief   Undloads the module.
     */
    void unload()
    {
        m_implementor->unload();
    }

private:

    /**
     * @brief   The native module implementor.
     */
    std::unique_ptr<native> m_implementor;
};

} // namespace sight::core::runtime::detail::dl

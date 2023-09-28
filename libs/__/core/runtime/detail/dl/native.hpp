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
#include "core/runtime/runtime_exception.hpp"

#include <filesystem>
#include <regex>

namespace sight::core::runtime::detail
{

class module;

namespace dl
{

/**
 * @brief   Defines the abstract class for native module implementors.
 * @struct  Native
 */
struct native
{
    friend class core::runtime::detail::module;

    /**
     * @brief       Constructor
     *
     * @param[in]   name    libray name
     */
    native(std::string name) noexcept;

    /**
     * @brief   Assignment operator.
     *
     * @remark  Assignment is forbidden for this class.
     */
    void operator=(const native&) noexcept = delete;

    /**
     * @brief   Destructor : does nothing.
     */
    virtual ~native() noexcept;

    /**
     * @brief   Tells if the module is loaded.
     *
     * @return  true or false
     */
    [[nodiscard]] virtual bool is_loaded() const noexcept = 0;

    /**
     * @brief       Retrieves the address of a symbol specified by its name.
     *
     * @param[in]   name    a string containing the symbol name.
     *
     * @return      a pointer to the found symbol or null if none has been found
     */
    [[nodiscard]] virtual void* get_symbol(const std::string& name) const = 0;

    /**
     * @brief   Loads the module.
     */
    virtual void load() = 0;

    /**
     * @brief   Undloads the module.
     */
    virtual void unload() = 0;

    /**
     * @brief       Retrieves the file path of the library including the owning module's path.
     *
     * @param[in]   _bMustBeFile  if true : module is a dynamic library and we want return an exception if is not a file
     *
     * @return      a file path
     *
     * @see         getPath
     */
    [[nodiscard]] std::filesystem::path get_full_path() const;

    /**
     * @brief   Retrieves the file name of the native library.
     *
     * @return  a string containing the native module file path
     */
    [[nodiscard]] std::string name() const;

    /**
     * @brief       Set the initial path from which the library will be loaded.
     *
     * @param[in]   path  search path
     */
    void set_search_path(const std::filesystem::path& path) noexcept;

    private:

        /**
         * @brief   The path to the module to load.
         */
        const std::string m_name;

        /**
         * @brief   A pointer to the module the library is attached to.
         */
        std::filesystem::path m_search_path;
};

} // namespace dl

} // namespace sight::core::runtime::detail

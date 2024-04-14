/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#if defined(__unix__)

#include "core/runtime/detail/dl/native.hpp"

#include <dlfcn.h>

namespace sight::core::runtime::detail::dl
{

/**
 * @brief   Implements a posix native module.
 * @struct  posix
 */
struct posix : public native
{
    /**
     * @brief       Constructor.
     *
     * @param[in]   modulePath  a path to the module to manage
     */
    posix(const std::filesystem::path& _module_path) noexcept;

    /**
     * @brief   Destructor.
     */
    ~posix() noexcept override;

    /**
     * @see core::runtime::dl::Native#isLoaded
     */
    [[nodiscard]] bool is_loaded() const noexcept override;

    /**
     * @brief       Retrieves the address of a symbol specified by its name.
     *
     * @param[in]   name    a string containing the symbol name.
     *
     * @return      a pointer to the found symbol or null if none has been found
     */
    [[nodiscard]] void* get_symbol(const std::string& _name) const override;

    /**
     * @see core::runtime::dl::Native#load
     */
    void load() override;

    /**
     * @see core::runtime::dl::Native#unload
     */
    void unload() override;

    private:

        /**
         * @brief   The handle of the loaded module.
         */
        void* m_handle {nullptr};
};

} // namespace sight::core::runtime::detail::dl

#endif // #ifdef __unix__

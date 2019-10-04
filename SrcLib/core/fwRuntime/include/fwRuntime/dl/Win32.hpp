/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#ifdef _WIN32

#include "fwRuntime/config.hpp"
#include "fwRuntime/dl/Native.hpp"

#ifdef NOMINMAX
#include <windows.h>
#else
#define NOMINMAX
#include <windows.h>
#undef NOMINMAX
#endif

namespace fwRuntime
{

namespace dl
{

/**
 * @brief   Implements a win32 native module.
 * @struct  Win32
 */
struct Win32 : public Native
{
    /**
     * @brief       Constructor.
     *
     * @param[in]   modulePath      a path to the module to manage
     */
    Win32( const std::filesystem::path& modulePath ) noexcept;

    /**
     * @brief   Tells if the module is loaded.
     *
     * @return  true or false
     */
    bool isLoaded() const noexcept;

    /**
     * @brief       Retrieves the address of a symbol specified by its name.
     *
     * @param[in]   name    a string containing the symbol name.
     *
     * @return      a pointer to the found symbol or null if none has been found
     */
    void* getSymbol(const std::string& name) const;

    /**
     * @brief   Loads the module.
     */
    void load();

    /**
     * @brief   Undloads the module.
     */
    void unload();

    private:

        /**
         * @brief   A handle of the loaded module.
         */
        HMODULE m_handle;

};

} // namespace dl

} // namespace fwRuntime

#endif // #ifdef _WIN32

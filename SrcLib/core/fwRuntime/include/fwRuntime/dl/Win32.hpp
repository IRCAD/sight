/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#ifdef _WIN32

#include "fwRuntime/config.hpp"
#include "fwRuntime/dl/Native.hpp"

#define NOMINMAX
#include <windows.h>

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
    Win32( const boost::filesystem::path& modulePath ) noexcept;

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

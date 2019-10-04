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

#include "fwRuntime/config.hpp"
#include "fwRuntime/RuntimeException.hpp"

#include <boost/regex.hpp>

#include <filesystem>

namespace fwRuntime
{

struct Bundle;

namespace dl
{

/**
 * @brief   Defines the abstract class for native module implementors.
 * @struct  Native
 */
struct Native
{
    friend struct ::fwRuntime::Bundle;

    /**
     * @brief       Constructor
     *
     * @param[in]   modulePath      a path to the module to manage
     */
    Native( const std::filesystem::path& modulePath) noexcept;

    /**
     * @brief   Destructor : does nothing.
     */
    virtual ~Native() noexcept;

    /**
     * @brief   Tells if the module is loaded.
     *
     * @return  true or false
     */
    virtual bool isLoaded() const noexcept = 0;

    /**
     * @brief       Retrieves the address of a symbol specified by its name.
     *
     * @param[in]   name    a string containing the symbol name.
     *
     * @return      a pointer to the found symbol or null if none has been found
     */
    virtual void* getSymbol( const std::string& name ) const = 0;

    /**
     * @brief   Loads the module.
     */
    virtual void load() = 0;

    /**
     * @brief   Undloads the module.
     */
    virtual void unload() = 0;

    /**
     * @brief       Retrieves the file path of the library including the owning bundle's path.
     *
     * @param[in]   _bMustBeFile  if true : bundle is a dynamic library and we want return an exception if is not a file
     *
     * @return      a file path
     *
     * @see         getPath
     */
    const std::filesystem::path getFullPath( const bool _bMustBeFile = false ) const;

    /**
     * @brief   Retrieves the file path of the native library.
     *
     * @note  This path is relative to the owning bundle's path.
     *
     * @return  a string containing the native module file path
     */
    const std::filesystem::path getPath() const;

    /**
     * @brief       Set the bundle the library is attached to.
     *
     * @param[in]   bundle  a pointer to a bundle instance
     */
    void setBundle( const ::fwRuntime::Bundle* bundle ) noexcept;

    /**
     * @brief  Retrieves the pattern of the dynamic library file name given the host OS
     * @return the boost::regex
     */
    const ::boost::regex getNativeName() const;

    private:

        /**
         * @brief  Returns the location of bundle library
         * @return The path of bundle library.
         */
        const std::filesystem::path getBundleLocation() const;

        /**
         * @brief   The path to the module to load.
         */
        const std::filesystem::path m_modulePath;

        /**
         * @brief   A pointer to the bundle the library is attached to.
         */
        const Bundle* m_bundle;

        /**
         * @brief   Assignment operator.
         *
         * @remark  Assignment is forbidden for this class.
         */
        void operator=( const Native& ) noexcept;

};

} // namesapce dl

} // namespace fwRuntime

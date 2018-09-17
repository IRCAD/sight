/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "fwRuntime/dl/Posix.hpp"
#include "fwRuntime/dl/Win32.hpp"
#include "fwRuntime/RuntimeException.hpp"

#include <boost/filesystem/path.hpp>

namespace fwRuntime
{

struct Bundle;

namespace dl
{

/**
 * @brief   Defines the module class.
 * @struct  LibraryBridge
 * This class is only a bridge to a native module implementor.
 */
template< typename Implementor >
struct LibraryBridge
{

    /**
     * @brief       Constructor.
     *
     * @param[in]   modulePath      a path pointing the module to load without any extension
     *                          information
     */
    LibraryBridge( const boost::filesystem::path& modulePath ) noexcept :
        m_implementor( modulePath )
    {
    }

    /**
     * @brief   Destructor : does nothing.
     */
    ~LibraryBridge() noexcept
    {
    }

    /**
     * @brief   Tells if the module is loaded.
     *
     * @return  true or false
     */
    bool isLoaded() const noexcept
    {
        return m_implementor.isLoaded();
    }

    /**
     * @brief   Retrieves the file path of the library including the owning bundle's path.
     *
     * @return  a file path
     *
     * @see     getPath
     */
    const boost::filesystem::path getFullPath() const
    {
        return m_implementor.getFullPath();
    }

    /**
     * @brief   Retrieves the file path of the library.
     *
     * @remark  This path is relative to the owning bundle's path.
     *
     * @return  a file path
     *
     * @see     getFullPath
     */
    const boost::filesystem::path getPath() const
    {
        return m_implementor.getPath();
    }

    /**
     * @brief       Retrieves the address of a symbol specified by its name.
     *
     * @param[in]   name    a string containing the symbol name.
     *
     * @return      a pointer to the found symbol or null if none has been found
     */
    void* getSymbol(const std::string& name) const
    {
        return m_implementor.getSymbol(name);
    }

    /**
     * @brief   Loads the module.
     */
    void load()
    {
        m_implementor.load();
    }

    /**
     * @brief       Sets the bundle the library is attached to.
     *
     * @param[in]   bundle  a pointer to a bundle instance
     */
    void setBundle(const ::fwRuntime::Bundle* bundle) noexcept
    {
        m_implementor.setBundle(bundle);
    }

    /**
     * @brief   Undloads the module.
     */
    void unload()
    {
        m_implementor.unload();
    }

    private:

        /**
         * @brief   The native module implementator.
         */
        Implementor m_implementor;

        /**
         * @brief   Assignment operator.
         *
         * @remark  Assignment is forbidden for this class.
         */
        void operator=(const LibraryBridge&) noexcept
        {
        }
};

#if defined(linux) || defined(__linux) || defined(__APPLE__)
typedef struct LibraryBridge< Posix > Library;
#else
typedef struct LibraryBridge< Win32 > Library;
#endif

} // namespace dl

} // namespace fwRuntime

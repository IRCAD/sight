/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWRUNTIME_DL_POSIX_HPP__
#define __FWRUNTIME_DL_POSIX_HPP__


#if defined(linux) || defined(__linux) || defined (__MACOSX__)


#include "fwRuntime/dl/Native.hpp"

#include <dlfcn.h>

namespace fwRuntime
{

namespace dl
{


/**
 * @brief   Implements a posix native module.
 * @struct  Posix
 */
struct Posix : public Native
{

    /**
     * @brief       Constructor.
     *
     * @param[in]   modulePath  a path to the module to manage
     */
    Posix( const boost::filesystem::path & modulePath ) throw();

    /**
     * @brief   Destructor.
     */
    ~Posix() throw();


    /**
     * @see ::fwRuntime::dl::Native#isLoaded
     */
    bool isLoaded() const throw();

    /**
     * @brief       Retrieves the address of a symbol specified by its name.
     *
     * @param[in]   name    a string containing the symbol name.
     *
     * @return      a pointer to the found symbol or null if none has been found
     */
    void* getSymbol( const std::string & name ) const throw(RuntimeException);

    /**
     * @see ::fwRuntime::dl::Native#load
     */
    void load() throw(RuntimeException);

    /**
     * @see ::fwRuntime::dl::Native#unload
     */
    void unload() throw(RuntimeException);


    private:

        /**
         * @brief   The handle of the loaded module.
         */
        void * m_handle;

};


} // namespace dl

} // namespace fwRuntime


#endif // #ifdef linux


#endif // __FWRUNTIME_DL_POSIX_HPP__

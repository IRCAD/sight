/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWRUNTIME_DL_NATIVE_HPP
#define _FWRUNTIME_DL_NATIVE_HPP

#include <boost/filesystem/path.hpp>
#include <boost/shared_ptr.hpp>

#include "fwRuntime/config.hpp"
#include "fwRuntime/RuntimeException.hpp"


namespace fwRuntime
{


struct Bundle;


namespace dl
{



struct INameDecorator;



/**
 * @brief   Defines the abstract class for native module implementors.
 * @struct  Native
 * @date    2004-2009
 * @author  IRCAD (Research and Development Team).
 */
struct Native
{
    friend struct ::fwRuntime::Bundle;


    /**
     * @brief       Constructor
     *
     * @param[in]   modulePath      a path to the module to manage
     * @param[in]   nameDecorator   a shared pointer to a name decorator to use to retrieve the native file name
     */
    Native( const boost::filesystem::path & modulePath, const ::boost::shared_ptr< INameDecorator > nameDecorator ) throw();

    /**
     * @brief   Destructor : does nothing.
     */
    virtual ~Native() throw();


    /**
     * @brief   Tells if the module is loaded.
     *
     * @return  true or false
     */
    virtual const bool isLoaded() const throw() = 0;

    /**
     * @brief       Retrieves the address of a symbol specified by its name.
     *
     * @param[in]   name    a string containing the symbol name.
     *
     * @return      a pointer to the found symbol or null if none has been found
     */
    virtual void * getSymbol( const std::string& name ) const throw(RuntimeException) = 0;

    /**
     * @brief   Loads the module.
     */
    virtual void load() throw(RuntimeException) = 0;

    /**
     * @brief   Undloads the module.
     */
    virtual void unload() throw(RuntimeException) = 0;

    /**
     * @brief       Retrieves the file path of the library including the owning bundle's path.
     *
     * @param[in]   bundle is a dynamic library and we want return an exception if is not a file
     *
     * @return      a file path
     *
     * @see         getPath
     */
    const boost::filesystem::path getFullPath( const bool _bMustBeFile = false ) const throw(RuntimeException);

    /**
     * @brief   Retrieves the file path of the native library.
     *
     * @remark  This path is relative to the owning bundle's path.
     *
     * @return  a string containing the native module file path
     */
    const boost::filesystem::path getPath() const throw(RuntimeException);

    /**
     * @brief       Set the bundle the library is attached to.
     *
     * @param[in]   bundle  a pointer to a bundle instance
     */
    void setBundle( const ::fwRuntime::Bundle * bundle ) throw();


private:

    /**
     * @brief   The path to the module to load.
     */
    const boost::filesystem::path       m_modulePath;

    /**
     * @brief   a name decorator to use to retrieve the native path
     */
    const ::boost::shared_ptr< INameDecorator > m_nameDecorator;

    /**
     * @brief   A pointer to the bundle the library is attached to.
     */
    const Bundle                            * m_bundle;


    /**
     * @brief   Assignment operator.
     *
     * @remark  Assignment is forbidden for this class.
     */
    void operator=( const Native & ) throw();

};



} // namesapce dl

} // namespace fwRuntime


#endif // #define _FWRUNTIME_DL_NATIVE_HPP

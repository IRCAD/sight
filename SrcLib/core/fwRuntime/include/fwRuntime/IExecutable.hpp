/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWRUNTIME_EXECUTABLE_HPP
#define _FWRUNTIME_EXECUTABLE_HPP

#include <string>

#include "fwRuntime/config.hpp"
#include "fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp"



namespace fwRuntime
{


#define REGISTER_EXECUTABLE( type, id ) static ::fwRuntime::utils::GenericExecutableFactoryRegistrar< type > registrar( id );



struct Bundle;
struct ConfigurationElement;
struct Runtime;


/**
 * @brief   Defines the base executable interface.
 * @struct  IExecutable
 *
 * An executable object is an instance created by an extension
 * point of a plugin.
 *
 * @date    2004-2009
 * @author  IRCAD (Research and Development Team).
 */
struct FWRUNTIME_CLASS_API IExecutable
{
    friend struct Runtime;


    /**
     * @brief   Destructor : does nothing.
     */
    FWRUNTIME_API virtual ~IExecutable();


    /**
     * @brief   Retrieves the bundle the executable originates from.
     *
     * @return  a pointer to the originating bundle.
     */
    virtual ::boost::shared_ptr<Bundle> getBundle() const throw() = 0;

    /**
     * @brief       Initializes the executable instance with the specified
     *              configuration element.
     *
     * @param[in]   configuration   a shared pointer to the configuration element used to
     *              trigger this execution
     */
    virtual void setInitializationData( const ::boost::shared_ptr<ConfigurationElement> configuration ) throw() = 0;


protected:

    /**
     * @brief       Updates the bundle the executable originates from.
     *
     * @param[in]   bundle  a pointer to the bundle the executable originates from
     */
    virtual void setBundle( ::boost::shared_ptr< Bundle > bundle ) = 0;

};



} // namespace fwRuntime



#endif // _FWRUNTIME_EXECUTABLE_HPP

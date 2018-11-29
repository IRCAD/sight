/************************************************************************
 *
 * Copyright (C) 2009-2015 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
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

#ifndef __FWRUNTIME_IEXECUTABLE_HPP__
#define __FWRUNTIME_IEXECUTABLE_HPP__

#include <string>

#include "fwRuntime/config.hpp"
#include "fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp"



namespace fwRuntime
{


#define REGISTER_EXECUTABLE( type, id ) static ::fwRuntime::utils::GenericExecutableFactoryRegistrar< type > registrar( \
        id );



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
 *
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
    virtual std::shared_ptr<Bundle> getBundle() const noexcept = 0;

    /**
     * @brief       Initializes the executable instance with the specified
     *              configuration element.
     *
     * @param[in]   configuration   a shared pointer to the configuration element used to
     *              trigger this execution
     */
    virtual void setInitializationData( const std::shared_ptr<ConfigurationElement> configuration ) noexcept = 0;


    protected:

        /**
         * @brief       Updates the bundle the executable originates from.
         *
         * @param[in]   bundle  a pointer to the bundle the executable originates from
         */
        virtual void setBundle( std::shared_ptr< Bundle > bundle ) = 0;

};



} // namespace fwRuntime



#endif // __FWRUNTIME_IEXECUTABLE_HPP__

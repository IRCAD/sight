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

#ifndef __FWRUNTIME_EXECUTABLEFACTORY_HPP__
#define __FWRUNTIME_EXECUTABLEFACTORY_HPP__

#include <string>

#include "fwRuntime/config.hpp"
#include "fwRuntime/BundleElement.hpp"
#include "fwRuntime/RuntimeException.hpp"

namespace fwRuntime
{
struct IExecutable;
}



namespace fwRuntime
{


/**
 * @brief   Defines the abstract executable factory class.
 * @struct  ExecutableFactory
 * @date    2004-2009
 *
 */
struct FWRUNTIME_CLASS_API ExecutableFactory : public BundleElement
{

    /**
     * @name    Construction & Destruction
     *
     * @{
     */

    /**
     * @brief       Constructor
     *
     * @param[in]   type    a string containing a type identifier
     */
    FWRUNTIME_API ExecutableFactory( const std::string & type );

    /**
     * @brief   Destructor : does nothing
     */
    FWRUNTIME_API virtual ~ExecutableFactory();
    ///@}

    /**
     * @brief   Creates an executable object instance.
     *
     * @return  a pointer to an executable instance
     */
    virtual IExecutable* createExecutable() const = 0;


    /**
     * @brief   Retrieves the type of executable the factory is able to create.
     *
     * @return  a string containing an executable type
     */
    FWRUNTIME_API const std::string getType() const;


    private:


        std::string m_type; ///< a string containing the type identifier managed by the factory

};


} // namespace fwRuntime


#endif // __FWRUNTIME_EXECUTABLEFACTORY_HPP__

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

#ifndef __FWRUNTIME_UTILS_GENERICEXECUTABLEFACTORY_HPP__
#define __FWRUNTIME_UTILS_GENERICEXECUTABLEFACTORY_HPP__

#include <sstream>
#include <string>

#include "fwRuntime/Bundle.hpp"
#include "fwRuntime/ExecutableFactory.hpp"


namespace fwRuntime
{
namespace utils
{


/**
 * @brief   Defines a generic template executable factory class.
 * @struct  GenericExecutableFactory
 * @date    2004-2009
 *
 */
template<typename E>
struct GenericExecutableFactory : public ExecutableFactory
{
    /**
     * @brief       Constructor.
     *
     * @param[in]   type    a string containing the type supported by the factory
     */
    GenericExecutableFactory(const std::string& type)
        :   ExecutableFactory( type )
    {
    }

    /**
     * @brief   Destructor : does nothing.
     */
    virtual ~GenericExecutableFactory()
    {
    }


    /**
     * @brief   Creates an executable object instance.
     *
     * @return  a pointer to an executable instance
     */
    IExecutable* createExecutable() const
    {
        IExecutable * result( 0 );
        try
        {
            result = new E();
        }
        catch( std::exception & exception )
        {
            std::ostringstream buffer;
            buffer << "Unexpected exception caught while creating an ::fwRuntime::IExecutable instance. ";
            buffer << exception.what();
            throw RuntimeException( buffer.str() );
        }
        catch( ... )
        {
            throw RuntimeException( "Unexpected exception caught while creating an ::fwRuntime::IExecutable instance." );
        }
        return result;
    }


    private:

        /**
         * @brief   Assignment operator.
         *
         * @remark  Assignment forbidden for this class.
         */
        void operator=(const GenericExecutableFactory&) noexcept
        {
        }

};


} // namespace utils

} // namespace fwRuntime


#endif // __FWRUNTIME_UTILS_GENERICEXECUTABLEFACTORY_HPP__

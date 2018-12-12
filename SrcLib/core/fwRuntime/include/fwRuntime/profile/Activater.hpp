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

#ifndef __FWRUNTIME_PROFILE_ACTIVATER_HPP__
#define __FWRUNTIME_PROFILE_ACTIVATER_HPP__

#include <map>
#include <vector>
#include <string>
#include <boost/utility.hpp>

#include "fwRuntime/config.hpp"
#include "fwRuntime/Version.hpp"



namespace fwRuntime
{

namespace profile
{



/**
 * @brief   Activates a given bundle with optional parameters.
 * @class  Activater
 * @date    2007-2009.
 *
 */
class Activater : public boost::noncopyable
{
public:

    /**
     * @brief       Constructor
     *
     * @param[in]   identifier  a string containing a bundle identifier
     * @param[in]   version     a string a bundle version
     */
    FWRUNTIME_API Activater( const std::string & identifier, const std::string & version );


    /**
     * @brief       Adds a new parameter to the activater.
     *
     * @param[in]   identifier  a string containing the parameter name
     * @param[in]   value       a string containing the parameter value
     */
    FWRUNTIME_API void addParameter( const std::string & identifier, const std::string & value );

    /**
     * @brief       Adds a new disable extension point  to the activater.
     *
     * @param[in]   identifier    a string containing the parameter name
     */
    FWRUNTIME_API void addDisableExtensionPoint( const std::string & identifier );

    /**
     * @brief       Adds a new disable extension  to the activater.
     *
     * @param[in]   identifier    a string containing the parameter name
     */
    FWRUNTIME_API void addDisableExtension( const std::string & identifier );

    /**
     * @brief   Applies the activater on the system.
     *
     * @remark  This method should be called directly.
     */
    void apply();


private:

    typedef std::map< std::string, std::string >    ParameterContainer;
    typedef std::vector< std::string >              DisableExtensionPointContainer;
    typedef std::vector< std::string >              DisableExtensionContainer;


    const std::string m_identifier;                             ///< a bundle identifier
    const Version m_version;                                    ///< a bundle version

    ParameterContainer m_parameters;                            ///< all parameters
    DisableExtensionPointContainer m_disableExtensionPoints;    ///< all disable extension points
    DisableExtensionContainer m_disableExtensions;              ///< all disable extensions

};



} // namespace profile

} // namespace fwRuntime



#endif /*__FWRUNTIME_PROFILE_ACTIVATER_HPP__*/

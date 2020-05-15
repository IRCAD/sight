/************************************************************************
 *
 * Copyright (C) 2009-2020 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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
#include "fwRuntime/ConfigurationElement.hpp"

#include <boost/property_tree/ptree.hpp>

#include <libxml/tree.h>

namespace fwRuntime
{

/**
 * @brief   Defines the convert class.
 */
class FWRUNTIME_CLASS_API Convert
{
public:

    /**
     * @brief   Destructor : does nothing.
     */
    FWRUNTIME_API virtual ~Convert();

    /**
     * @brief   Build an xmlNodePtr from a ConfigurationElement
     */
    FWRUNTIME_API static xmlNodePtr toXml( std::shared_ptr< ::fwRuntime::ConfigurationElement > _cfgElement);

    /**
     * @brief   Build an std::string from a ConfigurationElement
     */
    FWRUNTIME_API std::string static toXmlString( ::fwRuntime::ConfigurationElement::sptr _cfgElement);

    /**
     * @brief   Build an xmlNodePtr with all running Modules
     */
    FWRUNTIME_API static xmlNodePtr runningModulesToXml( );

    FWRUNTIME_API static ::boost::property_tree::ptree toPropertyTree(
        ::fwRuntime::ConfigurationElement::csptr _cfgElement );

    FWRUNTIME_API static ::fwRuntime::ConfigurationElement::sptr fromPropertyTree( ::boost::property_tree::ptree pt );

private:

    static void fromConfigurationElementToXml(std::shared_ptr< ::fwRuntime::ConfigurationElement > _cfgElement,
                                              xmlNodePtr _node);

    /**
     * @brief   Constructor : does nothing.
     */
    Convert();

};

} // namespace fwRuntime

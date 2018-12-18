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

#ifndef __FWRUNTIME_ECONFIGURATIONELEMENT_HPP__
#define __FWRUNTIME_ECONFIGURATIONELEMENT_HPP__

#include "fwRuntime/ConfigurationElement.hpp"
#include "fwRuntime/config.hpp"

namespace fwRuntime
{


/**
 * @brief   Editable configuration element : allow the self configuration element building, which reading XML structure provided by bundle descriptions
 * @class   EConfigurationElement
 * @date    2006-2009
 *
 */
class FWRUNTIME_CLASS_API EConfigurationElement : public ::fwRuntime::ConfigurationElement
{
public:
    fwCoreClassDefinitionsWithFactoryMacro( (EConfigurationElement)(ConfigurationElement),
                                            ((( const std::string& ))),
                                            new EConfigurationElement
                                            );

    /**
     * @brief       Constructor.
     * @param[in]   name name of the configuration element
     */
    FWRUNTIME_API EConfigurationElement(const std::string& name);

    /**
     * @brief   Destructor : does nothing.
     */
    FWRUNTIME_API virtual ~EConfigurationElement();

    /**
     * @brief       Create and add a configuration element.
     * @param[in]   name name of the configuration element
     * @return      the created configuration element
     */
    FWRUNTIME_API EConfigurationElement::sptr addConfigurationElement( const std::string& name );

    /**
     * @brief       Adds a new configuration element to the extension.
     * @param[in]   element     a shared pointer to the configuration element to add
     */
    FWRUNTIME_API void addConfigurationElement( EConfigurationElement::sptr element );

    FWRUNTIME_API void setAttributeValue(const std::string& name, const std::string& value) noexcept;

    FWRUNTIME_API void setValue(const std::string& value) noexcept;
};

}

#endif /*__FWRUNTIME_ECONFIGURATIONELEMENT_HPP__*/

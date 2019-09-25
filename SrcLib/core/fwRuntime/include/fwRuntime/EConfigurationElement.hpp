/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

namespace fwRuntime
{

/**
 * @brief   Editable configuration element : allow the self configuration element building,
 *          which reading XML structure provided by bundle descriptions
 *
 */
class FWRUNTIME_CLASS_API EConfigurationElement : public ::fwRuntime::ConfigurationElement
{
public:

    /// Shared pointer type
    typedef std::shared_ptr < EConfigurationElement > sptr;
    /// Weak pointer type
    typedef std::weak_ptr < EConfigurationElement > wptr;
    /// Const shared pointer type
    typedef std::shared_ptr < const EConfigurationElement > csptr;

    /**
     * @brief       EConfigurationElement factory.
     *
     * @param[in]   name name of the configuration element
     */
    static sptr New(const std::string& name)
    {
        return std::make_shared<EConfigurationElement>(name);
    }

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

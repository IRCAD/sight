/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "core/config.hpp"
#include "core/runtime/ConfigurationElementContainer.hpp"
#include "core/runtime/RuntimeException.hpp"

#include <core/base.hpp>

#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace sight::core::runtime
{

class Module;
// Deprecated, kept for compatibility
typedef Module Bundle;

/**
 * @brief   Defines the configuration element class.
 * @deprecated use boost::property::ptree instead.
 */
struct CORE_CLASS_API ConfigurationElement : public ConfigurationElementContainer,
                                             public std::enable_shared_from_this<ConfigurationElement>
{
    SIGHT_DECLARE_CLASS(ConfigurationElement);

    /**
     * @brief       ConfigurationElement factory.
     *
     * @param[in]   module  a shared pointer to the module to the configuration element is attached to
     * @param[in]   name    a string containing the configuration element name
     */
    static sptr New(const std::shared_ptr<Module>& module, const std::string& name)
    {
        return std::make_shared<ConfigurationElement>(module, name);
    }

    /**
     * @brief   Defines the attribute pair type.
     */
    typedef std::pair<bool, std::string> AttributePair;

    /**
     * @brief   Defines the a class for attributes exceptions.
     */
    struct NoSuchAttribute : public core::Exception
    {
        NoSuchAttribute(const std::string& attr);
    };

    /**
     * @brief   Retrieves the module the configuration element is attached to.
     *
     * @return  a shared pointer to a module instance
     */
    CORE_API std::shared_ptr<Module> getModule() const noexcept;

    /**
     * @brief       Retrieves the value of an attribute for the specified name.
     *
     * This method will search for an attribute having the specified name and
     * return it's value. When no matching attribute is found, an empty string
     * is returned.
     *
     * @param[in]   name    a string containing an attribute value
     *
     * @return      a string containing the attribute value or nothing when none has
     *              been found
     *
     * @see     getExistingAttributeValue
     * @see     getSafeAttributeValue
     */
    CORE_API std::string getAttributeValue(const std::string& name) const noexcept;

    /**
     * @brief       Retrieves the value of an attribute for the specified name.
     *
     * This method will search for an attribute having the specified name and
     * return it's value. When no matching attribute is found, an object of
     * type NoSuchAttribute is thrown.
     *
     * @param[in]   name    a string containing an attribute name
     *
     * @return      a string containing an attribute value
     *
     * @exception   NoSuchAttribute no attribute with the specified name has been found
     *
     * @see         getAttributeValue
     * @see         getSafeAttributeValue
     */
    CORE_API std::string getExistingAttributeValue(const std::string& name) const;

    /**
     * @brief       Retrieves the value of an existing attribute for the specified name.
     *
     * The returned value is a pair where the first element is a boolean telling
     * if the attribute really exists and the second element is the attribute value.
     *
     * @param[in]   name    a string containing an attribute name
     *
     * @return      a pair where the first element is a boolean telling if the attribute
     *              really exists and the second element is the attribute value
     *
     * @see         getAttributeValue
     * @see         getExistingAttributeValue
     */
    CORE_API AttributePair getSafeAttributeValue(const std::string& name) const noexcept;

    /**
     * @brief   Retrieves the name of the configuration element.
     *
     * @return  a string containing the configuration element name
     */
    CORE_API std::string getName() const noexcept;

    /**
     * @brief   Retrieves the configuration element value.
     *
     * @return  a string containing the configuration element value
     */
    CORE_API std::string getValue() const noexcept;

    /**
     * @brief       Tells if the specified attributes exists.
     *
     * @param[in]   name    a string containing an attribute name
     *
     * @return      true or false
     */
    CORE_API bool hasAttribute(const std::string& name) const noexcept;

    /**
     * @brief   Return the map with attributes
     */
    CORE_API std::map<std::string, std::string> getAttributes() const noexcept;

    /**
     * @brief   Print contents
     */
    CORE_API friend std::ostream& operator<<(
        std::ostream& _sstream,
        ConfigurationElement& _configurationElement
    );

    /**
     * @brief       Find recursively all the corresponding configuration elements.
     *
     * @param[in]   name            name of the configuration element (if name is empty, find all name)
     * @param[in]   attribute       attribute which must be contained by the configuration element
     *                              ( if attribute is empty, this doesn't check the attribute)
     * @param[in]   attributeValue  value of the attribute which must be contained by the configuration element
     *                              ( if value is empty, this doesn't check the value )
     * @param[in]   depth           depth of the recursion
     */
    CORE_API std::vector<ConfigurationElement::sptr> find(
        std::string name           = "",
        std::string attribute      = "",
        std::string attributeValue = "",
        int depth                  = 1
    );

    /**
     * @brief       Used only because this class inherit from enable_shared_from_this
     */
    CORE_API virtual ~ConfigurationElement();

    /**
     * @brief   Assignment operator.
     *
     * @remark  Assignment is forbidden for this class.
     */
    void operator=(const ConfigurationElement&) noexcept = delete;

    //TODO: Fix visibility
    //protected:

    /**
     * @brief       Constructor.
     *
     * @param[in]   module  a shared pointer to the module to the configuration element is attached to
     * @param[in]   name    a string containing the configuration element name
     *
     * @todo        test parameters validity
     */
    CORE_API ConfigurationElement(const std::shared_ptr<Module> module, std::string name);

    /**
     * @brief       Sets an attribute with the specified name and value.
     *
     * Setting a value for an existing attribute will override the old value.
     *
     * @param[in]   name    a string containing the attribute name
     * @param[in]   value   a string containing the attribute value
     */
    CORE_API void setAttributeValue(const std::string& name, const std::string& value) noexcept;

    /**
     * @brief       Sets the value of the configuration element it-self.
     *
     * @param[in]   value   a string containing the new configuration element value
     */
    CORE_API void setValue(const std::string& value) noexcept;

    private:

        /**
         * @brief   Defines the attributes container type.
         */
        typedef std::map<std::string, std::string> AttributeContainer;

        /**
         * @brief   A string containing the configuration element name.
         */
        const std::string m_name;

        /**
         * @brief   A string containing the configuration element value.
         */
        std::string m_value;

        /**
         * @brief   All attributes of the configuration element.
         */
        AttributeContainer m_attributes;

        /**
         * @brief   A pointer to the module the configuration element is attached to.
         */
        const std::weak_ptr<Module> m_module;
};

} // namespace sight::core::runtime

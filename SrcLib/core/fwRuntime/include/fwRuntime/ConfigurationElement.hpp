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
#include "fwRuntime/ConfigurationElementContainer.hpp"
#include "fwRuntime/RuntimeException.hpp"

#include <fwCore/base.hpp>

#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace fwRuntime
{
struct Bundle;
struct Executable;

namespace io
{
struct BundleDescriptorReader;
}
}

namespace fwRuntime
{

/**
 * @brief   Defines the configuration element class.
 * @deprecated use ::boost::property::ptree instead.
 */
struct FWRUNTIME_CLASS_API ConfigurationElement :   public ConfigurationElementContainer,
                                                    public std::enable_shared_from_this< ConfigurationElement >
{
    friend struct ::fwRuntime::io::BundleDescriptorReader;

    __FWCORE_CLASS_TYPEDEFS((ConfigurationElement));

    /**
     * @brief       ConfigurationElement factory.
     *
     * @param[in]   bundle  a shared pointer to the bundle to the configuration element is attached to
     * @param[in]   name    a string containing the configuration element name
     */
    static sptr New(const std::shared_ptr<Bundle>& bundle, const std::string& name)
    {
        return sptr(new ConfigurationElement(bundle, name));
    }
    __FWCORE_GENERATE_CAST(__FWCORE_GET_CLASSNAME((ConfigurationElement)), __FWCORE_TYPEDEF_ROOTCLASS_NAME)
    fwCoreInterfaceMacro()
    fwCoreInterfaceIsTypeOfMacro((ConfigurationElement))

    /**
     * @brief   Defines the attribute pair type.
     */
    typedef std::pair< bool, std::string > AttributePair;

    /**
     * @brief   Defines the a class for attributes exceptions.
     */
    struct NoSuchAttribute : public ::fwCore::Exception
    {
        NoSuchAttribute(const std::string& attr) :
            ::fwCore::Exception(std::string("No such attribute: ") + attr)
        {
        }
    };

    /**
     * @brief   Retrieves the bundle the configuration element is attached to.
     *
     * @return  a shared pointer to a bundle instance
     */
    FWRUNTIME_API const std::shared_ptr< Bundle > getBundle() const noexcept;

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
    FWRUNTIME_API const std::string getAttributeValue(const std::string& name) const noexcept;

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
    FWRUNTIME_API const std::string getExistingAttributeValue(const std::string& name) const;

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
    FWRUNTIME_API const AttributePair getSafeAttributeValue(const std::string& name) const noexcept;

    /**
     * @brief   Retrieves the name of the configuration element.
     *
     * @return  a string containing the configuration element name
     */
    FWRUNTIME_API const std::string getName() const noexcept;

    /**
     * @brief   Retrieves the configuration element value.
     *
     * @return  a string containing the configuration element value
     */
    FWRUNTIME_API const std::string getValue() const noexcept;

    /**
     * @brief       Tells if the specified attributes exists.
     *
     * @param[in]   name    a string containing an attribute name
     *
     * @return      true or false
     */
    FWRUNTIME_API bool hasAttribute(const std::string& name) const noexcept;

    /**
     * @brief   Return the map with attributes
     */
    FWRUNTIME_API const std::map<std::string, std::string> getAttributes() const noexcept;

    /**
     * @brief   Print contents
     */
    FWRUNTIME_API friend std::ostream& operator<<(std::ostream& _sstream,
                                                  ConfigurationElement& _configurationElement);

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
    FWRUNTIME_API std::vector < ConfigurationElement::sptr > find(
        std::string name           = "",
        std::string attribute      = "",
        std::string attributeValue = "",
        int depth                  = 1
        );

    /**
     * @brief       Used only because this class inherit from enable_shared_from_this
     */
    FWRUNTIME_API virtual ~ConfigurationElement();

    protected:

        /**
         * @brief       Constructor.
         *
         * @param[in]   bundle  a shared pointer to the bundle to the configuration element is attached to
         * @param[in]   name    a string containing the configuration element name
         *
         * @todo        test parameters validity
         */
        FWRUNTIME_API ConfigurationElement(const std::shared_ptr<Bundle> bundle, const std::string& name);

        /**
         * @brief       Sets an attribute with the specified name and value.
         *
         * Setting a value for an existing attribute will override the old value.
         *
         * @param[in]   name    a string containing the attribute name
         * @param[in]   value   a string containing the attribute value
         */
        FWRUNTIME_API void setAttributeValue(const std::string& name, const std::string& value) noexcept;

        /**
         * @brief       Sets the value of the configuration element it-self.
         *
         * @param[in]   value   a string containing the new configuration element value
         */
        FWRUNTIME_API void setValue(const std::string& value) noexcept;

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
         * @brief   Assignemet operator.
         *
         * @remark  Assignement is forbidden for this class.
         */
        void operator=(const ConfigurationElement&) noexcept;

        /**
         * @brief   A pointer to the bundle the configuration element is attached to.
         */
        const std::weak_ptr<Bundle> m_bundle;
};

} // namespace fwRuntime

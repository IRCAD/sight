/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWRUNTIME_CONFIGURATIONELEMENT_HPP
#define _FWRUNTIME_CONFIGURATIONELEMENT_HPP

#include <map>
#include <string>
#include <utility>
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>

#include <fwCore/base.hpp>

#include "fwRuntime/config.hpp"
#include "fwRuntime/ConfigurationElementContainer.hpp"
#include "fwRuntime/RuntimeException.hpp"

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
 * @struct  ConfigurationElement
 * @date    2004-2009
 * @author  IRCAD (Research and Development Team).

 */
struct FWRUNTIME_CLASS_API ConfigurationElement :   public ConfigurationElementContainer,
                                public boost::enable_shared_from_this< ConfigurationElement >
{

    friend struct ::fwRuntime::io::BundleDescriptorReader;

    fwCoreClassDefinitionsWithFactoryMacro((ConfigurationElement), ((( const ::boost::shared_ptr< Bundle > ))(( const std::string ))), new ConfigurationElement);

    /**
     * @brief   Defines the attribute pair type.
     */
    typedef std::pair< bool, std::string > AttributePair;


    /**
     * @brief   Defines the a class for attributes exceptions.
     */
    struct NoSuchAttribute : public ::fwCore::Exception
    {
        NoSuchAttribute(const std::string &attr) : ::fwCore::Exception(std::string("No such attribute: ") + attr) {}
    };


    /**
     * @brief   Retrieves the bundle the configuration element is attached to.
     *
     * @return  a shared pointer to a bundle instance
     */
    FWRUNTIME_API const ::boost::shared_ptr< Bundle > getBundle() const throw();

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
    FWRUNTIME_API const std::string getAttributeValue(const std::string& name) const throw();

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
    FWRUNTIME_API const std::string getExistingAttributeValue(const std::string& name) const throw(NoSuchAttribute);

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
    FWRUNTIME_API const AttributePair getSafeAttributeValue(const std::string& name) const throw();

    /**
     * @brief   Retrieves the name of the configuration element.
     *
     * @return  a string containing the configuration element name
     */
    FWRUNTIME_API const std::string getName() const throw();

    /**
     * @brief   Retrieves the configuration element value.
     *
     * @return  a string containing the configuration element value
     */
    FWRUNTIME_API const std::string getValue() const throw();

    /**
     * @brief       Tells if the specified attributes exists.
     *
     * @param[in]   name    a string containing an attribute name
     *
     * @return      true or false
     */
    FWRUNTIME_API const bool hasAttribute(const std::string& name) const throw();

    /**
     * @brief   Return the map with attributes
     */
    FWRUNTIME_API const std::map<std::string, std::string> getAttributes() const throw() ;

    /**
     * @brief   Print contents
     */
    FWRUNTIME_API friend std::ostream & operator<<(std::ostream & _sstream, ConfigurationElement& _configurationElement) ;

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
            std::string name = "",
            std::string attribute = "",
            std::string attributeValue = "",
            int depth = 1
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
    FWRUNTIME_API ConfigurationElement(const ::boost::shared_ptr<Bundle> bundle, const std::string& name);


    /**
     * @brief       Sets an attribute with the specified name and value.
     *
     * Setting a value for an existing attribute will override the old value.
     *
     * @param[in]   name    a string containing the attribute name
     * @param[in]   value   a string containing the attribute value
     */
    FWRUNTIME_API void setAttributeValue(const std::string& name, const std::string& value) throw();

    /**
     * @brief       Sets the value of the configuration element it-self.
     *
     * @param[in]   value   a string containing the new configuration element value
     */
    FWRUNTIME_API void setValue(const std::string& value) throw();


private:

    /**
     * @brief   Defines the attributes container type.
     */
    typedef std::map<std::string, std::string> AttributeContainer;


    /**
     * @brief   A pointer to the bundle the configuration element is attached to.
     */
    const ::boost::shared_ptr<Bundle> m_bundle;

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
    void operator=(const ConfigurationElement&) throw();
};


} // namespace fwRuntime


#endif // #define  _FWRUNTIME_CONFIGURATIONELEMENT_HPP

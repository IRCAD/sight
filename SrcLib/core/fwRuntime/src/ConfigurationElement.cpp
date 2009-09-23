/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>

#include "fwRuntime/ConfigurationElement.hpp"

#include "fwRuntime/Bundle.hpp"
#include "fwRuntime/IExecutable.hpp"
#include "fwRuntime/RuntimeException.hpp"


namespace fwRuntime
{

std::ostream & operator<<(std::ostream & _sstream, ConfigurationElement& _configurationElement)
{
	_sstream << "Configuration element " << _configurationElement.getName() << " value = " << _configurationElement.getValue() << std::endl;
	for(ConfigurationElement::AttributeContainer::iterator iter = _configurationElement.m_attributes.begin() ; iter != _configurationElement.m_attributes.end() ; ++iter )
	{
		_sstream << "Id = " << iter->first << " with value " << iter->second << std::endl;
	}
	_sstream << "Subelement : " << std::endl;
	for(ConfigurationElementContainer::Container::iterator iter = _configurationElement.begin() ; iter != _configurationElement.end() ; ++iter )
	{

		_sstream << std::endl << *(*iter) << std::endl;
	}

	return _sstream ;
}


ConfigurationElement::ConfigurationElement( const ::boost::shared_ptr< Bundle > bundle, const std::string & name )
:	m_bundle( bundle	),
	m_name	( name		),
	m_value ( std::string("") )
{}




const ::boost::shared_ptr<Bundle> ConfigurationElement::getBundle() const throw()
{
	return m_bundle;
}



const std::string ConfigurationElement::getAttributeValue(const std::string& name) const throw()
{
	AttributeContainer::const_iterator foundPos = m_attributes.find(name);
	return foundPos == m_attributes.end() ? std::string() : foundPos->second;
}



const std::string ConfigurationElement::getExistingAttributeValue(const std::string& name) const throw(NoSuchAttribute)
{
	AttributeContainer::const_iterator foundPos = m_attributes.find(name);
	if(foundPos == m_attributes.end())
	{
		throw NoSuchAttribute();
	}
	return foundPos->second;
}



const ConfigurationElement::AttributePair ConfigurationElement::getSafeAttributeValue(const std::string& name) const throw()
{
	AttributeContainer::const_iterator foundPos = m_attributes.find(name);
	if(foundPos == m_attributes.end())
	{
		return AttributePair(false, std::string());
	}
	else
	{
		return AttributePair(true, foundPos->second);
	}
}



const std::string ConfigurationElement::getName() const throw()
{
	return m_name;
}



const std::string ConfigurationElement::getValue() const throw()
{
	return m_value;
}



const bool ConfigurationElement::hasAttribute(const std::string& name) const throw()
{
	AttributeContainer::const_iterator foundPos = m_attributes.find(name);
	return foundPos != m_attributes.end();
}

//--------------------Ajout------------------------------
const std::map<std::string, std::string> ConfigurationElement::getAttributes() const throw()
{
	return m_attributes;
}
//--------------------Fin----Ajout----------------------


void ConfigurationElement::setAttributeValue(const std::string& name, const std::string& value) throw()
{
	m_attributes[name] = value;
}



void ConfigurationElement::setValue(const std::string& value) throw()
{
	m_value = value;
}



void ConfigurationElement::operator=(const ConfigurationElement&) throw()
{}


std::vector < ConfigurationElement::sptr > ConfigurationElement::find(
            std::string name,
            std::string attribute,
            std::string attributeValue,
            int depth
            )
{

    typedef std::vector < ConfigurationElement::sptr > ConfVector;
    ConfVector result;

    bool nameOk = (name.empty() || this->getName() == name);
    bool attributeOk = (attribute.empty() || this->hasAttribute(attribute));
    bool attributeValueOk = (attributeValue.empty() || (this->hasAttribute(attribute) && this->getAttributeValue(attribute) == attributeValue));
    if (nameOk && attributeOk && attributeValueOk) {
        result.push_back(this->shared_from_this());
    }

    if (depth != 0) {
        ConfigurationElement::Iterator iter;
        for (iter = this->begin(); iter != this->end(); ++iter)
        {
            ConfVector deepConf;
            deepConf = (*iter)->find(name, attribute, attributeValue, depth-1);
            result.insert(result.end(), deepConf.begin(), deepConf.end());
        }
    }

    return result;
}

} // namespace fwRuntime

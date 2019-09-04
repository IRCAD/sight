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

#include "fwRuntime/ConfigurationElement.hpp"

#include "fwRuntime/Bundle.hpp"
#include "fwRuntime/IExecutable.hpp"
#include "fwRuntime/RuntimeException.hpp"

#include <fwCore/base.hpp>

namespace fwRuntime
{

//------------------------------------------------------------------------------

std::ostream& operator<<(std::ostream& _sstream, ConfigurationElement& _configurationElement)
{
    _sstream << "Configuration element " << _configurationElement.getName() << " value = " <<
        _configurationElement.getValue() << std::endl;
    for(ConfigurationElement::AttributeContainer::iterator iter = _configurationElement.m_attributes.begin();
        iter != _configurationElement.m_attributes.end(); ++iter )
    {
        _sstream << "Id = " << iter->first << " with value " << iter->second << std::endl;
    }
    _sstream << "Subelement : " << std::endl;
    for(ConfigurationElementContainer::Container::iterator iter = _configurationElement.begin();
        iter != _configurationElement.end(); ++iter )
    {

        _sstream << std::endl << *(*iter) << std::endl;
    }

    return _sstream;
}

//------------------------------------------------------------------------------

ConfigurationElement::ConfigurationElement( const std::shared_ptr< Bundle > bundle, const std::string& name ) :
    m_name(name),
    m_bundle(bundle)
{
}

//------------------------------------------------------------------------------

const std::shared_ptr<Bundle> ConfigurationElement::getBundle() const noexcept
{
    return m_bundle.lock();
}

//------------------------------------------------------------------------------

const std::string ConfigurationElement::getAttributeValue(const std::string& name) const noexcept
{
    AttributeContainer::const_iterator foundPos = m_attributes.find(name);
    return foundPos == m_attributes.end() ? std::string() : foundPos->second;
}

//------------------------------------------------------------------------------

const std::string ConfigurationElement::getExistingAttributeValue(const std::string& name) const
{
    AttributeContainer::const_iterator foundPos = m_attributes.find(name);
    if(foundPos == m_attributes.end())
    {
        FW_RAISE_EXCEPTION(NoSuchAttribute(name));
    }
    return foundPos->second;
}

//------------------------------------------------------------------------------

const ConfigurationElement::AttributePair ConfigurationElement::getSafeAttributeValue(const std::string& name) const
noexcept
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

//------------------------------------------------------------------------------

const std::string ConfigurationElement::getName() const noexcept
{
    return m_name;
}

//------------------------------------------------------------------------------

const std::string ConfigurationElement::getValue() const noexcept
{
    return m_value;
}

//------------------------------------------------------------------------------

bool ConfigurationElement::hasAttribute(const std::string& name) const noexcept
{
    AttributeContainer::const_iterator foundPos = m_attributes.find(name);
    return foundPos != m_attributes.end();
}

//------------------------------------------------------------------------------

const std::map<std::string, std::string> ConfigurationElement::getAttributes() const noexcept
{
    return m_attributes;
}

//------------------------------------------------------------------------------

void ConfigurationElement::setAttributeValue(const std::string& name, const std::string& value) noexcept
{
    m_attributes[name] = value;
}

//------------------------------------------------------------------------------

void ConfigurationElement::setValue(const std::string& value) noexcept
{
    m_value = value;
}

//------------------------------------------------------------------------------

void ConfigurationElement::operator=(const ConfigurationElement&) noexcept
{
}

//------------------------------------------------------------------------------

std::vector < ConfigurationElement::sptr > ConfigurationElement::find(
    std::string name,
    std::string attribute,
    std::string attributeValue,
    int depth
    )
{
    typedef std::vector < ConfigurationElement::sptr > ConfVector;
    ConfVector result;

    bool nameOk           = (name.empty() || this->getName() == name);
    bool attributeOk      = (attribute.empty() || this->hasAttribute(attribute));
    bool attributeValueOk =
        (attributeValue.empty() ||
         (this->hasAttribute(attribute) && this->getAttributeValue(attribute) == attributeValue));
    if (nameOk && attributeOk && attributeValueOk)
    {
        result.push_back(this->shared_from_this());
    }

    if (depth != 0)
    {
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

//-----------------------------------------------------------------------------

ConfigurationElement::~ConfigurationElement()
{
}

//-----------------------------------------------------------------------------

} // namespace fwRuntime

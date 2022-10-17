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

#include "core/runtime/ConfigurationElement.hpp"

#include "core/runtime/IExecutable.hpp"
#include "core/runtime/Module.hpp"
#include "core/runtime/RuntimeException.hpp"

#include <core/base.hpp>

#include <utility>

namespace sight::core::runtime
{

//------------------------------------------------------------------------------

std::ostream& operator<<(std::ostream& _sstream, ConfigurationElement& _configurationElement)
{
    _sstream << "Configuration element " << _configurationElement.getName() << " value = "
    << _configurationElement.getValue() << std::endl;
    for(auto& m_attribute : _configurationElement.m_attributes)
    {
        _sstream << "Id = " << m_attribute.first << " with value " << m_attribute.second << std::endl;
    }

    _sstream << "Subelement : " << std::endl;
    for(auto& iter : _configurationElement)
    {
        _sstream << std::endl << *iter << std::endl;
    }

    return _sstream;
}

//------------------------------------------------------------------------------

ConfigurationElement::ConfigurationElement(const std::shared_ptr<Module> module, std::string name) :
    m_name(std::move(name)),
    m_module(module)
{
}

//------------------------------------------------------------------------------

std::shared_ptr<Module> ConfigurationElement::getModule() const noexcept
{
    return m_module.lock();
}

//------------------------------------------------------------------------------

std::string ConfigurationElement::getAttributeValue(const std::string& name) const noexcept
{
    auto foundPos = m_attributes.find(name);
    return foundPos == m_attributes.end() ? std::string() : foundPos->second;
}

//------------------------------------------------------------------------------

std::string ConfigurationElement::getExistingAttributeValue(const std::string& name) const
{
    auto foundPos = m_attributes.find(name);
    if(foundPos == m_attributes.end())
    {
        SIGHT_THROW_EXCEPTION(NoSuchAttribute(name));
    }

    return foundPos->second;
}

//------------------------------------------------------------------------------

ConfigurationElement::AttributePair ConfigurationElement::getSafeAttributeValue(const std::string& name) const
noexcept
{
    auto foundPos = m_attributes.find(name);
    if(foundPos == m_attributes.end())
    {
        return {false, std::string()};
    }

    return {true, foundPos->second};
}

//------------------------------------------------------------------------------

std::string ConfigurationElement::getName() const noexcept
{
    return m_name;
}

//------------------------------------------------------------------------------

std::string ConfigurationElement::getValue() const noexcept
{
    return m_value;
}

//------------------------------------------------------------------------------

bool ConfigurationElement::hasAttribute(const std::string& name) const noexcept
{
    auto foundPos = m_attributes.find(name);
    return foundPos != m_attributes.end();
}

//------------------------------------------------------------------------------

std::map<std::string, std::string> ConfigurationElement::getAttributes() const noexcept
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

std::vector<ConfigurationElement::sptr> ConfigurationElement::find(
    std::string name,
    std::string attribute,
    std::string attributeValue,
    int depth
)
{
    using ConfVector = std::vector<ConfigurationElement::sptr>;
    ConfVector result;

    bool nameOk           = (name.empty() || this->getName() == name);
    bool attributeOk      = (attribute.empty() || this->hasAttribute(attribute));
    bool attributeValueOk =
        (attributeValue.empty()
         || (this->hasAttribute(attribute) && this->getAttributeValue(attribute) == attributeValue));
    if(nameOk && attributeOk && attributeValueOk)
    {
        result.push_back(this->shared_from_this());
    }

    if(depth != 0)
    {
        ConfigurationElement::Iterator iter;
        for(iter = this->begin() ; iter != this->end() ; ++iter)
        {
            ConfVector deepConf;
            deepConf = (*iter)->find(name, attribute, attributeValue, depth - 1);
            result.insert(result.end(), deepConf.begin(), deepConf.end());
        }
    }

    return result;
}

//-----------------------------------------------------------------------------

ConfigurationElement::~ConfigurationElement()
= default;

//-----------------------------------------------------------------------------

ConfigurationElement::NoSuchAttribute::NoSuchAttribute(const std::string& attr) :
    core::Exception(std::string("No such attribute: ") + attr)
{
}

//-----------------------------------------------------------------------------

} // namespace sight::core::runtime

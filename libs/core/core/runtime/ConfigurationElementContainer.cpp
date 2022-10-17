/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "core/runtime/ConfigurationElementContainer.hpp"

#include "core/runtime/ConfigurationElement.hpp"

#include <algorithm>
#include <utility>

namespace sight::core::runtime
{

namespace
{

/**
 * @brief   Implements a STL compliant predicate that test if a configuration
 *          element has the given name.
 *
 *
 */
struct HasName
{
    explicit HasName(std::string name) :
        m_name(std::move(name))
    {
    }

    //------------------------------------------------------------------------------

    bool operator()(ConfigurationElement::csptr element) const
    {
        return element->getName() == m_name;
    }

    private:

        std::string m_name;
};

} // namespace

//------------------------------------------------------------------------------

void ConfigurationElementContainer::addConfigurationElement(ConfigurationElement::sptr element)
{
    m_elements.push_back(element);
}

//------------------------------------------------------------------------------

ConfigurationElementContainer::Iterator ConfigurationElementContainer::begin()
{
    return m_elements.begin();
}

//------------------------------------------------------------------------------

ConfigurationElementContainer::Iterator ConfigurationElementContainer::end()
{
    return m_elements.end();
}

//------------------------------------------------------------------------------

const ConfigurationElementContainer::Container& ConfigurationElementContainer::getElements() const
{
    return m_elements;
}

//-----------------------------------------------------------------------------

bool ConfigurationElementContainer::hasConfigurationElement(const std::string& name) const
{
    auto found = std::ranges::find_if(m_elements, HasName(name));
    return found != m_elements.end();
}

//-----------------------------------------------------------------------------

std::shared_ptr<ConfigurationElement> ConfigurationElementContainer::findConfigurationElement(
    const std::string& name
) const
{
    auto found = std::ranges::find_if(m_elements, HasName(name));

    return (found == m_elements.end()) ? std::shared_ptr<ConfigurationElement>() : *found;
}

//-----------------------------------------------------------------------------

ConfigurationElementContainer ConfigurationElementContainer::findAllConfigurationElement(const std::string& _name)
const
{
    ConfigurationElementContainer container;

    for(const auto& m_element : m_elements)
    {
        if(m_element->getName() == _name)
        {
            container.addConfigurationElement(m_element);
        }
    }

    return container;
}

//-----------------------------------------------------------------------------

std::size_t ConfigurationElementContainer::size() const
{
    return m_elements.size();
}

//-----------------------------------------------------------------------------

} // namespace sight::core::runtime

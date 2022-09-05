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

#include "core/runtime/EConfigurationElement.hpp"

#include "core/runtime/Module.hpp"

namespace sight::core::runtime
{

EConfigurationElement::EConfigurationElement(const std::string& name) :
    core::runtime::ConfigurationElement(std::shared_ptr<core::runtime::Module>(), name)
{
}

EConfigurationElement::~EConfigurationElement()
= default;

//------------------------------------------------------------------------------

EConfigurationElement::sptr EConfigurationElement::addConfigurationElement(const std::string& name)
{
    EConfigurationElement::sptr cfg = EConfigurationElement::New(name);
    this->core::runtime::ConfigurationElement::addConfigurationElement(cfg);
    return cfg;
}

//------------------------------------------------------------------------------

void EConfigurationElement::addConfigurationElement(EConfigurationElement::sptr element)
{
    this->core::runtime::ConfigurationElement::addConfigurationElement(element);
}

//------------------------------------------------------------------------------

void EConfigurationElement::setAttributeValue(const std::string& name, const std::string& value) noexcept
{
    this->core::runtime::ConfigurationElement::setAttributeValue(name, value);
}

//------------------------------------------------------------------------------

void EConfigurationElement::setValue(const std::string& value) noexcept
{
    this->core::runtime::ConfigurationElement::setValue(value);
}

} // namespace sight::core::runtime

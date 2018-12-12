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

/**
 * @file fwGui/builder/IContainerBuilder.cpp
 * @brief This file defines the implementation of the interface class  for the menubar builder.
 *
 *
 * @date 2009-2010
 */

#include "fwGui/builder/IContainerBuilder.hpp"

namespace fwGui
{
namespace builder
{

const IContainerBuilder::RegistryKeyType IContainerBuilder::REGISTRY_KEY = "::fwGui::ContainerBuilder";

//-----------------------------------------------------------------------------

IContainerBuilder::IContainerBuilder()
{
}

//-----------------------------------------------------------------------------

IContainerBuilder::~IContainerBuilder()
{
}

//-----------------------------------------------------------------------------

void IContainerBuilder::initialize( ::fwRuntime::ConfigurationElement::sptr configuration)
{
}

//-----------------------------------------------------------------------------

::fwGui::container::fwContainer::sptr IContainerBuilder::getContainer()
{
    return this->m_container;
}

//-----------------------------------------------------------------------------

} // namespace builder
} // namespace fwGui




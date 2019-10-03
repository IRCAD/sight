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

/**
 * @file fwGui/builder/IMenuBarBuilder.cpp
 * @brief This file defines the implementation of the interface class  for the menubar builder.
 */

#include "fwGui/builder/IMenuBarBuilder.hpp"

namespace fwGui
{
namespace builder
{

const IMenuBarBuilder::RegistryKeyType IMenuBarBuilder::REGISTRY_KEY = "::fwGui::MenuBarBuilder";

//-----------------------------------------------------------------------------

IMenuBarBuilder::IMenuBarBuilder()
{
}

//-----------------------------------------------------------------------------

IMenuBarBuilder::~IMenuBarBuilder()
{
}

//-----------------------------------------------------------------------------

void IMenuBarBuilder::initialize( ::fwRuntime::ConfigurationElement::sptr configuration)
{
    if (configuration->hasAttribute("backgroundColor"))
    {
        const std::string hexaColor = configuration->getExistingAttributeValue("backgroundColor");
        if(!hexaColor.empty())
        {
            OSLM_ASSERT(
                "Color string should start with '#' and followed by 6 ou 8 "
                "hexadecimal digits. Given color : " << hexaColor,
                    hexaColor[0] == '#'
                    && ( hexaColor.length() == 7 || hexaColor.length() == 9)
                );
            m_backgroundColor = hexaColor;
        }
    }
}

//-----------------------------------------------------------------------------

::fwGui::container::fwMenuBar::sptr IMenuBarBuilder::getMenuBar()
{
    return this->m_menuBar;
}

//-----------------------------------------------------------------------------

} // namespace builder
} // namespace fwGui

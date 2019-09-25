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
 * @file fwGui/builder/IToolBarBuilder.cpp
 * @brief This file defines the implementation of the interface class  for the toolbar builder.
 */

#include "fwGui/builder/IToolBarBuilder.hpp"

#include <boost/lexical_cast.hpp>

namespace fwGui
{
namespace builder
{

const IToolBarBuilder::RegistryKeyType IToolBarBuilder::REGISTRY_KEY = "::fwGui::ToolBarBuilder";

//-----------------------------------------------------------------------------

IToolBarBuilder::IToolBarBuilder() :
    m_aligment(TOP)
{
    m_toolBitmapSize = std::make_pair(32, 32);
}

//-----------------------------------------------------------------------------

IToolBarBuilder::~IToolBarBuilder()
{
}

//-----------------------------------------------------------------------------

void IToolBarBuilder::initialize( ::fwRuntime::ConfigurationElement::sptr configuration)
{
    OSLM_ASSERT("Bad configuration name "<<configuration->getName()<< ", must be toolBar",
                configuration->getName() == "toolBar");

    if (configuration->hasAttribute("align"))
    {
        std::string aligment = configuration->getExistingAttributeValue("align");
        if (aligment == "top")
        {
            m_aligment = TOP;
        }
        else if (aligment == "bottom")
        {
            m_aligment = BOTTOM;
        }
        else if (aligment == "right")
        {
            m_aligment = RIGHT;
        }
        else if (aligment == "left")
        {
            m_aligment = LEFT;
        }
        else
        {
            OSLM_FATAL("Wrong value '"<< aligment <<"' for 'align' attribute (require top, bottom, right or left)");
        }
    }

    if (configuration->hasAttribute("backgroundColor"))
    {
        const std::string hexaColor = configuration->getExistingAttributeValue("backgroundColor");
        if(hexaColor != "default")
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

    ::fwRuntime::ConfigurationElementContainer::Iterator iter;
    for( iter = configuration->begin(); iter != configuration->end(); ++iter )
    {
        if( (*iter)->getName() == "toolBitmapSize" )
        {
            if((*iter)->hasAttribute("height"))
            {
                m_toolBitmapSize.second = ::boost::lexical_cast< int > ((*iter)->getExistingAttributeValue("height"));
            }
            if((*iter)->hasAttribute("width"))
            {
                m_toolBitmapSize.first = ::boost::lexical_cast< int > ((*iter)->getExistingAttributeValue("width"));
            }
        }
    }
}

//-----------------------------------------------------------------------------

::fwGui::container::fwToolBar::sptr IToolBarBuilder::getToolBar()
{
    return this->m_toolBar;
}

//-----------------------------------------------------------------------------

} // namespace builder
} // namespace fwGui

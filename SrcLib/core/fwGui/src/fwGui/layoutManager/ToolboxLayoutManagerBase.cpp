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
 * @file fwGui/layoutManager/ToolboxLayoutManagerBase.cpp
 * @brief This file defines the implementation of the class for managing a line layout geometry.
 *
 */

#include "fwGui/layoutManager/ToolboxLayoutManagerBase.hpp"

#include <fwCore/base.hpp>

#include <boost/lexical_cast.hpp>

namespace fwGui
{

namespace layoutManager
{
//-----------------------------------------------------------------------------

const ToolboxLayoutManagerBase::RegistryKeyType ToolboxLayoutManagerBase::REGISTRY_KEY =
    "::fwGui::ToolboxLayoutManager";

//-----------------------------------------------------------------------------

ToolboxLayoutManagerBase::ToolboxLayoutManagerBase()
{
}

//-----------------------------------------------------------------------------

ToolboxLayoutManagerBase::~ToolboxLayoutManagerBase()
{
}

//-----------------------------------------------------------------------------

void ToolboxLayoutManagerBase::initialize( ConfigurationType configuration)
{
    OSLM_ASSERT("Bad configuration name "<<configuration->getName()<< ", must be layout",
                configuration->getName() == "layout");
    m_views.clear();
    for (ConfigurationType view : configuration->getElements())
    {
        if( view->getName() == "view" )
        {
            ViewInfo vi;
            if( view->hasAttribute("border") )
            {
                std::string border = view->getExistingAttributeValue("border");
                vi.m_border = ::boost::lexical_cast< int >(border);
            }
            if( view->hasAttribute("caption") )
            {
                vi.m_caption = view->getExistingAttributeValue("caption");
            }
            if( view->hasAttribute("minWidth") )
            {
                std::string width = view->getExistingAttributeValue("minWidth");
                vi.m_minSize.first = ::boost::lexical_cast< int >(width);
            }
            if( view->hasAttribute("minHeight") )
            {
                std::string height = view->getExistingAttributeValue("minHeight");
                vi.m_minSize.second = ::boost::lexical_cast< int >(height);
            }
            if( view->hasAttribute("visible") )
            {
                std::string visible = view->getExistingAttributeValue("visible");
                OSLM_ASSERT("Incorrect value for \"visible\" attribute "<<visible,
                            (visible == "true") || (visible == "false") ||
                            (visible == "yes") || (visible == "no"));
                vi.m_visible = ((visible == "true") || (visible == "yes"));
            }
            if( view->hasAttribute("expanded") )
            {
                std::string expanded = view->getExistingAttributeValue("expanded");
                OSLM_ASSERT("Incorrect value for \"expanded\" attribute "<<expanded,
                            (expanded == "true") || (expanded == "false") ||
                            (expanded == "yes") || (expanded == "no"));
                vi.m_expanded = ((expanded == "true") || (expanded == "yes"));
            }
            if( view->hasAttribute("useScrollBar") )
            {
                std::string useScrollBar = view->getExistingAttributeValue("useScrollBar");
                OSLM_ASSERT("Incorrect value for \"useScrollBar\" attribute "<<useScrollBar,
                            (useScrollBar == "yes") || (useScrollBar == "no"));
                vi.m_useScrollBar = (useScrollBar == "yes");
            }
            if( view->hasAttribute("backgroundColor") )
            {
                const std::string hexaColor = view->getExistingAttributeValue("backgroundColor");
                if(!hexaColor.empty())
                {
                    OSLM_ASSERT(
                        "Color string should start with '#' and followed by 6 ou 8 "
                        "hexadecimal digits. Given color : " << hexaColor,
                            hexaColor[0] == '#'
                            && ( hexaColor.length() == 7 || hexaColor.length() == 9)
                        );
                    vi.m_backgroundColor = hexaColor;
                }
            }
            m_views.push_back(vi);
        }
    }
}

//-----------------------------------------------------------------------------

} // namespace layoutManager
} // namespace fwGui

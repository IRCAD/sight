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

#include "fwGui/layoutManager/TabLayoutManagerBase.hpp"

#include <fwCore/base.hpp>

namespace fwGui
{

namespace layoutManager
{
//-----------------------------------------------------------------------------

const TabLayoutManagerBase::RegistryKeyType TabLayoutManagerBase::REGISTRY_KEY = "::fwGui::TabLayoutManager";

//-----------------------------------------------------------------------------

TabLayoutManagerBase::TabLayoutManagerBase()
{
}

//-----------------------------------------------------------------------------

TabLayoutManagerBase::~TabLayoutManagerBase()
{
}

//-----------------------------------------------------------------------------

void TabLayoutManagerBase::initialize( ConfigurationType configuration)
{
    OSLM_ASSERT("Bad configuration name "<<configuration->getName()<< ", must be layout",
                configuration->getName() == "layout");

    const std::vector < ConfigurationType > vectViews = configuration->find("view");
    SLM_TRACE_IF("No view define.", vectViews.empty() );
    m_views.clear();
    for (const ConfigurationType& view : vectViews)
    {
        ViewInfo vi;
        if( view->hasAttribute("border") )
        {
            const std::string border = view->getExistingAttributeValue("border");
            vi.m_border = std::stoi(border);
        }
        if( view->hasAttribute("caption") )
        {
            vi.m_caption = view->getExistingAttributeValue("caption");
        }
        if( view->hasAttribute("minWidth") )
        {
            const std::string width = view->getExistingAttributeValue("minWidth");
            vi.m_minSize.first = std::stoi(width);
        }
        if( view->hasAttribute("minHeight") )
        {
            const std::string height = view->getExistingAttributeValue("minHeight");
            vi.m_minSize.second = std::stoi(height);
        }
        if( view->hasAttribute("selected") )
        {
            const std::string isSelected = view->getExistingAttributeValue("selected");
            OSLM_ASSERT("The value "<<isSelected<<" it's incorrect, it should either be yes or no.",
                        isSelected == "yes" || isSelected == "no");
            vi.m_isSelect = (isSelected == "yes");
        }

        if( view->hasAttribute("useScrollBar") )
        {
            const std::string useScrollBar = view->getExistingAttributeValue("useScrollBar");
            OSLM_ASSERT("Incorrect value for \"useScrollBar\" attribute "<<useScrollBar,
                        (useScrollBar == "yes") || (useScrollBar == "no"));
            vi.m_useScrollBar = (useScrollBar == "yes");
        }

        if( view->hasAttribute("backgroundColor") )
        {
            const std::string hexaColor = view->getExistingAttributeValue("backgroundColor");
            if(hexaColor != "default")
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

//-----------------------------------------------------------------------------

} // namespace layoutManager
} // namespace fwGui

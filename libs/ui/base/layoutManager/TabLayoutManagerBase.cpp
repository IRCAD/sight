/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "ui/base/layoutManager/TabLayoutManagerBase.hpp"

#include <core/base.hpp>

namespace sight::ui::base
{

namespace layoutManager
{
//-----------------------------------------------------------------------------

const TabLayoutManagerBase::RegistryKeyType TabLayoutManagerBase::REGISTRY_KEY = "::sight::ui::base::TabLayoutManager";

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
    SLM_ASSERT("Bad configuration name "<<configuration->getName()<< ", must be layout",
               configuration->getName() == "layout");

    const std::vector < ConfigurationType > vectViews = configuration->find("view");
    m_views.clear();
    for (const ConfigurationType& view : vectViews)
    {
        ViewInfo vi;
        if( view->hasAttribute("border") )
        {
            const std::string border = view->getExistingAttributeValue("border");
            vi.m_border = std::stoi(border);
        }
        else
        {
            if( view->hasAttribute("leftBorder") )
            {
                const std::string border = view->getExistingAttributeValue("leftBorder");
                vi.m_leftBorder = std::stoi(border);
            }
            if( view->hasAttribute("topBorder") )
            {
                const std::string border = view->getExistingAttributeValue("topBorder");
                vi.m_topBorder = std::stoi(border);
            }
            if( view->hasAttribute("rightBorder") )
            {
                const std::string border = view->getExistingAttributeValue("rightBorder");
                vi.m_rightBorder = std::stoi(border);
            }
            if( view->hasAttribute("bottomBorder") )
            {
                const std::string border = view->getExistingAttributeValue("bottomBorder");
                vi.m_bottomBorder = std::stoi(border);
            }
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
            SLM_ASSERT("The value "<<isSelected<<" it's incorrect, it should either be yes or no.",
                       isSelected == "yes" || isSelected == "no");
            vi.m_isSelect = (isSelected == "yes");
        }

        if( view->hasAttribute("useScrollBar") )
        {
            const std::string useScrollBar = view->getExistingAttributeValue("useScrollBar");
            SLM_ASSERT("Incorrect value for \"useScrollBar\" attribute "<<useScrollBar,
                       (useScrollBar == "yes") || (useScrollBar == "no"));
            vi.m_useScrollBar = (useScrollBar == "yes");
        }

        if( view->hasAttribute("backgroundColor") )
        {
            const std::string hexaColor = view->getExistingAttributeValue("backgroundColor");
            if(!hexaColor.empty())
            {
                SLM_ASSERT(
                    "Color string should start with '#' and followed by 6 or 8 "
                    "hexadecimal digits. Given color: " << hexaColor,
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
} // namespace sight::ui::base

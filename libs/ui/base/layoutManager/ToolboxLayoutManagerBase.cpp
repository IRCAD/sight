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

/**
 * @file ui/base/layoutManager/ToolboxLayoutManagerBase.cpp
 * @brief This file defines the implementation of the class for managing a line layout geometry.
 *
 */

#include "ui/base/layoutManager/ToolboxLayoutManagerBase.hpp"

#include <core/base.hpp>

namespace sight::ui::base
{

namespace layoutManager
{

//-----------------------------------------------------------------------------

const ToolboxLayoutManagerBase::RegistryKeyType ToolboxLayoutManagerBase::REGISTRY_KEY =
    "sight::ui::base::ToolboxLayoutManager";

//-----------------------------------------------------------------------------

ToolboxLayoutManagerBase::ToolboxLayoutManagerBase()
{
}

//-----------------------------------------------------------------------------

ToolboxLayoutManagerBase::~ToolboxLayoutManagerBase()
{
}

//-----------------------------------------------------------------------------

void ToolboxLayoutManagerBase::initialize(ConfigurationType configuration)
{
    SIGHT_ASSERT(
        "Bad configuration name " << configuration->getName() << ", must be layout",
        configuration->getName() == "layout"
    );
    m_views.clear();
    for(ConfigurationType view : configuration->getElements())
    {
        if(view->getName() == "view")
        {
            ViewInfo vi;
            if(view->hasAttribute("border"))
            {
                std::string border = view->getExistingAttributeValue("border");
                vi.m_border = std::stoi(border);
            }
            else
            {
                if(view->hasAttribute("leftBorder"))
                {
                    const std::string border = view->getExistingAttributeValue("leftBorder");
                    vi.m_leftBorder = std::stoi(border);
                }

                if(view->hasAttribute("topBorder"))
                {
                    const std::string border = view->getExistingAttributeValue("topBorder");
                    vi.m_topBorder = std::stoi(border);
                }

                if(view->hasAttribute("rightBorder"))
                {
                    const std::string border = view->getExistingAttributeValue("rightBorder");
                    vi.m_rightBorder = std::stoi(border);
                }

                if(view->hasAttribute("bottomBorder"))
                {
                    const std::string border = view->getExistingAttributeValue("bottomBorder");
                    vi.m_bottomBorder = std::stoi(border);
                }
            }

            if(view->hasAttribute("caption"))
            {
                vi.m_caption = view->getExistingAttributeValue("caption");
            }

            if(view->hasAttribute("minWidth"))
            {
                std::string width = view->getExistingAttributeValue("minWidth");
                vi.m_minSize.first = std::stoi(width);
            }

            if(view->hasAttribute("minHeight"))
            {
                std::string height = view->getExistingAttributeValue("minHeight");
                vi.m_minSize.second = std::stoi(height);
            }

            if(view->hasAttribute("visible"))
            {
                std::string visible = view->getExistingAttributeValue("visible");
                SIGHT_ASSERT(
                    "Incorrect value for \"visible\" attribute " << visible,
                    (visible == "true") || (visible == "false")
                );
                vi.m_visible = ((visible == "true"));
            }

            if(view->hasAttribute("expanded"))
            {
                std::string expanded = view->getExistingAttributeValue("expanded");
                SIGHT_ASSERT(
                    "Incorrect value for \"expanded\" attribute " << expanded,
                    (expanded == "true") || (expanded == "false")
                );
                vi.m_expanded = ((expanded == "true"));
            }

            if(view->hasAttribute("useScrollBar"))
            {
                std::string useScrollBar = view->getExistingAttributeValue("useScrollBar");
                SIGHT_ASSERT(
                    "Incorrect value for \"useScrollBar\" attribute " << useScrollBar,
                    (useScrollBar == "true") || (useScrollBar == "false")
                );
                vi.m_useScrollBar = (useScrollBar == "true");
            }

            if(view->hasAttribute("backgroundColor"))
            {
                const std::string hexaColor = view->getExistingAttributeValue("backgroundColor");
                if(!hexaColor.empty())
                {
                    SIGHT_ASSERT(
                        "Color string should start with '#' and followed by 6 or 8 "
                        "hexadecimal digits. Given color: " << hexaColor,
                        hexaColor[0] == '#'
                        && (hexaColor.length() == 7 || hexaColor.length() == 9)
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

} // namespace sight::ui::base

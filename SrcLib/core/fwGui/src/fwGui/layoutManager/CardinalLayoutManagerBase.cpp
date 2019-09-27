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
 * @brief This file defines the implementation of the class for managing a cardinal layout geometry.
 */

#include "fwGui/layoutManager/CardinalLayoutManagerBase.hpp"

#include <fwCore/base.hpp>

#include <boost/assign/list_of.hpp>
#include <boost/lexical_cast.hpp>

namespace fwGui
{

namespace layoutManager
{
//-----------------------------------------------------------------------------

const CardinalLayoutManagerBase::RegistryKeyType CardinalLayoutManagerBase::REGISTRY_KEY =
    "::fwGui::CardinalLayoutManager";

//-----------------------------------------------------------------------------

const std::map<std::string, CardinalLayoutManagerBase::Align> CardinalLayoutManagerBase::STRING_TO_ALIGN =
    ::boost::assign::map_list_of("center", CENTER)
        ("right", RIGHT)
        ("left", LEFT)
        ("bottom", BOTTOM)
        ("top", TOP);

//-----------------------------------------------------------------------------

CardinalLayoutManagerBase::CardinalLayoutManagerBase()
{
}

//-----------------------------------------------------------------------------

CardinalLayoutManagerBase::~CardinalLayoutManagerBase()
{
}

//-----------------------------------------------------------------------------

void CardinalLayoutManagerBase::initialize( ConfigurationType configuration)
{
    OSLM_ASSERT("Bad configuration name "<<configuration->getName()<< ", must be layout",
                configuration->getName() == "layout");

    std::vector < ConfigurationType > vectViews = configuration->find("view");
    SLM_TRACE_IF("No view define.", vectViews.empty() );
    m_views.clear();
    for (ConfigurationType view : vectViews)
    {
        ViewInfo vi;
        if( view->hasAttribute("align") )
        {
            const std::string align = view->getExistingAttributeValue("align");
            OSLM_ASSERT("Align "<<align<<" unknown", STRING_TO_ALIGN.find(align) != STRING_TO_ALIGN.end() );
            vi.m_align = STRING_TO_ALIGN.find(align)->second;
        }

        if( view->hasAttribute("minWidth") )
        {
            const std::string width = view->getExistingAttributeValue("minWidth");
            vi.m_minSize.first = ::boost::lexical_cast< int >(width);
        }

        if( view->hasAttribute("minHeight") )
        {
            const std::string height = view->getExistingAttributeValue("minHeight");
            vi.m_minSize.second = ::boost::lexical_cast< int >(height);
        }

        if( view->hasAttribute("resizable") )
        {
            const std::string resizable = view->getExistingAttributeValue("resizable");
            OSLM_ASSERT("Incorrect value for \"resizable\" attribute "<<resizable,
                        (resizable == "yes") || (resizable == "no"));
            vi.m_isResizable = (resizable == "yes");
        }

        if( view->hasAttribute("position") )
        {
            const std::string position = view->getExistingAttributeValue("position");
            vi.m_position = ::boost::lexical_cast< int >(position);
        }

        if( view->hasAttribute("layer") )
        {
            const std::string layer = view->getExistingAttributeValue("layer");
            vi.m_layer = ::boost::lexical_cast< int >(layer);
        }

        if( view->hasAttribute("row") )
        {
            const std::string row = view->getExistingAttributeValue("row");
            vi.m_row = ::boost::lexical_cast< int >(row);
        }

        if( view->hasAttribute("visible") )
        {
            const std::string visible = view->getExistingAttributeValue("visible");
            OSLM_ASSERT("Incorrect value for \"visible\" attribute "<<visible,
                        (visible == "true") || (visible == "false") ||
                        (visible == "yes") || (visible == "no"));
            vi.m_visible = ((visible == "true") || (visible == "yes"));
        }

        if( view->hasAttribute("caption") )
        {
            vi.m_caption.first  = true;
            vi.m_caption.second = view->getExistingAttributeValue("caption");
        }

        if( view->hasAttribute("useScrollBar") )
        {
            const std::string useScrollBar = view->getExistingAttributeValue("useScrollBar");
            OSLM_ASSERT("Incorrect value for \"useScrollBar\" attribute "<<useScrollBar,
                        (useScrollBar == "yes") || (useScrollBar == "no"));
            vi.m_useScrollBar = (useScrollBar == "yes");
        }
        if( view->hasAttribute("toolTip") )
        {
            const std::string toolTip = view->getExistingAttributeValue("toolTip");
            vi.m_toolTip = toolTip;
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

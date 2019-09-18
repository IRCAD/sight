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
 * @file fwGui/layoutManager/LineLayoutManagerBase.cpp
 * @brief This file defines the implementation of the class for managing a line layout geometry.
 *
 */

#include "fwGui/layoutManager/LineLayoutManagerBase.hpp"

#include <fwCore/base.hpp>

#include <boost/lexical_cast.hpp>

namespace fwGui
{

namespace layoutManager
{
//-----------------------------------------------------------------------------

const LineLayoutManagerBase::RegistryKeyType LineLayoutManagerBase::REGISTRY_KEY = "::fwGui::LineLayoutManager";

//-----------------------------------------------------------------------------

LineLayoutManagerBase::LineLayoutManagerBase() :
    m_orientation(VERTICAL)
{
}

//-----------------------------------------------------------------------------

LineLayoutManagerBase::~LineLayoutManagerBase()
{
}

//-----------------------------------------------------------------------------

void LineLayoutManagerBase::initialize( ConfigurationType configuration)
{
    OSLM_ASSERT("Bad configuration name "<<configuration->getName()<< ", must be layout",
                configuration->getName() == "layout");

    std::vector < ConfigurationType > vectOrientation = configuration->find("orientation");

    SLM_FATAL_IF( "missing orientation configuration", vectOrientation.empty() );
    SLM_ASSERT("<orientation> tag must have value attribute", vectOrientation.at(0)->hasAttribute("value"));
    std::string orientation = vectOrientation.at(0)->getExistingAttributeValue("value");
    SLM_ASSERT("Wrong value '"<< orientation <<"' for 'orientation' attribute (require vertical or horizontal)",
               orientation == "vertical" || orientation == "horizontal");
    m_orientation = (orientation == "vertical") ? VERTICAL : HORIZONTAL;

    m_views.clear();
    for (ConfigurationType view : configuration->getElements())
    {
        if( view->getName() == "spacer" )
        {
            ViewInfo vi;
            vi.m_isSpacer = true;
            m_views.push_back(vi);
        }
        else if( view->getName() == "view" )
        {
            ViewInfo vi;
            if( view->hasAttribute("proportion") )
            {
                const std::string proportion = view->getExistingAttributeValue("proportion");
                vi.m_proportion = ::boost::lexical_cast< int >(proportion);
            }
            if( view->hasAttribute("border") )
            {
                const std::string border = view->getExistingAttributeValue("border");
                vi.m_border = ::boost::lexical_cast< int >(border);
            }
            if( view->hasAttribute("caption") )
            {
                vi.m_caption.first  = true;
                vi.m_caption.second = view->getExistingAttributeValue("caption");
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
            if( view->hasAttribute("visible") )
            {
                const std::string visible = view->getExistingAttributeValue("visible");
                OSLM_ASSERT("Incorrect value for \"visible\" attribute "<<visible,
                            (visible == "true") || (visible == "false") ||
                            (visible == "yes") || (visible == "no"));
                vi.m_visible = ((visible == "true") || (visible == "yes"));
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
                std::string hexaColor = view->getExistingAttributeValue("backgroundColor");
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
}

//-----------------------------------------------------------------------------

} // namespace layoutManager
} // namespace fwGui

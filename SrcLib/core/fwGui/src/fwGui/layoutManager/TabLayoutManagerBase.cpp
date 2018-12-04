/************************************************************************
 *
 * Copyright (C) 2009-2016 IRCAD France
 * Copyright (C) 2012-2016 IHU Strasbourg
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
 * @file fwGui/layoutManager/TabLayoutManagerBase.cpp
 * @brief This file defines the interface of the class for managing a tab layout geometry.
 *
 *
 * @date 2009-2010
 */

#include "fwGui/layoutManager/TabLayoutManagerBase.hpp"

#include <fwCore/base.hpp>

#include <boost/lexical_cast.hpp>
#include <boost/assign/list_of.hpp>

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

    std::vector < ConfigurationType > vectViews = configuration->find("view");
    SLM_TRACE_IF("No view define.", vectViews.empty() );
    m_views.clear();
    for (ConfigurationType view : vectViews)
    {
        ViewInfo vi;
        if( view->hasAttribute("caption") )
        {
            vi.m_caption = view->getExistingAttributeValue("caption");
        }
        if( view->hasAttribute("selected") )
        {
            std::string isSelected = view->getExistingAttributeValue("selected");
            OSLM_ASSERT("The value "<<isSelected<<" it's incorrect, it should either be yes or no.",
                        isSelected == "yes" || isSelected == "no");
            vi.m_isSelect = (isSelected == "yes");
        }

        if( view->hasAttribute("useScrollBar") )
        {
            std::string useScrollBar = view->getExistingAttributeValue("useScrollBar");
            OSLM_ASSERT("Incorrect value for \"useScrollBar\" attribute "<<useScrollBar,
                        (useScrollBar == "yes") || (useScrollBar == "no"));
            vi.m_useScrollBar = (useScrollBar=="yes");
        }

        m_views.push_back(vi);
    }
}

//-----------------------------------------------------------------------------

} // namespace layoutManager
} // namespace fwGui




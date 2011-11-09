/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

/**
 * @file fwGui/ToolboxLayoutManagerBase.cpp
 * @brief This file defines the implementation of the class for managing a line layout geometry.
 *
 * @author IRCAD (Research and Development Team).
 * @date 2009-2010
 */

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

#include <fwCore/base.hpp>

#include "fwGui/layoutManager/ToolboxLayoutManagerBase.hpp"

namespace fwGui
{

namespace layoutManager
{
//-----------------------------------------------------------------------------

const ToolboxLayoutManagerBase::RegistryKeyType ToolboxLayoutManagerBase::REGISTRY_KEY =  "::fwGui::ToolboxLayoutManager";

//-----------------------------------------------------------------------------

ToolboxLayoutManagerBase::ToolboxLayoutManagerBase()
{}

//-----------------------------------------------------------------------------

ToolboxLayoutManagerBase::~ToolboxLayoutManagerBase()
{}

//-----------------------------------------------------------------------------

void ToolboxLayoutManagerBase::initialize( ConfigurationType configuration)
{
    OSLM_ASSERT("Bad configuration name "<<configuration->getName()<< ", must be layout",
            configuration->getName() == "layout");
    m_views.clear();
    BOOST_FOREACH (ConfigurationType view, configuration->getElements())
    {
        if( view->getName() == "view" )
        {
            ViewInfo vi;
            if( view->hasAttribute("border") )
            {
                std::string border = view->getExistingAttributeValue("border") ;
                vi.m_border = ::boost::lexical_cast< int >(border) ;
            }
            if( view->hasAttribute("caption") )
            {
                vi.m_caption = view->getExistingAttributeValue("caption") ;
            }
            if( view->hasAttribute("minWidth") )
            {
                std::string width = view->getExistingAttributeValue("minWidth") ;
                vi.m_minSize.first = ::boost::lexical_cast< int >(width) ;
            }
            if( view->hasAttribute("minHeight") )
            {
                std::string height = view->getExistingAttributeValue("minHeight") ;
                vi.m_minSize.second = ::boost::lexical_cast< int >(height) ;
            }
            if( view->hasAttribute("visible") )
            {
                std::string visible = view->getExistingAttributeValue("visible") ;
                OSLM_ASSERT("Incorrect value for \"visible\" attribute "<<visible,
                        (visible == "true") || (visible == "false") ||
                        (visible == "yes") || (visible == "no"));
                vi.m_visible = ((visible == "true") || (visible == "yes"));
            }
            if( view->hasAttribute("expanded") )
            {
                std::string expanded = view->getExistingAttributeValue("expanded") ;
                OSLM_ASSERT("Incorrect value for \"expanded\" attribute "<<expanded,
                        (expanded == "true") || (expanded == "false") ||
                        (expanded == "yes") || (expanded == "no"));
                vi.m_expanded = ((expanded == "true") || (expanded == "yes"));
            }
            if( view->hasAttribute("useScrollBar") )
            {
                std::string useScrollBar = view->getExistingAttributeValue("useScrollBar") ;
                OSLM_ASSERT("Incorrect value for \"useScrollBar\" attribute "<<useScrollBar,
                        (useScrollBar == "yes") || (useScrollBar == "no"));
                vi.m_useScrollBar = (useScrollBar=="yes") ;
            }
            m_views.push_back(vi);
        }
    }
}

//-----------------------------------------------------------------------------

} // namespace layoutManager
} // namespace fwGui




/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

/**
 * @file fwGui/LineLayoutManagerBase.cpp
 * @brief This file defines the implementation of the class for managing a line layout geometry.
 *
 * @author IRCAD (Research and Development Team).
 * @date 2009-2010
 */

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

#include <fwCore/base.hpp>

#include "fwGui/layoutManager/LineLayoutManagerBase.hpp"

namespace fwGui
{

namespace layoutManager
{
//-----------------------------------------------------------------------------

const LineLayoutManagerBase::RegistryKeyType LineLayoutManagerBase::REGISTRY_KEY =  "::fwGui::LineLayoutManager";

//-----------------------------------------------------------------------------

LineLayoutManagerBase::LineLayoutManagerBase() : m_orientation(VERTICAL)
{}

//-----------------------------------------------------------------------------

LineLayoutManagerBase::~LineLayoutManagerBase()
{}

//-----------------------------------------------------------------------------

void LineLayoutManagerBase::initialize( ConfigurationType configuration)
{
    OSLM_ASSERT("Bad configuration name "<<configuration->getName()<< ", must be layout",
            configuration->getName() == "layout");

    std::vector < ConfigurationType > vectOrientation = configuration->find("orientation");

    SLM_FATAL_IF( "missing orientation configuration" , vectOrientation.empty() );
    SLM_ASSERT("<orientation> tag must have value attribute", vectOrientation.at(0)->hasAttribute("value"));
    std::string orientation = vectOrientation.at(0)->getExistingAttributeValue("value") ;
    SLM_ASSERT("Wrong value '"<< orientation <<"' for 'orientation' attribute (require vertical or horizontal)",
            orientation == "vertical" || orientation == "horizontal");
    m_orientation = (orientation == "vertical")? VERTICAL:HORIZONTAL ;

    m_views.clear();
    BOOST_FOREACH (ConfigurationType view, configuration->getElements())
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
                std::string proportion = view->getExistingAttributeValue("proportion") ;
                vi.m_proportion = ::boost::lexical_cast< int >(proportion) ;
            }
            if( view->hasAttribute("border") )
            {
                std::string border = view->getExistingAttributeValue("border") ;
                vi.m_border = ::boost::lexical_cast< int >(border) ;
            }
            if( view->hasAttribute("caption") )
            {
                vi.m_caption.first = true;
                vi.m_caption.second = view->getExistingAttributeValue("caption") ;
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
            m_views.push_back(vi);
        }
    }
}

//-----------------------------------------------------------------------------

} // namespace layoutManager
} // namespace fwGui




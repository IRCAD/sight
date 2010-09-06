/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/assign/list_of.hpp>

#include <fwCore/base.hpp>

#include "fwGui/layoutManager/CardinalLayoutManagerBase.hpp"

namespace fwGui
{

namespace layoutManager
{
//-----------------------------------------------------------------------------

const CardinalLayoutManagerBase::RegistryKeyType CardinalLayoutManagerBase::REGISTRY_KEY =  "::fwGui::CardinalLayoutManager";

//-----------------------------------------------------------------------------

const std::map<std::string, CardinalLayoutManagerBase::Align> CardinalLayoutManagerBase::STRING_TO_ALIGN =
                                                ::boost::assign::map_list_of("center",CENTER)
                                                                            ("right",RIGHT)
                                                                            ("left",LEFT)
                                                                            ("bottom",BOTTOM)
                                                                            ("top",TOP);

//-----------------------------------------------------------------------------

CardinalLayoutManagerBase::CardinalLayoutManagerBase()
{}

//-----------------------------------------------------------------------------

CardinalLayoutManagerBase::~CardinalLayoutManagerBase()
{}

//-----------------------------------------------------------------------------

void CardinalLayoutManagerBase::initialize( ConfigurationType configuration)
{
    OSLM_ASSERT("Bad configuration name "<<configuration->getName()<< ", must be layout",
            configuration->getName() == "layout");

    std::vector < ConfigurationType > vectViews = configuration->find("view");
    SLM_TRACE_IF("No view define.", vectViews.empty() );
    m_views.clear();
    BOOST_FOREACH (ConfigurationType view, vectViews)
    {
        ViewInfo vi;
        if( view->hasAttribute("align") )
        {
            std::string align = view->getExistingAttributeValue("align");
            OSLM_ASSERT("Align "<<align<<" unknown", STRING_TO_ALIGN.find(align) != STRING_TO_ALIGN.end() );
            vi.m_align = STRING_TO_ALIGN.find(align)->second;
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

        if( view->hasAttribute("resizable") )
        {
            std::string resizable = view->getExistingAttributeValue("resizable") ;
            OSLM_ASSERT("Incorrect value for \"resizable\" attribute "<<resizable,
                    (resizable == "yes") || (resizable == "no"));
            vi.m_isResizable = (resizable=="yes") ;
        }

        if( view->hasAttribute("position") )
        {
            std::string position = view->getExistingAttributeValue("position") ;
            vi.m_position = ::boost::lexical_cast< int >(position);
        }

        if( view->hasAttribute("layer") )
        {
            std::string layer = view->getExistingAttributeValue("layer") ;
            vi.m_layer = ::boost::lexical_cast< int >(layer);
        }

        if( view->hasAttribute("row") )
        {
            std::string row = view->getExistingAttributeValue("row") ;
            vi.m_row = ::boost::lexical_cast< int >(row);
        }

        if( view->hasAttribute("visible") )
        {
            std::string visible = view->getExistingAttributeValue("visible") ;
            OSLM_ASSERT("Incorrect value for \"visible\" attribute "<<visible,
                    (visible == "true") || (visible == "false") ||
                    (visible == "yes") || (visible == "no"));
            vi.m_visible = ((visible == "true") || (visible == "yes"));
        }

        if( view->hasAttribute("caption") )
        {
            vi.m_caption.first = true;
            vi.m_caption.second = view->getExistingAttributeValue("caption") ;
        }

        m_views.push_back(vi);
    }
}

//-----------------------------------------------------------------------------

} // namespace layoutManager
} // namespace fwGui




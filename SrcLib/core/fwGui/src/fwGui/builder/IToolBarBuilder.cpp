/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

/**
 * @file fwGui/IToolBarBuilder.cpp
 * @brief This file defines the implementation of the interface class  for the toolbar builder.
 *
 * @author IRCAD (Research and Development Team).
 * @date 2009-2010
 */

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

#include "fwGui/builder/IToolBarBuilder.hpp"

namespace fwGui
{
namespace builder
{

const IToolBarBuilder::RegistryKeyType IToolBarBuilder::REGISTRY_KEY = "::fwGui::ToolBarBuilder";

//-----------------------------------------------------------------------------

IToolBarBuilder::IToolBarBuilder(): m_aligment(TOP)
{
    m_toolBitmapSize = std::make_pair(32, 32);
}

//-----------------------------------------------------------------------------

IToolBarBuilder::~IToolBarBuilder()
{}

//-----------------------------------------------------------------------------

void IToolBarBuilder::initialize( ::fwRuntime::ConfigurationElement::sptr configuration)
{
    OSLM_ASSERT("Bad configuration name "<<configuration->getName()<< ", must be toolBar",
                configuration->getName() == "toolBar");


    if (configuration->hasAttribute("align"))
    {
        std::string aligment = configuration->getExistingAttributeValue("align");
        if (aligment == "top")
        {
            m_aligment = TOP;
        }
        else  if (aligment == "bottom")
        {
            m_aligment = BOTTOM;
        }
        else  if (aligment == "right")
        {
            m_aligment = RIGHT;
        }
        else  if (aligment == "left")
        {
            m_aligment = LEFT;
        }
        else
        {
            OSLM_FATAL("Wrong value '"<< aligment <<"' for 'align' attribute (require top, bottom, right or left)");
        }
    }

    ::fwRuntime::ConfigurationElementContainer::Iterator iter ;
    for( iter = configuration->begin() ; iter != configuration->end() ; ++iter )
    {
        if( (*iter)->getName() == "toolBitmapSize" )
        {
            if((*iter)->hasAttribute("height"))
            {
                m_toolBitmapSize.second = ::boost::lexical_cast< int > ((*iter)->getExistingAttributeValue("height"));
            }
            if((*iter)->hasAttribute("width"))
            {
                m_toolBitmapSize.first = ::boost::lexical_cast< int > ((*iter)->getExistingAttributeValue("width"));
            }
        }
    }
}

//-----------------------------------------------------------------------------

::fwGui::container::fwToolBar::sptr IToolBarBuilder::getToolBar()
{
    return this->m_toolBar;
}

//-----------------------------------------------------------------------------

} // namespace builder
} // namespace fwGui




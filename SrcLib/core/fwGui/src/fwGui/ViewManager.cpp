/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <utility>

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

#include "fwGui/ViewManager.hpp"

namespace fwGui
{

//-----------------------------------------------------------------------------

ViewManager::ViewManager(const std::string sid) : m_sid(sid)
{}

//-----------------------------------------------------------------------------

ViewManager::~ViewManager()
{}

//-----------------------------------------------------------------------------

::fwGui::fwContainer::sptr ViewManager::getParent()
{
    return this->m_parentContainer;
}

//-----------------------------------------------------------------------------

void ViewManager::initialize( ::fwRuntime::ConfigurationElement::sptr configuration)
{
    OSLM_ASSERT("Bad configuration name "<<configuration->getName()<< ", must be viewManager",
            configuration->getName() == "viewManager");

    std::vector < ConfigurationType > vectViews = configuration->find("scene");
    BOOST_FOREACH( ConfigurationType view, vectViews)
    {
        SLM_ASSERT("<view> tag must have sid or wid attribute",
                view->hasAttribute("sid") || view->hasAttribute("wid"));
        SLM_ASSERT("<view> tag must have index attribute",
                        view->hasAttribute("index"));

        int index = ::boost::lexical_cast< int >(view->getAttributeValue("index"));
        if(view->hasAttribute("sid"))
        {
            bool start = false;
            if(view->hasAttribute("start"))
            {
                std::string startValue = view->getAttributeValue("start");
                SLM_ASSERT("Wrong value '"<< startValue <<"' for 'start' attribute (require yes or no)",
                        startValue == "yes" || startValue == "no");
                start = (startValue=="yes");
            }
            std::string sid = view->getAttributeValue("sid");
            std::pair<int, bool> indexStart =  std::make_pair( index, start);
            m_sids[sid] = indexStart;
        }
        if(view->hasAttribute("wid"))
        {
            std::string wid = view->getAttributeValue("wid");
            m_wids[wid] = index;
        }
    }
}

//-----------------------------------------------------------------------------

void ViewManager::manage(std::vector< ::fwGui::fwContainer::sptr > subViews )
{
    SLM_FATAL("TODO: manage not yet available");
}

//-----------------------------------------------------------------------------

void ViewManager::unmanage()
{
    SLM_FATAL("TODO: unmanage not yet available");
}

//-----------------------------------------------------------------------------

} //namespace fwGui

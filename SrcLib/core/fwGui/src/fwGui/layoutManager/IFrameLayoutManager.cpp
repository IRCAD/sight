/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/filesystem/operations.hpp>
#include <boost/lexical_cast.hpp>

#include <fwCore/base.hpp>

#include "fwGui/layoutManager/IFrameLayoutManager.hpp"

namespace fwGui
{

namespace layoutManager
{
//-----------------------------------------------------------------------------

const IFrameLayoutManager::RegistryKeyType IFrameLayoutManager::REGISTRY_KEY =  "::fwGui::FrameLayoutManager";

//-----------------------------------------------------------------------------

IFrameLayoutManager::IFrameLayoutManager()
{}

//-----------------------------------------------------------------------------

IFrameLayoutManager::~IFrameLayoutManager()
{}

//-----------------------------------------------------------------------------

void IFrameLayoutManager::initialize( ConfigurationType configuration)
{
    OSLM_ASSERT("Bad configuration name "<<configuration->getName()<< ", must be frame",
            configuration->getName() == "frame");

    std::vector < ConfigurationType > name    = configuration->find("name");
    std::vector < ConfigurationType > icon    = configuration->find("icon");
    std::vector < ConfigurationType > minSize = configuration->find("minSize");
    std::vector < ConfigurationType > styles = configuration->find("styles");

    if(!name.empty())
    {
        m_frameInfo.m_name = name.at(0)->getValue();
    }

    if(!icon.empty())
    {
        m_frameInfo.m_iconPath = ::boost::filesystem::path( icon.at(0)->getValue() ) ;
        OSLM_ASSERT("Sorry, icon "<< m_frameInfo.m_iconPath << " doesn't exist", ::boost::filesystem::exists(m_frameInfo.m_iconPath));
    }

    if(!minSize.empty())
    {
        if(minSize.at(0)->hasAttribute("width"))
        {
            m_frameInfo.m_minSize.first = ::boost::lexical_cast<int >(minSize.at(0)->getExistingAttributeValue("width")) ;
        }
        if(minSize.at(0)->hasAttribute("height"))
        {
            m_frameInfo.m_minSize.second = ::boost::lexical_cast<int >(minSize.at(0)->getExistingAttributeValue("height")) ;
        }
    }

    if(!styles.empty())
    {
        SLM_FATAL("[TODO] Styles not yet available");
    }
}

//-----------------------------------------------------------------------------

} // namespace layoutManager
} // namespace fwGui




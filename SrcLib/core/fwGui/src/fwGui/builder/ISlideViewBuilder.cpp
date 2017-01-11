/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#include "fwGui/builder/ISlideViewBuilder.hpp"

#include <boost/lexical_cast.hpp>

namespace fwGui
{
namespace builder
{

const ISlideViewBuilder::RegistryKeyType ISlideViewBuilder::REGISTRY_KEY = "::fwGui::SlideViewBuilder";

//-----------------------------------------------------------------------------

ISlideViewBuilder::ISlideViewBuilder() :
    m_size(200),
    m_opacity(1.),
    m_aligment(LEFT)
{
}

//-----------------------------------------------------------------------------

ISlideViewBuilder::~ISlideViewBuilder()
{
}

//-----------------------------------------------------------------------------

void ISlideViewBuilder::initialize( ::fwRuntime::ConfigurationElement::sptr configuration)
{
    SLM_ASSERT("Bad configuration name " + configuration->getName() + ", must be 'slideView'",
               configuration->getName() == "slideView");


    if (configuration->hasAttribute("align"))
    {
        std::string aligment = configuration->getExistingAttributeValue("align");
        if (aligment == "top")
        {
            m_aligment = TOP;
        }
        else if (aligment == "bottom")
        {
            m_aligment = BOTTOM;
        }
        else if (aligment == "right")
        {
            m_aligment = RIGHT;
        }
        else if (aligment == "left")
        {
            m_aligment = LEFT;
        }
        else
        {
            SLM_FATAL("Wrong value '"+ aligment +"' for 'align' attribute (require top, bottom, right or left)");
        }
    }

    if (configuration->hasAttribute("opacity"))
    {
        std::string opacity = configuration->getExistingAttributeValue("opacity");
        m_opacity = std::stod(opacity);
        SLM_ASSERT("Opacity must be in [0 - 1]; actual: " + opacity, m_opacity >= 0. && m_opacity <= 1.);
    }

    if (configuration->hasAttribute("size"))
    {
        std::string size = configuration->getExistingAttributeValue("size");
        m_size = std::stoi(size);
        SLM_ASSERT("Size must not be negative", m_size >= 0.);
    }

    ::fwRuntime::ConfigurationElement::csptr styleCfg = configuration->findConfigurationElement("styleSheet");
    if (styleCfg)
    {
        m_styleSheet = styleCfg->getValue();
    }
}

//-----------------------------------------------------------------------------

::fwGui::container::fwContainer::sptr ISlideViewBuilder::getContainer() const
{
    return m_container;
}

//-----------------------------------------------------------------------------

} // namespace builder
} // namespace fwGui

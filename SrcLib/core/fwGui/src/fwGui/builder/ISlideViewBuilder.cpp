/************************************************************************
 *
 * Copyright (C) 2016 IRCAD France
 * Copyright (C) 2016 IHU Strasbourg
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

/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "scene2d/parser/Viewport.hpp"

#include <fwRenderQt/data/Viewport.hpp>

#include <fwServices/IXMLParser.hpp>
#include <fwServices/macros.hpp>

fwServicesRegisterMacro( ::fwServices::IXMLParser, ::scene2d::parser::Viewport, ::fwRenderQt::data::Viewport );

namespace scene2d
{
namespace parser
{

//------------------------------------------------------------------------------

void Viewport::createConfig( ::fwTools::Object::sptr _obj )
{
    ::fwRenderQt::data::Viewport::sptr viewport = ::fwRenderQt::data::Viewport::dynamicCast( _obj );
    SLM_ASSERT("Viewport not instanced", viewport);

    typedef ::fwRuntime::ConfigurationElement::sptr ConfigurationType;
    const ConfigurationType config = m_cfg->findConfigurationElement("config");

    if(config)
    {
        SLM_ASSERT("Attribute 'x' is missing", config->hasAttribute("x"));
        SLM_ASSERT("Attribute 'y' is missing", config->hasAttribute("y"));
        SLM_ASSERT("Attribute 'width' is missing", config->hasAttribute("width"));
        SLM_ASSERT("Attribute 'height' is missing", config->hasAttribute("height"));

        const std::string x      = config->getAttributeValue("x");
        const std::string y      = config->getAttributeValue("y");
        const std::string width  = config->getAttributeValue("width");
        const std::string height = config->getAttributeValue("height");
        viewport->setX(std::stof(x));
        viewport->setY(std::stof(y));
        viewport->setWidth(std::stof(width));
        viewport->setHeight(std::stof(height));
    }
}

//------------------------------------------------------------------------------

} //namespace parser
} //namespace scene2d


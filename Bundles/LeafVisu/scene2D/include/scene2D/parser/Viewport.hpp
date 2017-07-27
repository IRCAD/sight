/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __SCENE2D_PARSER_VIEWPORT_HPP__
#define __SCENE2D_PARSER_VIEWPORT_HPP__

#include "scene2d/config.hpp"

#include <fwRuntime/ConfigurationElement.hpp>

#include <fwServices/IXMLParser.hpp>

#include <fwTools/Failed.hpp>
#include <fwTools/Object.hpp>

namespace scene2d
{
namespace parser
{
/**
 * @brief   Specific service for the construction of a Viewport and its associated services from an XML-based
 * description.
 * @see     ::fwServices::IXMLParser
 */
class SCENE2D_CLASS_API Viewport : public ::fwServices::IXMLParser
{

public:

    fwCoreServiceClassDefinitionsMacro( (Viewport)(::fwServices::IXMLParser) );

    /// Constructor : does nothing.
    Viewport()
    {
    }

    /// Destructor : does nothing.
    virtual ~Viewport()
    {
    }

    SCENE2D_API void createConfig( ::fwTools::Object::sptr _obj );

};

} //namespace parser
} //namespace scene2d

#endif // __SCENE2D_PARSER_VIEWPORT_HPP__

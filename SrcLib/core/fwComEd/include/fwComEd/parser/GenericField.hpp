/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWCOMED_PARSER_GENERICFIELD_HPP_
#define _FWCOMED_PARSER_GENERICFIELD_HPP_

#include <fwTools/Failed.hpp>

#include <fwTools/Object.hpp>

#include <fwRuntime/ConfigurationElement.hpp>

#include <fwServices/IXMLParser.hpp>

#include "fwComEd/export.hpp"

namespace fwComEd
{
namespace parser
{
/**
 * @brief   Specific service for the construction of a GenericField and its associated services from an XML-based description.
 * @class   TransformationMatrix3D
 * @author  IRCAD (Research and Development Team).

 * @date    2007-2009
 * @see     ::fwServices::IXMLParser
 */
class FWCOMED_CLASS_API GenericField  : public ::fwServices::IXMLParser
{

public :

    fwCoreServiceClassDefinitionsMacro ( (GenericField)(::fwServices::IXMLParser) ) ;

    /// Constructor : does nothing.
    FWCOMED_API GenericField() {};

    /// Destructor : does nothing.
    FWCOMED_API virtual ~GenericField() {};

    FWCOMED_API void createConfig( ::fwTools::Object::sptr _obj );
protected:

    /**
     * @brief Updating method : create the process object.
     *
     * Parse the configuration element to configure inputs and outputs and add
     * them in the process object.
     */
    FWCOMED_API virtual void updating( ) throw(fwTools::Failed) ;

};

class BooleanParser : public GenericField{
public:
    fwCoreServiceClassDefinitionsMacro ( (BooleanParser)(::fwServices::IXMLParser) ) ;
};
class IntegerParser : public GenericField{
public:
    fwCoreServiceClassDefinitionsMacro ( (IntegerParser)(::fwServices::IXMLParser) ) ;
};
class FloatParser : public GenericField{
public:
        fwCoreServiceClassDefinitionsMacro ( (FloatParser)(::fwServices::IXMLParser) ) ;
};
class StringParser : public GenericField{
public:
    fwCoreServiceClassDefinitionsMacro ( (StringParser)(::fwServices::IXMLParser) ) ;
};

} //namespace parser
} //namespace fwComEd

#endif // _FWCOMED_PARSER_GENERICFIELD_HPP_

/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __DATAREG_PARSER_GENERICFIELD_HPP__
#define __DATAREG_PARSER_GENERICFIELD_HPP__

#include "dataReg/config.hpp"

#include <fwRuntime/ConfigurationElement.hpp>

#include <fwServices/IXMLParser.hpp>

#include <fwTools/Failed.hpp>
#include <fwTools/Object.hpp>

namespace dataReg
{
namespace parser
{
/**
 * @brief   Specific service for the construction of a GenericField and its associated services from an XML-based description.
 * @see     ::fwServices::IXMLParser
 */
class DATAREG_CLASS_API GenericField  : public ::fwServices::IXMLParser
{

public:

    fwCoreServiceClassDefinitionsMacro ( (GenericField)(::fwServices::IXMLParser) );

    /// Constructor : does nothing.
    GenericField()
    {
    }

    /// Destructor : does nothing.
    virtual ~GenericField()
    {
    }

    DATAREG_API void createConfig( ::fwTools::Object::sptr _obj );
protected:

    /**
     * @brief Updating method : create the process object.
     *
     * Parse the configuration element to configure inputs and outputs and add
     * them in the process object.
     */
    DATAREG_API virtual void updating( );

};

/**
 * @brief Specialisation of GenericField parser for boolean
 */
class BooleanParser : public GenericField
{
public:
    fwCoreServiceClassDefinitionsMacro ( (BooleanParser)(::fwServices::IXMLParser) );
};


/**
 * @brief Specialisation of GenericField parser for integer
 */
class IntegerParser : public GenericField
{
public:
    fwCoreServiceClassDefinitionsMacro ( (IntegerParser)(::fwServices::IXMLParser) );
};


/**
 * @brief Specialisation of GenericField parser for float
 */
class FloatParser : public GenericField
{
public:
    fwCoreServiceClassDefinitionsMacro ( (FloatParser)(::fwServices::IXMLParser) );
};


/**
 * @brief Specialisation of GenericField parser for string
 */
class StringParser : public GenericField
{
public:
    fwCoreServiceClassDefinitionsMacro ( (StringParser)(::fwServices::IXMLParser) );
};

} //namespace parser
} //namespace dataReg

#endif // __DATAREG_PARSER_GENERICFIELD_HPP__

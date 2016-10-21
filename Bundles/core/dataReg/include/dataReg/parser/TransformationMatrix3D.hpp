/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __DATAREG_PARSER_TRANSFORMATIONMATRIX3D_HPP__
#define __DATAREG_PARSER_TRANSFORMATIONMATRIX3D_HPP__

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
 * @brief   Specific service for the construction of a TransformationMatrix3D and its associated services from an XML-based description.
 * @see     ::fwServices::IXMLParser
 */
class DATAREG_CLASS_API TransformationMatrix3D : public ::fwServices::IXMLParser
{

public:

    fwCoreServiceClassDefinitionsMacro ( (TransformationMatrix3D)(::fwServices::IXMLParser) );

    /// Constructor : does nothing.
    TransformationMatrix3D()
    {
    }

    /// Destructor : does nothing.
    virtual ~TransformationMatrix3D()
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
    DATAREG_API virtual void updating( ) throw(fwTools::Failed);

};

} //namespace parser
} //namespace dataReg

#endif // __DATAREG_PARSER_TRANSFORMATIONMATRIX3D_HPP__

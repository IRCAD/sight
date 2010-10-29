/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWCOMED_TRANSFORMATIONMATRIX3DPARSER_HPP_
#define _FWCOMED_TRANSFORMATIONMATRIX3DPARSER_HPP_

#include <fwTools/Failed.hpp>

#include <fwTools/Object.hpp>

#include <fwRuntime/ConfigurationElement.hpp>

#include <fwServices/IXMLParser.hpp>

#include "fwComEd/export.hpp"

namespace fwComEd
{

/**
 * @brief   Specific service for the construction of a TransformationMatrix3DParser and its associated services from an XML-based description.
 * @class   TransformationMatrix3DParser
 * @author  IRCAD (Research and Development Team).

 * @date    2007-2009
 * @see     ::fwServices::IXMLParser
 */
class FWCOMED_CLASS_API TransformationMatrix3DParser : public ::fwServices::IXMLParser
{

public :

    fwCoreServiceClassDefinitionsMacro ( (TransformationMatrix3DParser)(::fwServices::IXMLParser) ) ;

    /// Constructor : does nothing.
    FWCOMED_API TransformationMatrix3DParser() {};

    /// Destructor : does nothing.
    FWCOMED_API virtual ~TransformationMatrix3DParser() {};

    /**
     * @brief Updating method : create the process object.
     *
     * Parse the configuration element to configure inputs and outputs and add
     * them in the process object.
     */
    FWCOMED_API virtual void updating( ) throw(fwTools::Failed) ;

};

}

#endif // _FWCOMED_TRANSFORMATIONMATRIX3DPARSER_HPP_

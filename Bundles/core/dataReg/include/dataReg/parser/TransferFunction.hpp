/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __DATAREG_PARSER_TRANSFERFUNCTION_HPP__
#define __DATAREG_PARSER_TRANSFERFUNCTION_HPP__

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
 * @brief   Specific service for the construction of a TransferFunction and its associated services from an XML-based
 * description.
 * @see     ::fwServices::IXMLParser
 */
class DATAREG_CLASS_API TransferFunction : public ::fwServices::IXMLParser
{

public:

    fwCoreServiceClassDefinitionsMacro( (TransferFunction)(::fwServices::IXMLParser) );

    /// Constructor : does nothing.
    TransferFunction()
    {
    }

    /// Destructor : does nothing.
    virtual ~TransferFunction()
    {
    }

    /**
     * @code{.xml}
       <object type="::fwData::TransferFunction">
           <colors isClamped="true">
               <step color="#ffffff" value="0" />
               <step color="#0000ff" value="1000" />
               <step color="#00ff00" value="2000" />
               <step color="#ffff00" value="3000" />
               <step color="#ff0000" value="4000" />
               <step color="#000000" value="5000" />
           </colors>
       </object>
       @endcode
     * - \b step : defines a step in the legend given with the arguments color in hex-code and the max value.
     * - \b isClamped(optional, default: yes) : defines interpolation mode on extremities, if yes then after extremity
     *       point, the returned TF color is TFColor(0,0,0,0), else it is the color value of the extremity.
     */
    DATAREG_API void createConfig( ::fwTools::Object::sptr _obj ) override;
protected:

    /**
     * @brief Updating method : create the process object.
     *
     * Parse the configuration element to configure inputs and outputs and add
     * them in the process object.
     */
    DATAREG_API virtual void updating( ) override;

};

} //namespace parser
} //namespace dataReg

#endif // __DATAREG_PARSER_TRANSFERFUNCTION_HPP__

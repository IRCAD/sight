/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "dataReg/parser/TransformationMatrix3D.hpp"

#include <fwData/TransformationMatrix3D.hpp>

#include <fwServices/macros.hpp>

fwServicesRegisterMacro( ::fwServices::IXMLParser, ::dataReg::parser::TransformationMatrix3D,
                         ::fwData::TransformationMatrix3D );

namespace dataReg
{
namespace parser
{

//------------------------------------------------------------------------------

void TransformationMatrix3D::updating( )
{
    SLM_FATAL("This method is deprecated, and thus shouldn't be used.");
}

//------------------------------------------------------------------------------

void TransformationMatrix3D::createConfig( ::fwTools::Object::sptr _obj )
{
    ::fwData::TransformationMatrix3D::sptr matrix = ::fwData::TransformationMatrix3D::dynamicCast( _obj );
    SLM_ASSERT("matrix not instanced", matrix);

    for( ::fwRuntime::ConfigurationElement::csptr elem :  m_cfg->getElements() )
    {
        if ( elem->getName() == "matrix" )
        {
            std::string input = elem->getValue();
            std::istringstream inputString( input );

            double coef;
            for ( unsigned int k = 0; k < 16; k++)
            {
                inputString >> coef;
                matrix->getRefCoefficients()[k] = coef;
            }
        }
    }
}

//------------------------------------------------------------------------------

} //namespace parser
} //namespace dataReg


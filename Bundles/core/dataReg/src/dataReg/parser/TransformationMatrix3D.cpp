/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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
                matrix->getCoefficients()[k] = coef;
            }
        }
    }
}

//------------------------------------------------------------------------------

} //namespace parser
} //namespace dataReg

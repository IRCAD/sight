/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwTools/ClassRegistrar.hpp>
#include <fwTools/Factory.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/Factory.hpp>

#include <fwData/TransformationMatrix3D.hpp>

#include "fwComEd/TransformationMatrix3DParser.hpp"

REGISTER_SERVICE( ::fwServices::IXMLParser, ::fwComEd::TransformationMatrix3DParser, ::fwData::TransformationMatrix3D );

namespace fwComEd
{

//------------------------------------------------------------------------------

void TransformationMatrix3DParser::updating( ) throw(fwTools::Failed)
{
    ::fwRuntime::ConfigurationElement::sptr _cfgElement = this->m_configuration;
    ::fwData::TransformationMatrix3D::sptr matrix = this->getObject< ::fwData::TransformationMatrix3D >() ;
    assert( matrix ) ;

    for(    ::fwRuntime::ConfigurationElement::Iterator configEltIter = _cfgElement->begin();
            !(configEltIter == _cfgElement->end());
            ++configEltIter )
    {
        if ( (*configEltIter)->getName() == "matrix" )
        {
            std::string input = (*configEltIter)->getValue();
            std::istringstream inputString( input );

            matrix->getRefCoefficients().clear();
            matrix->getRefCoefficients().reserve(16);

            double coef;
            for ( unsigned int k=0; k < 16; k++)
            {
                inputString >> coef;
                OSLM_WARN("Coef = " <<  coef );
                matrix->getRefCoefficients().push_back( coef );
            }
        }
    }
}

//------------------------------------------------------------------------------

}

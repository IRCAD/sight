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

#include "fwComEd/parser/TransformationMatrix3D.hpp"

REGISTER_SERVICE( ::fwServices::IXMLParser, ::fwComEd::parser::TransformationMatrix3D, ::fwData::TransformationMatrix3D );

namespace fwComEd
{
namespace parser
{

//------------------------------------------------------------------------------

void TransformationMatrix3D::updating( ) throw(fwTools::Failed)
{
    SLM_FATAL("Sorry, this method is depreciated.");
}

//------------------------------------------------------------------------------

void TransformationMatrix3D::createConfig( ::fwTools::Object::sptr _obj )
{
    ::fwData::TransformationMatrix3D::sptr matrix = ::fwData::TransformationMatrix3D::dynamicCast( _obj );
    SLM_ASSERT("matrix not instanced", matrix);

    BOOST_FOREACH( ::fwRuntime::ConfigurationElement::csptr elem, m_cfg->getElements() )
    {
        if ( elem->getName() == "matrix" )
        {
            std::string input = elem->getValue();
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

} //namespace parser
} //namespace fwComEd


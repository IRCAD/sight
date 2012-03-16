/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/filesystem/convenience.hpp>

#include <fwData/Vector.hpp>

#include "fwXML/Serializer.hpp"
#include "fwXML/XML/XMLParser.hpp"
#include "fwXML/XML/GenericXMLTranslator.hpp"
#include "fwXML/XML/XMLTranslatorHelper.hpp"
#include "fwXML/boostSerializer/Acquisition.hpp"

#include "fwXML/XML/AcquisitionXMLTranslator.hpp"

namespace fwXML
{

AcquisitionXMLTranslator::AcquisitionXMLTranslator()
{}

//------------------------------------------------------------------------------

AcquisitionXMLTranslator::~AcquisitionXMLTranslator()
{}

//------------------------------------------------------------------------------

xmlNodePtr AcquisitionXMLTranslator::getXMLFrom( ::fwData::Object::sptr obj )
{
    ::fwData::Acquisition::sptr acquisition = ::fwData::Acquisition::dynamicCast(obj);

    GenericXMLTranslator< ::fwData::Acquisition > acquisition2xmlbase;
    SLM_ASSERT("Acquisition not instanced", acquisition);

    xmlNodePtr masterNode = acquisition2xmlbase.getXMLFrom(obj);
    SLM_ASSERT("node not instanced", masterNode);

    XMLTranslatorHelper::addAttribute( masterNode, "image", acquisition->getImage() );

    XMLTranslatorHelper::addAttributeVector( masterNode, "reconstructions", acquisition->getReconstructions() );

    XMLTranslatorHelper::addAttribute( masterNode, "structAnat", acquisition->getStructAnat(), false );

    return masterNode;
}

//-----------------------------------------------------------------------------

void AcquisitionXMLTranslator::updateDataFromXML( ::fwData::Object::sptr toUpdate,  xmlNodePtr source)
{
    SLM_ASSERT("toUpdate not instanced", toUpdate);
    ::fwData::Acquisition::sptr acquisition = ::fwData::Acquisition::dynamicCast(toUpdate);

    GenericXMLTranslator< ::fwData::Acquisition > acquisition2xmlbase;
    acquisition2xmlbase.updateDataFromXML(toUpdate, source);

    acquisition->setImage( XMLTranslatorHelper::getAttribute< ::fwData::Image >( source, "image" ) );

    acquisition->setReconstructions( XMLTranslatorHelper::getAttributeVector< ::fwData::Reconstruction >( source, "reconstructions" ) );

    acquisition->setStructAnat( XMLTranslatorHelper::getAttribute< ::fwData::Image >( source, "structAnat", false ) );
}

//------------------------------------------------------------------------------

}

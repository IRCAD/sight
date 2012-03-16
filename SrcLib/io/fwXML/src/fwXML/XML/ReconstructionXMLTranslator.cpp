/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/filesystem/convenience.hpp>

#include <fwData/Material.hpp>

#include "fwXML/Serializer.hpp"
#include "fwXML/XML/XMLParser.hpp"
#include "fwXML/XML/GenericXMLTranslator.hpp"
#include "fwXML/XML/XMLTranslatorHelper.hpp"
#include "fwXML/IFileFormatService.hpp"

#include "fwXML/boostSerializer/Reconstruction.hpp"

#include "fwXML/XML/ReconstructionXMLTranslator.hpp"

namespace fwXML
{

ReconstructionXMLTranslator::ReconstructionXMLTranslator()
{}

//------------------------------------------------------------------------------

ReconstructionXMLTranslator::~ReconstructionXMLTranslator()
{}

//------------------------------------------------------------------------------

xmlNodePtr ReconstructionXMLTranslator::getXMLFrom( ::fwData::Object::sptr obj )
{
    ::fwData::Reconstruction::sptr reconst = ::fwData::Reconstruction::dynamicCast(obj);

    GenericXMLTranslator< ::fwData::Reconstruction > reconst2xmlbase;
    SLM_ASSERT("Reconstruction not instanced", reconst);

    xmlNodePtr masterNode = reconst2xmlbase.getXMLFrom(obj);
    SLM_ASSERT("node not instanced", masterNode);

    XMLTranslatorHelper::addAttribute( masterNode, "material", reconst->getMaterial() );
    XMLTranslatorHelper::addAttribute( masterNode, "image", reconst->getImage() );
    XMLTranslatorHelper::addAttribute( masterNode, "mesh", reconst->getMesh() );

    return masterNode;
}

//-----------------------------------------------------------------------------

void ReconstructionXMLTranslator::updateDataFromXML( ::fwData::Object::sptr toUpdate,  xmlNodePtr source)
{
    SLM_ASSERT("toUpdate not instanced", toUpdate);
    ::fwData::Reconstruction::sptr reconst = ::fwData::Reconstruction::dynamicCast(toUpdate);

    GenericXMLTranslator< ::fwData::Reconstruction > reconst2xmlbase;
    reconst2xmlbase.updateDataFromXML(toUpdate, source);

    reconst->setMaterial( XMLTranslatorHelper::getAttribute< ::fwData::Material >( source, "material" ) );
    reconst->setImage( XMLTranslatorHelper::getAttribute< ::fwData::Image >( source, "image" ) );
    reconst->setMesh( XMLTranslatorHelper::getAttribute< ::fwData::Mesh >( source, "mesh" ) );
}

//------------------------------------------------------------------------------

}

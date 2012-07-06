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
#include "fwXML/boostSerializer/ResectionDB.hpp"

#include "fwXML/XML/ResectionDBXMLTranslator.hpp"

namespace fwXML
{

ResectionDBXMLTranslator::ResectionDBXMLTranslator()
{}

//------------------------------------------------------------------------------

ResectionDBXMLTranslator::~ResectionDBXMLTranslator()
{}

//------------------------------------------------------------------------------

xmlNodePtr ResectionDBXMLTranslator::getXMLFrom( ::fwData::Object::sptr obj )
{
    ::fwData::ResectionDB::sptr resectionDB = ::fwData::ResectionDB::dynamicCast(obj);

    xmlNodePtr masterNode = XMLTranslatorHelper::MasterNode( obj );

    SLM_ASSERT("Node not instanced", masterNode);

    XMLTranslatorHelper::addAttributeVector( masterNode, "Resections", resectionDB->getResections() );
    XMLTranslatorHelper::addAttribute( masterNode, "SafeResection", resectionDB->getSafeResection(), false );

    return masterNode;
}

//-----------------------------------------------------------------------------

void ResectionDBXMLTranslator::updateDataFromXML( ::fwData::Object::sptr toUpdate,  xmlNodePtr source)
{
    SLM_ASSERT("toUpdate not instanced", toUpdate);
    ::fwData::ResectionDB::sptr resectionDB = ::fwData::ResectionDB::dynamicCast(toUpdate);

    resectionDB->setResections(XMLTranslatorHelper::getAttributeVector< ::fwData::Resection >( source, "Resections" ) );
    resectionDB->setSafeResection(XMLTranslatorHelper::getAttribute< ::fwData::Resection >( source, "SafeResection", false ) );
}

//------------------------------------------------------------------------------

}

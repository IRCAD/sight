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
#include "fwXML/boostSerializer/Dictionary.hpp"

#include "fwXML/XML/DictionaryXMLTranslator.hpp"

namespace fwXML
{

DictionaryXMLTranslator::DictionaryXMLTranslator()
{}

//------------------------------------------------------------------------------

DictionaryXMLTranslator::~DictionaryXMLTranslator()
{}

//------------------------------------------------------------------------------

xmlNodePtr DictionaryXMLTranslator::getXMLFrom( ::fwData::Object::sptr obj )
{
    ::fwData::Dictionary::sptr dictionary = ::fwData::Dictionary::dynamicCast(obj);

    xmlNodePtr masterNode = XMLTranslatorHelper::MasterNode( obj );
    SLM_ASSERT("Node not instanced", masterNode);

    XMLTranslatorHelper::addAttributeComposite( masterNode, "dictionary", dictionary->getDictionaryOrgans() );

    return masterNode;
}

//-----------------------------------------------------------------------------

void DictionaryXMLTranslator::updateDataFromXML( ::fwData::Object::sptr toUpdate,  xmlNodePtr source)
{
    SLM_ASSERT("toUpdate not instanced", toUpdate);
    ::fwData::Dictionary::sptr dictionary = ::fwData::Dictionary::dynamicCast(toUpdate);


    dictionary->setDictionaryOrgans(XMLTranslatorHelper::getAttributeComposite< ::fwData::DictionaryOrgan >( source, "dictionary" ) );
}

//------------------------------------------------------------------------------

}

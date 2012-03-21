/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/filesystem/convenience.hpp>


#include "fwXML/Serializer.hpp"
#include "fwXML/XML/XMLParser.hpp"
#include "fwXML/XML/GenericXMLTranslator.hpp"
#include "fwXML/XML/XMLTranslatorHelper.hpp"
#include "fwXML/boostSerializer/ProcessObject.hpp"

#include "fwXML/XML/ProcessObjectXMLTranslator.hpp"

namespace fwXML
{

ProcessObjectXMLTranslator::ProcessObjectXMLTranslator()
{}

//------------------------------------------------------------------------------

ProcessObjectXMLTranslator::~ProcessObjectXMLTranslator()
{}

//------------------------------------------------------------------------------

xmlNodePtr ProcessObjectXMLTranslator::getXMLFrom( ::fwData::Object::sptr obj )
{
    ::fwData::ProcessObject::sptr dictionary = ::fwData::ProcessObject::dynamicCast(obj);

    xmlNodePtr masterNode = XMLTranslatorHelper::MasterNode( obj );
    SLM_ASSERT("Node not instanced", masterNode);

    XMLTranslatorHelper::addAttributeComposite( masterNode, "inputs", dictionary->getInputs() );
    XMLTranslatorHelper::addAttributeComposite( masterNode, "outputs", dictionary->getOutputs() );

    return masterNode;
}

//-----------------------------------------------------------------------------

void ProcessObjectXMLTranslator::updateDataFromXML( ::fwData::Object::sptr toUpdate,  xmlNodePtr source)
{
    SLM_ASSERT("toUpdate not instanced", toUpdate);
    ::fwData::ProcessObject::sptr processObject = ::fwData::ProcessObject::dynamicCast(toUpdate);


    processObject->setInputs(XMLTranslatorHelper::getAttributeComposite< ::fwData::Object >( source, "inputs" ) );
    processObject->setOutputs(XMLTranslatorHelper::getAttributeComposite< ::fwData::Object >( source, "outputs" ) );
}

//------------------------------------------------------------------------------

}

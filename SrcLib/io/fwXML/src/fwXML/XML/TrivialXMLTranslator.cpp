/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwXML/XML/TrivialXMLTranslator.hpp"
#include "fwXML/XML/XMLTranslatorHelper.hpp"
#include "libxml/tree.h"
#include <fwData/Object.hpp>


namespace fwXML
{

//------------------------------------------------------------------------------

TrivialXMLTranslator::TrivialXMLTranslator()
{}

//------------------------------------------------------------------------------

TrivialXMLTranslator::~TrivialXMLTranslator()
{}

//------------------------------------------------------------------------------

xmlNodePtr TrivialXMLTranslator::getXMLFrom( ::fwData::Object::sptr obj )
{
    // create master node with className+id
    xmlNodePtr node = XMLTranslatorHelper::MasterNode( obj );

    xmlAddChild( node, xmlNewComment( BAD_CAST "Trivial XML Translator" ) );
    return node;
}

//------------------------------------------------------------------------------

void TrivialXMLTranslator::updateDataFromXML(::fwData::Object::sptr , xmlNode* )
{
    return;
}


}

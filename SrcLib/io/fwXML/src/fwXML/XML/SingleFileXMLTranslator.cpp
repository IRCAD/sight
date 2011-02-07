/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwXML/XML/SingleFileXMLTranslator.hpp"
#include <fwData/location/SingleFile.hpp>

#include "fwXML/XML/XMLParser.hpp"
#include "fwXML/XML/XMLTranslatorHelper.hpp"
#include "fwCore/base.hpp"

namespace fwXML
{

SingleFileXMLTranslator::SingleFileXMLTranslator() {};

SingleFileXMLTranslator::~SingleFileXMLTranslator() {};






xmlNodePtr SingleFileXMLTranslator::getXMLFrom( ::boost::shared_ptr<fwTools::Object> obj )
{

   ::fwData::location::SingleFile::sptr  path = ::fwData::location::SingleFile::dynamicCast(obj);
    assert( path );

    // create master node with className+id
    xmlNodePtr node = XMLTranslatorHelper::MasterNode( obj );

    // append value= Property
    std::string val= path->getPath().string() ;
    xmlNodeAddContent( node,  xmlStrdup( BAD_CAST val.c_str() ) );

    return node;

}


void SingleFileXMLTranslator::updateDataFromXML( ::fwTools::Object::sptr toUpdate,  xmlNodePtr source)
{
    assert( toUpdate ); // object should exist
    //get its label
    ::fwData::location::SingleFile *path=dynamic_cast< ::fwData::location::SingleFile *>(toUpdate.get());
    assert( path );
    std::string spath = XMLParser::getTextValue(source);
    path->setPath( ::boost::filesystem::path( spath) );
}


}

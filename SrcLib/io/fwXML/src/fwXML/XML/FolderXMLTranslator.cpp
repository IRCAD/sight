/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwXML/XML/FolderXMLTranslator.hpp"
#include "fwXML/XML/XMLParser.hpp"
#include "fwXML/XML/XMLTranslatorHelper.hpp"

namespace fwXML
{

//------------------------------------------------------------------------------

FolderXMLTranslator::FolderXMLTranslator()
{}

//------------------------------------------------------------------------------

FolderXMLTranslator::~FolderXMLTranslator()
{}

//------------------------------------------------------------------------------

xmlNodePtr FolderXMLTranslator::getXMLFrom( ::fwTools::Object::sptr obj )
{
    ::fwData::location::Folder::sptr  folder = ::fwData::location::Folder::dynamicCast(obj);
    SLM_ASSERT("path not instanced", folder);

    // create master node with className+id
    xmlNodePtr node = XMLTranslatorHelper::MasterNode( obj );

    // append value= Property
    std::string val= folder->getFolder().string() ;
    xmlNodeAddContent( node,  xmlStrdup( BAD_CAST val.c_str() ) );

    return node;
}

//------------------------------------------------------------------------------

void FolderXMLTranslator::updateDataFromXML( ::fwTools::Object::sptr toUpdate,  xmlNodePtr source)
{
    SLM_ASSERT("toUpdate not instanced", toUpdate); // object should exist
    //get its label
    ::fwData::location::Folder *folder = dynamic_cast< ::fwData::location::Folder *>(toUpdate.get());
    SLM_ASSERT("path not instanced", folder);
    std::string spath = XMLParser::getTextValue(source);
    folder->setFolder( ::boost::filesystem::path(spath) );
}

//------------------------------------------------------------------------------

}

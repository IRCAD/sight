/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>

#include <fwServices/helper.hpp>
#include <fwServices/registry/ServiceFactory.hpp>

#include <fwTools/ClassFactoryRegistry.hpp>
#include <fwTools/UUID.hpp>

#include <fwDataIO/writer/IObjectWriter.hpp>

#include <libxml/tree.h>

#include "fwXML/visitor/SerializeXML.hpp"
#include "fwXML/XML/XMLTranslatorHelper.hpp"
#include "fwXML/XML/TrivialXMLTranslator.hpp"
#include "fwXML/IFileFormatService.hpp"

namespace visitor
{

SerializeXML::SerializeXML()
{}

//-----------------------------------------------------------------------------

SerializeXML::~SerializeXML()
{}

//-----------------------------------------------------------------------------

void SerializeXML::visit( ::fwTools::Object::sptr obj)
{
    SLM_ASSERT("Object is null", obj);
    std::string uuid = ::fwTools::UUID::get(obj);
    std::string srcUuid = m_source?::fwTools::UUID::get(m_source):"NoSOURCENOUUID";
    bool supportFileFormatSrv =  ::fwServices::registry::ServiceFactory::getDefault()->support(obj->getClassname(),  "::fwXML::IFileFormatService");
    OSLM_DEBUG( "SerializeXML Visitor Visiting : Class " << obj->className() <<
                "(" <<  uuid    <<
                ") Support<FileFormatService>" <<  (supportFileFormatSrv?"yes":"no") <<
                "ParentClass: " <<  (m_source?m_source->className():"NULL")   << "(" << srcUuid << ")"
                );

    // get XMLTranslator
     ::boost::shared_ptr< fwXML::XMLTranslator > translator;;
    translator = fwTools::ClassFactoryRegistry::create< fwXML::XMLTranslator  >(  obj->className()  );
    if (translator.get()==0)
    {
        translator = ::boost::shared_ptr< ::fwXML::XMLTranslator>(new  ::fwXML::TrivialXMLTranslator() );
    }

    if ( supportFileFormatSrv )
    {
         ::boost::shared_ptr< ::fwXML::IFileFormatService >  saver =fwServices::get< ::fwXML::IFileFormatService >(obj,0);
        if (saver)
        {
            saver->filename() = obj->getLeafClassname() + "_" + ::fwTools::UUID::get(obj);
            saver->extension() = saver->getWriter()->extension();
        }
        else
        {
            saver->filename() = obj->getLeafClassname() + "_" + ::fwTools::UUID::get(obj);
            saver->extension() = ".dummy";
        }
    }

    // update XML
    xmlNodePtr objectXMLNode = translator->getXMLFrom(obj);
    if ( m_correspondance[m_source] ) //manage the root
    {
        xmlAddChild(m_correspondance[m_source] , objectXMLNode );
    }
    // keep correspondance
    m_correspondance[obj] = objectXMLNode;
}

//-----------------------------------------------------------------------------

}


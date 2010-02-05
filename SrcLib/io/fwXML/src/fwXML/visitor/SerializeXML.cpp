#include <fwCore/base.hpp>

#include <fwServices/helper.hpp>

#include <fwTools/ClassFactoryRegistry.hpp>
#include <fwTools/UUID.hpp>
#include "fwXML/visitor/SerializeXML.hpp"
#include "fwXML/XML/XMLTranslatorHelper.hpp"

#include "fwXML/XML/TrivialXMLTranslator.hpp"
#include "fwXML/IFileFormatService.hpp"
#include <fwDataIO/writer/IObjectWriter.hpp>

#include <libxml/tree.h>

namespace visitor
{

SerializeXML::SerializeXML()
{
}

SerializeXML::~SerializeXML()
{
}



void SerializeXML::visit( ::fwTools::Object::sptr obj)
{
    assert(obj);
    std::string uuid = ::fwTools::UUID::get(obj,::fwTools::UUID::EXTENDED);
    std::string srcUuid = m_source?::fwTools::UUID::get(m_source,::fwTools::UUID::EXTENDED):"NoSOURCENOUUID";
    OSLM_DEBUG( "SerializeXML Visitor Visiting : Class " << obj->className() <<
                "(" <<  uuid    <<
                ") Support<FileFormatService>" <<  (fwServices::support< ::fwXML::IFileFormatService >(obj)?"yes":"no") <<
                "ParentClass: " <<  (m_source?m_source->className():"NULL")   << "(" << srcUuid << ")"
                );

    // get XMLTranslator
     ::boost::shared_ptr< fwXML::XMLTranslator > translator;;
    translator = fwTools::ClassFactoryRegistry::create< fwXML::XMLTranslator  >(  obj->className()  );
    //assert(translator);
    if (translator.get()==0)
    {
        translator = ::boost::shared_ptr< ::fwXML::XMLTranslator>(new  ::fwXML::TrivialXMLTranslator() );
    }



    if ( fwServices::support< ::fwXML::IFileFormatService >(obj) )
    {
         ::boost::shared_ptr< ::fwXML::IFileFormatService >  saver =fwServices::get< ::fwXML::IFileFormatService >(obj,0);
        if (saver)
        {
            saver->filename() = obj->getLeafClassname() + "_" + ::fwTools::UUID::get(obj,::fwTools::UUID::EXTENDED);
            saver->extension() = saver->getWriter()->extension();
        }
        else
        {
            saver->filename() = obj->getLeafClassname() + "_" + ::fwTools::UUID::get(obj,::fwTools::UUID::EXTENDED);
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



}


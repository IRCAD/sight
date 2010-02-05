#include <fwTools/UUID.hpp>
#include <fwData/Object.hpp>
#include <fwData/visitor/accept.hpp>
#include "fwXML/XML/XMLTranslatorHelper.hpp"
#include "fwXML/XML/TrivialXMLTranslator.hpp"
#include "fwXML/visitor/SerializeXML.hpp"

namespace fwXML
{

XMLTranslatorHelper::XMLTranslatorHelper()
{
    // TODO Auto-generated constructor stub

}

XMLTranslatorHelper::~XMLTranslatorHelper()
{
    // TODO Auto-generated destructor stub
}


xmlNodePtr XMLTranslatorHelper::MasterNode( ::fwTools::Object::sptr obj )
{
    // create node with className
    xmlNodePtr node = xmlNewNode(NULL, xmlStrdup( BAD_CAST obj->getLeafClassname().c_str() ) );

    // append an unique id to its objects

    // OLD STYLE std::string id = boost::lexical_cast<std::string>(obj);
    std::string id = ::fwTools::UUID::get(obj , ::fwTools::UUID::EXTENDED );

    xmlNewProp( node, BAD_CAST "id", xmlStrdup( BAD_CAST  id.c_str() ));
    xmlNewProp( node, BAD_CAST "class", xmlStrdup( BAD_CAST  obj->getRootedClassname().c_str() ));

    return node;
}




xmlNodePtr XMLTranslatorHelper::toXML( ::fwTools::Object::sptr obj )
{
    ::boost::shared_ptr< ::fwXML::XMLTranslator > translator;
    translator = fwTools::ClassFactoryRegistry::create< ::fwXML::XMLTranslator  >(  obj->getRootedClassname() );

    if (translator)
    {
        return translator->getXMLFrom(obj);
    }
    else
    {
        return TrivialXMLTranslator().getXMLFrom(obj);
    }
}



xmlNodePtr XMLTranslatorHelper::toXMLRecursive( ::fwTools::Object::sptr obj )
{
    ::visitor::SerializeXML visitor;
    ::fwData::visitor::accept( obj , &visitor );
    return visitor.m_correspondance[ obj ];
}



void XMLTranslatorHelper::fromXML( ::fwTools::Object::sptr toUpdate, xmlNodePtr source )
{
    const std::string nameInXML = (const char*)source->name;
    assert( toUpdate->getLeafClassname() ==  nameInXML );

    ::boost::shared_ptr< ::fwXML::XMLTranslator > translator;
    translator = fwTools::ClassFactoryRegistry::create< ::fwXML::XMLTranslator  >(  typeid(*toUpdate)  );

    if (translator.get() )
    {
        translator->updateDataFromXML(toUpdate,source);
    }
    else
    {
        OSLM_WARN("No XML Translator for " << toUpdate->getLeafClassname() << " Object UnModified");
    }
}



::fwTools::Object::sptr XMLTranslatorHelper::fromXML( xmlNodePtr source )
{
    OSLM_TRACE(" fromXML(xmlNode) with xmlNode->name=" << (const char*)source->name << " addr=" << source );

    std::string className = ObjectTracker::getClassname(source);
    std::string id        = ObjectTracker::getID(source);

    // !!! NOTE HERE WE DO NOT PERFORME TRANSLATION ID OLD -> NEW !!!

    // dot not create duplicate object
    ::boost::shared_ptr< ::fwTools::Object > obj;
    if ( ObjectTracker::isAlreadyInstanciated( id ) )
    {
        obj = ObjectTracker::buildObject( className, id ); // use previous one
    }
    else
    {
        obj = ObjectTracker::buildObject( className, id ); // create one with a new id
        fromXML(obj,source); //then fill with xml
    }
    return obj;
}


xmlNodePtr XMLTranslatorHelper::newElement( const  std::string &name, bool value )
{
    xmlNodePtr result = xmlNewNode( NULL, xmlStrdup( BAD_CAST name.c_str() )  );
    xmlNodeAddContent( result,   BAD_CAST (value?"1":"0")  );
    return result;
}

} // end namespace

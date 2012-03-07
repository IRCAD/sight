/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwTools/UUID.hpp>
#include <fwTools/Factory.hpp>
#include <fwCore/base.hpp>


#include "fwXML/XML/XMLParser.hpp"
#include "fwXML/ObjectTracker.hpp"

namespace fwXML
{

ObjectTracker::Registry ObjectTracker::m_buildedObject;
ObjectTracker::OldNewUUIDMap ObjectTracker::m_oldNewUUIDTranslation;

//------------------------------------------------------------------------------

ObjectTracker::ObjectTracker()
{}

//------------------------------------------------------------------------------

ObjectTracker::~ObjectTracker()
{}

//------------------------------------------------------------------------------

void ObjectTracker::clear()
{
    m_buildedObject.clear();
    m_oldNewUUIDTranslation.clear();
}

//------------------------------------------------------------------------------

bool ObjectTracker::isAlreadyInstanciated( const std::string &uniqueID  )
{
    bool result = m_buildedObject.find( uniqueID ) != m_buildedObject.end();
    OSLM_DEBUG("ObjectTracker::isAlreadyInstanciated(" << uniqueID << ") return bool = "<< result);
    return  result;
}

//------------------------------------------------------------------------------

std::string  ObjectTracker::xmlID2RuntimeID( const std::string &xmlID )
{
   assert(  m_oldNewUUIDTranslation.find(xmlID)  != m_oldNewUUIDTranslation.end() );

   return m_oldNewUUIDTranslation[xmlID];
}

//------------------------------------------------------------------------------

std::string ObjectTracker::getID( xmlNodePtr xmlNode )
{
    std::string id;
    try
    {
        id =  XMLParser::getAttribute (xmlNode, "id");
    }
    catch (...)
    {
    OSLM_WARN("No tracking id for " << xmlNode->name );
    }

    return id;
}

//------------------------------------------------------------------------------

std::string ObjectTracker::getClassname( xmlNodePtr xmlNode )
{
    std::string className;
    try
    {
        className =  XMLParser::getAttribute (xmlNode, "class");
    }
    catch (...)
    {
    OSLM_WARN("No className for " << xmlNode->name );
    }

    return className;
}

//------------------------------------------------------------------------------

::fwTools::Object::sptr ObjectTracker::buildObject( const std::string &className, const std::string &uniqueIDXML  )
{
    if ( uniqueIDXML.empty() )
    {
        OSLM_DEBUG( className << " generated without id");
        return ::boost::dynamic_pointer_cast< ::fwTools::Object >( ::fwTools::Factory::buildData( className ) ) ;
    }

    Registry::iterator i =  m_buildedObject.find( uniqueIDXML );

    if ( i == m_buildedObject.end() )
    {
        // not already registred : create it then register it
        ::fwTools::Object::sptr newObject = ::fwTools::Object::dynamicCast( ::fwTools::Factory::buildData( className ) ) ;
        m_buildedObject[uniqueIDXML] = newObject;
        OSLM_DEBUG(className<<"-"<<newObject.get() << " first instantiation");

        std::string uuid = ::fwTools::UUID::get(newObject); // generate a new one if not supervised
        m_oldNewUUIDTranslation[uniqueIDXML] = uuid;
        OSLM_DEBUG(className<<"-"<<newObject.get() << " UUID : " << uuid);

        return newObject;
    }
    else
    {
        OSLM_DEBUG( className <<"-"<< uniqueIDXML << " previoulsy instantiated : use this one" );
        return i->second; // object exist we return it
    }
}

//------------------------------------------------------------------------------

}

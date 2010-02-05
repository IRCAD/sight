/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */



#ifndef OBJECTTRACKER_H_
#define OBJECTTRACKER_H_

#include <string>
#include <map>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

#include <libxml/tree.h>

#include <fwData/Object.hpp>

#include "fwXML/config.hpp"

namespace fwXML
{

/**
 * @brief The purpose of this class is to act as a Factory of object for the serialization framework. Its goal is
 * to manage object sharing by performing a unique instanciation
 * @author IRCAD (Research and Development Team).
 */
class FWXML_CLASS_API ObjectTracker
{
public:

    FWXML_API ObjectTracker();

    FWXML_API virtual ~ObjectTracker();

    /**
     * @brief create a new Object if it was not already produced else return it. When a new Object is created it is registred
     * via its ID to ensure a unique instance
     */

    /**
     * @brief instanciate a new object expect if it was already constructed
     * If the object (className-ID) is already constructed return the corresponding object else create
     * via Factory the newObject and register its ID. If the ID is empty no registration are performed
     * @param className the name of object to build
     * @param id the unique ID for an object : if no one specified the object is created via ObjectFactory
     */
    FWXML_API static ::boost::shared_ptr< fwTools::Object > buildObject( const std::string &className, const std::string &uniqueID  );

    /// return true if its already instanciated
    FWXML_API static bool isAlreadyInstanciated( const std::string &uniqueID  );

    static std::string getID( xmlNodePtr xmlNode );

    static std::string getClassname( xmlNodePtr xmlNode );

    static std::string translateID( const std::string & xmlid, bool assignNewUUID);

    /**
     * @brief reset the registry and transatation oldID<->newID (if any)
     */
    FWXML_API static void clear();


protected:



    /**
     * @brief typedef of mapping bettwen object id <-> object created
     */
    typedef std::map< std::string , ::boost::weak_ptr<fwTools::Object> > Registry;

    /**
     * @brief mapping bettwen object id <-> object created
     */
    FWXML_API static Registry m_buildedObject;


    /**
      * @brief typedef of mapping between object id (From XML) to a new one (already the same)
      */
    typedef std::map< std::string , std::string > OldNewUUIDMap;
    // to generate new uuid from uuid given in XML (if any)
    FWXML_API static OldNewUUIDMap m_oldNewUUIDTranslation;

};

}

#endif /*OBJECTTRACKER_H_*/

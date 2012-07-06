/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <algorithm>
#include <sstream>

#include <boost/lexical_cast.hpp>

#include <fwCore/base.hpp>

#include <fwData/DictionaryOrgan.hpp>
#include <fwData/Reconstruction.hpp>
#include <fwData/Object.hpp>

#include "fwXML/policy/PatientFolderPathPolicy.hpp"
#include "fwXML/XML/XMLHierarchy.hpp"
#include "fwXML/policy/DefaultPathPolicy.hpp"
#include "fwXML/policy/IndexPathPolicy.hpp"

namespace fwXML
{

//------------------------------------------------------------------------------

// from an object find transitivaly the first parent which is not a Field
::fwData::Object::sptr dataParent( ::fwData::Object::sptr obj )
{
    ::fwData::Object::sptr parent = ::fwXML::XMLHierarchy::getDefault()->mapChildFather()[obj].lock();
    assert (parent);

    OSLM_DEBUG( "dataParentof ( " << obj->className() << " addr=" << obj.get() << " ) return " <<  parent->className() << " addr=" << parent.get()  );

    return parent;
}

//------------------------------------------------------------------------------

::boost::filesystem::path PatientFolderPathPolicy::getPath(::fwData::Object::sptr obj )
{
    static IndexPathPolicy fpolicy;

    std::string className = obj->getLeafClassname();

    if ( className == "Patient")
    {
        return "patient/patient.xml";
    }
    if  ( className == "Dictionary" )
    {
        return "dictionary/dictionary.xml";
    }

    if ( className == "DictionaryOrgan" )
    {
        return "dictionary/entry_" + ::boost::dynamic_pointer_cast< ::fwData::DictionaryOrgan >(obj)->getStructureType() + ".xml";
    }

    if ( className == "Study" ) // "s#/study.xml"
    {
        SLM_FATAL("Study, problem with field removing, ACH ToDo");
//        std::stringstream ss;
//        ss << "s" << getFieldIndex(obj) << "/study.xml";
//        return ss.str();
        return "study.xml";
    }

    if ( className == "Acquisition" ) // "s#/a#/acquisition.xml"
    {
        SLM_FATAL("Acquisition : problem with field removing, ACH ToDo");
//        std::string acquisitionFolder("a");
//        acquisitionFolder+=boost::lexical_cast<std::string>( getFieldIndex(obj) );
//        ::boost::filesystem::path parentFolder = this->getPath(  dataParent(obj)  ).parent_path();
//        return  parentFolder / acquisitionFolder / "/acquistion.xml";
        return "acquistion.xml";
    }

    if ( className == "Reconstruction" ) // "s#/a#/NOMORGANE/reconstruction.xml"
    {
        ::boost::filesystem::path parentFolder = this->getPath(  dataParent(obj)  ).parent_path();
        std::string organName = ::boost::dynamic_pointer_cast< ::fwData::Reconstruction >(obj)->getOrganName();

        return parentFolder / organName / "reconstruction.xml";
    }

    return fpolicy.getPath(obj);
}


}//namespace fwXML

#include <algorithm>
#include <iostream> //REMOVEME
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

// from an object ( in a FIELD it is *mandatory* ) find its index/rank in parent Field which contain it
int getFieldIndex( ::boost::shared_ptr <fwTools::Object> obj )
{
    ::boost::shared_ptr <fwTools::Object> parent = fwXML::XMLHierarchy::getDefault()->mapChildFather()[obj].lock();
    assert ( boost::dynamic_pointer_cast< fwTools::Field >(parent) );

    fwTools::Object::ChildContainer::iterator iter =
    std::find( parent->children().begin(),   parent->children().end(), obj );

    // must exist
    assert( iter !=  parent->children().end() );
    int index = std::distance( parent->children().begin(), iter );

    OSLM_DEBUG( "getFieldIndex" << obj->className() << " addr=" << obj.get() << " indexFound=" << index );

    return index;

}

// from an object find transitivaly the first parent which is not a Field
::boost::shared_ptr <fwTools::Object> dataParent( ::boost::shared_ptr <fwTools::Object> obj )
{
    ::boost::shared_ptr <fwTools::Object> parent = fwXML::XMLHierarchy::getDefault()->mapChildFather()[obj].lock();

    while ( parent &&  boost::dynamic_pointer_cast< fwTools::Field >(parent) )
    {
        parent = dataParent( parent );
    }

    assert (parent);
    OSLM_DEBUG( "dataParentof ( " << obj->className() << " addr=" << obj.get() << " ) return " <<  parent->className() << " addr=" << parent.get()  );

    return parent;


}




boost::filesystem::path PatientFolderPathPolicy::getPath(::boost::shared_ptr <fwTools::Object> obj )
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
        return "dictionary/entry_" + boost::dynamic_pointer_cast< ::fwData::DictionaryOrgan >(obj)->getStructureType() + ".xml";
    }

    if ( className == "Study" ) // "s#/study.xml"
    {
        std::stringstream ss;
        ss << "s" << getFieldIndex(obj) << "/study.xml";
        return ss.str();
    }

    if ( className == "Acquisition" ) // "s#/a#/acquisition.xml"
    {
        std::string acquisitionFolder("a");
        acquisitionFolder+=boost::lexical_cast<std::string>( getFieldIndex(obj) );
        boost::filesystem::path parentFolder = this->getPath(  dataParent(obj)  ).branch_path();

        return  parentFolder / acquisitionFolder / "/acquistion.xml";
    }

    if ( className == "Reconstruction" ) // "s#/a#/NOMORGANE/reconstruction.xml"
    {
        boost::filesystem::path parentFolder = this->getPath(  dataParent(obj)  ).branch_path();
        std::string organName = boost::dynamic_pointer_cast< ::fwData::Reconstruction >(obj)->getOrganName();

        return parentFolder / organName / "reconstruction.xml";
    }





    //return DefaultPathPolicy().getPath(obj);
    return fpolicy.getPath(obj);
}


}//namespace fwXML

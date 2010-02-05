#include <fwCore/base.hpp>
#include <fwTools/UUID.hpp>

#include "fwXML/visitor/Serialize.hpp"
#include "fwXML/XML/XMLPartitioner.hpp"


//#include <libxml/tree.h>

namespace visitor
{

Serialize::Serialize()
{
}

Serialize::~Serialize()
{
}



void Serialize::visit( ::boost::shared_ptr< ::fwTools::Object> obj)
{
    assert(obj);
    std::string uuid = ::fwTools::UUID::get(obj,::fwTools::UUID::EXTENDED);
    std::string srcUuid = m_source?::fwTools::UUID::get(m_source,::fwTools::UUID::EXTENDED):"NoSOURCENOUUID";
    OSLM_DEBUG( "Serialize Visitor Visiting : Class " << obj->className() << " (" <<  uuid    <<
                ") ParentClass: " <<  (m_source?m_source->className():"NULL")   << "(" << srcUuid << ")"
                );
    ::fwXML::XMLPartitioner::getDefault()->manage( m_source, obj );
}



}


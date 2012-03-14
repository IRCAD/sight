/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>
#include <fwTools/UUID.hpp>

#include "fwXML/visitor/Serialize.hpp"
#include "fwXML/XML/XMLPartitioner.hpp"

namespace visitor
{

Serialize::Serialize()
{}

Serialize::~Serialize()
{}

void Serialize::visit( ::fwData::Object::sptr obj)
{
    SLM_ASSERT("Object not instanced", obj);
    OSLM_TRACE( "Serialize Visitor Visiting : Class " << obj->className() <<
                " (" <<  ::fwTools::UUID::get(obj)    <<
                ") ParentClass: " <<  (m_source?m_source->className():"NULL")   <<
                "(" << (m_source?::fwTools::UUID::get(m_source):"NoSOURCENOUUID") << ")"
                );
    ::fwXML::XMLPartitioner::getDefault()->manage( m_source, obj );
}


}


/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwData/TriangularMesh.hpp>
#include <fwCore/base.hpp>

#include <fwServices/Base.hpp>
#include <fwServices/macros.hpp>
#include <fwComEd/TriangularMeshMsg.hpp>

#include <fwMemory/IDumpService.hpp>

#include "memory/TriangularMeshMemoryService.hpp"


namespace memory
{

REGISTER_SERVICE( ::fwMemory::IMemoryService, ::memory::TriangularMeshMemoryService , ::fwData::TriangularMesh );

//------------------------------------------------------------------------------

TriangularMeshMemoryService::TriangularMeshMemoryService()
{
    addNewHandledEvent( ::fwComEd::TriangularMeshMsg::NEW_MESH );
}

//------------------------------------------------------------------------------

TriangularMeshMemoryService::~TriangularMeshMemoryService()
{
}

//------------------------------------------------------------------------------

unsigned long TriangularMeshMemoryService::getVirtualSize()
{
    SLM_TRACE_FUNC();
    ::fwData::TriangularMesh::sptr mesh = this->getObject< ::fwData::TriangularMesh>();
    SLM_ASSERT("Mesh not instanced", mesh);

    unsigned long size = 0;

    OSLM_WARN_IF("Points : size (" << mesh->points().size() << ") != capacity (" << mesh->points().capacity() << ")", mesh->points().size() != mesh->points().capacity());
    BOOST_FOREACH(std::vector<float> point, mesh->points())
    {
        OSLM_ASSERT("Point size != 3 " << point.size(), point.size() == 3);
        OSLM_WARN_IF("Point capacity (" << point.capacity() << ") != point size (" << point.size() << ")", point.size() != point.capacity());
        size += point.capacity() * sizeof(float) + sizeof(std::vector<float>);
    }

    OSLM_WARN_IF("size = " << mesh->cells().size() << "capacity = " << mesh->cells().capacity(), mesh->cells().size() != mesh->cells().capacity());
    BOOST_FOREACH(std::vector<int> cell, mesh->cells())
    {
        OSLM_ASSERT("Cell size != 3 " << cell.size(), cell.size() == 3);
        OSLM_WARN_IF("Cell capacity (" << cell.capacity() << ") != cell size (" << cell.size() << ")", cell.size() != cell.capacity());
        size += cell.capacity() * sizeof(int) + sizeof(std::vector<int>);
    }

    return size;
}

//------------------------------------------------------------------------------

void TriangularMeshMemoryService::updating( ::boost::shared_ptr< const ::fwServices::ObjectMsg > _msg ) throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    ::fwComEd::TriangularMeshMsg::csptr meshMessage = ::fwComEd::TriangularMeshMsg::dynamicConstCast( _msg );
    if( meshMessage )
    {
        if ( meshMessage->hasEvent( ::fwComEd::TriangularMeshMsg::NEW_MESH ) )
        {
            this->update();
        }
    }
}


}

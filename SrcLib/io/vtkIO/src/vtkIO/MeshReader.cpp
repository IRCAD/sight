/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#include <vtkGenericDataObjectReader.h>
#include <vtkPolyData.h>

#include <fwTools/ClassRegistrar.hpp>

#include <fwCore/base.hpp>

#include "vtkIO/vtk.hpp"
#include "vtkIO/MeshReader.hpp"

REGISTER_BINDING_BYCLASSNAME( ::fwDataIO::reader::IObjectReader , ::vtkIO::MeshReader, ::vtkIO::MeshReader );


namespace vtkIO
{
//------------------------------------------------------------------------------

MeshReader::MeshReader()
: ::fwData::location::enableSingleFile< ::fwDataIO::reader::IObjectReader >(this)
{
	SLM_TRACE("vtkIO::MeshReader::MeshReader");
}

//------------------------------------------------------------------------------

MeshReader::~MeshReader()
{
	SLM_TRACE("vtkIO::MeshReader::~MeshReader");
}

//------------------------------------------------------------------------------

void MeshReader::read()
{
	assert( m_object.use_count() );
	assert( !m_object.expired() );
	assert( m_object.lock() );

	::fwData::TriangularMesh::sptr pTriangularMesh = getConcreteObject();

	vtkGenericDataObjectReader *reader = vtkGenericDataObjectReader::New();
	reader->SetFileName(this->getFile().string().c_str());
	reader->Update();

	vtkDataObject *obj = reader->GetOutput();
	vtkPolyData* mesh = vtkPolyData::SafeDownCast(obj);
	if(mesh)
	{
		::vtkIO::fromVTKMesh(mesh, pTriangularMesh);
	}
	else
	{
		std::string errMsg;
		errMsg	= "MeshReader cannot read VTK Mesh file : ";
		errMsg.append( this->getFile().string() );
		throw( errMsg );
	}
	reader->Delete();
}

//------------------------------------------------------------------------------

} // namespace vtkIO

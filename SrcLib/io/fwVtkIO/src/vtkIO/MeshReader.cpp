/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <vtkGenericDataObjectReader.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>

#include <fwTools/ClassRegistrar.hpp>

#include <fwCore/base.hpp>

#include "vtkIO/helper/Mesh.hpp"
#include "vtkIO/MeshReader.hpp"
#include "vtkIO/helper/ProgressVtkToFw.hpp"

REGISTER_BINDING_BYCLASSNAME( ::fwDataIO::reader::IObjectReader , ::vtkIO::MeshReader, ::vtkIO::MeshReader );

namespace vtkIO
{
//------------------------------------------------------------------------------

MeshReader::MeshReader() :
                ::fwData::location::enableSingleFile< ::fwDataIO::reader::IObjectReader >(this)
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

MeshReader::~MeshReader()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void MeshReader::read()
{
    assert( !m_object.expired() );
    assert( m_object.lock() );

    ::fwData::Mesh::sptr pMesh = getConcreteObject();

    vtkSmartPointer< vtkGenericDataObjectReader > reader = vtkSmartPointer< vtkGenericDataObjectReader >::New();
    reader->SetFileName(this->getFile().string().c_str());

    //add progress observation
    Progressor progress(reader, this->getSptr(), this->getFile().string());

    reader->Update();

    vtkDataObject *obj = reader->GetOutput();
    vtkPolyData* mesh = vtkPolyData::SafeDownCast(obj);
    FW_RAISE_IF("MeshReader cannot read VTK Mesh file : "<< this->getFile().string(), !mesh);
    ::vtkIO::helper::Mesh::fromVTKMesh(mesh, pMesh);
}

//------------------------------------------------------------------------------

std::string  MeshReader::extension()
{
   return ".vtk";
}

} // namespace vtkIO

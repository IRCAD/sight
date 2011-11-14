/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <vtkPolyData.h>
#include <vtkGenericDataObjectWriter.h>
#include <vtkSmartPointer.h>

#include <fwTools/ClassRegistrar.hpp>

#include <fwCore/base.hpp>

#include "vtkIO/helper/Mesh.hpp"
#include "vtkIO/MeshWriter.hpp"
#include "vtkIO/helper/ProgressVtkToFw.hpp"

REGISTER_BINDING_BYCLASSNAME( ::fwDataIO::writer::IObjectWriter , ::vtkIO::MeshWriter, ::vtkIO::MeshWriter );


namespace vtkIO
{
//------------------------------------------------------------------------------

MeshWriter::MeshWriter()
: ::fwData::location::enableSingleFile< ::fwDataIO::writer::IObjectWriter >(this)
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

MeshWriter::~MeshWriter()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void MeshWriter::write()
{
    assert( !m_object.expired() );
    assert( m_object.lock() );

    ::fwData::Mesh::sptr pMesh = getConcreteObject();

    vtkSmartPointer< vtkGenericDataObjectWriter > writer = vtkSmartPointer< vtkGenericDataObjectWriter >::New();
    vtkSmartPointer< vtkPolyData > vtkMesh = vtkSmartPointer< vtkPolyData >::New();
    ::vtkIO::helper::Mesh::toVTKMesh( pMesh, vtkMesh);
    writer->SetInput( vtkMesh );
    writer->SetFileName(this->getFile().string().c_str());
    writer->SetFileTypeToBinary();

    //add progress observation
    Progressor progress(writer, this->getSptr(), this->getFile().string());

    writer->Write();
    vtkMesh->Delete();
}

//------------------------------------------------------------------------------

std::string  MeshWriter::extension()
{
   return ".vtk";
}

} // namespace vtkIO

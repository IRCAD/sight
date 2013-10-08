/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <vtkPolyData.h>
#include <vtkGenericDataObjectWriter.h>
#include <vtkSmartPointer.h>

#include <fwCore/base.hpp>

#include <fwDataIO/writer/registry/macros.hpp>

#include "fwVtkIO/vtk.hpp"
#include "fwVtkIO/TriangularMeshWriter.hpp"
#include "fwVtkIO/helper/ProgressVtkToFw.hpp"

fwDataIOWriterRegisterMacro( ::fwVtkIO::TriangularMeshWriter );


namespace fwVtkIO
{
//------------------------------------------------------------------------------

TriangularMeshWriter::TriangularMeshWriter(::fwDataIO::writer::IObjectWriter::Key key)
: ::fwData::location::enableSingleFile< ::fwDataIO::writer::IObjectWriter >(this)
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

TriangularMeshWriter::~TriangularMeshWriter()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void TriangularMeshWriter::write()
{
    assert( !m_object.expired() );
    assert( m_object.lock() );

    ::fwData::TriangularMesh::sptr pTriangularMesh = getConcreteObject();

    vtkSmartPointer< vtkGenericDataObjectWriter > writer = vtkSmartPointer< vtkGenericDataObjectWriter >::New();
    vtkPolyData* vtkMesh = ::fwVtkIO::toVTKMesh( pTriangularMesh );
    writer->SetInputData( vtkMesh );
    writer->SetFileName(this->getFile().string().c_str());
    writer->SetFileTypeToBinary();

    //add progress observation
    Progressor progress(writer, this->getSptr(), this->getFile().string());

    writer->Write();
    vtkMesh->Delete();
}

//------------------------------------------------------------------------------

std::string  TriangularMeshWriter::extension()
{
   return ".trian";
}

} // namespace fwVtkIO

/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <vtkPolyData.h>
#include <vtkGenericDataObjectWriter.h>


#include <fwTools/ClassRegistrar.hpp>

#include <fwCore/base.hpp>

#include "vtkIO/vtk.hpp"
#include "vtkIO/MeshWriter.hpp"
#include "vtkIO/ProgressVtktoFw.hpp"

REGISTER_BINDING_BYCLASSNAME( ::fwDataIO::writer::IObjectWriter , ::vtkIO::MeshWriter, ::vtkIO::MeshWriter );


namespace vtkIO
{
//------------------------------------------------------------------------------

MeshWriter::MeshWriter()
: ::fwData::location::enableSingleFile< ::fwDataIO::writer::IObjectWriter >(this)
{
    SLM_TRACE("vtkIO::MeshWriter::MeshWriter");
}

//------------------------------------------------------------------------------

MeshWriter::~MeshWriter()
{
    SLM_TRACE("vtkIO::MeshWriter::~MeshWriter");
}

//------------------------------------------------------------------------------

void MeshWriter::write()
{
    assert( m_object.use_count() );
    assert( !m_object.expired() );
    assert( m_object.lock() );

    ::fwData::TriangularMesh::sptr pTriangularMesh = getConcreteObject();

    vtkGenericDataObjectWriter *writer = vtkGenericDataObjectWriter::New();
    writer->SetInput( ::vtkIO::toVTKMesh( pTriangularMesh ) );
    writer->SetFileName(this->getFile().string().c_str());
    writer->SetFileTypeToBinary ();

    //add progress observation
    ::vtkIO::ProgressVtktoFw( writer, this, getFile().string() );

    writer->Write();
    writer->Delete();
}

//------------------------------------------------------------------------------

std::string  MeshWriter::extension()
{
   return ".trian";
}

} // namespace vtkIO

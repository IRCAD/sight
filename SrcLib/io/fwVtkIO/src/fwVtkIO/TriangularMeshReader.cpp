/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <vtkGenericDataObjectReader.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>

#include <fwCore/base.hpp>

#include <fwDataIO/reader/registry/macros.hpp>

#include "fwVtkIO/vtk.hpp"
#include "fwVtkIO/TriangularMeshReader.hpp"
#include "fwVtkIO/helper/ProgressVtkToFw.hpp"

fwDataIOReaderRegisterMacro( ::fwVtkIO::TriangularMeshReader );


namespace fwVtkIO
{
//------------------------------------------------------------------------------

TriangularMeshReader::TriangularMeshReader(::fwDataIO::reader::IObjectReader::Key key) :
                ::fwData::location::enableSingleFile< ::fwDataIO::reader::IObjectReader >(this)
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

TriangularMeshReader::~TriangularMeshReader()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void TriangularMeshReader::read()
{
    assert( !m_object.expired() );
    assert( m_object.lock() );

    ::fwData::TriangularMesh::sptr pTriangularMesh = getConcreteObject();

    vtkSmartPointer< vtkGenericDataObjectReader > reader = vtkSmartPointer< vtkGenericDataObjectReader >::New();
    reader->SetFileName(this->getFile().string().c_str());

    //add progress observation
    Progressor progress(reader, this->getSptr(), this->getFile().string());

    reader->Update();

    vtkDataObject *obj = reader->GetOutput();
    vtkPolyData* mesh = vtkPolyData::SafeDownCast(obj);
    if(mesh)
    {
        ::fwVtkIO::fromVTKMesh(mesh, pTriangularMesh);
    }
    else
    {
        std::string errMsg;
        errMsg  = "TriangularMeshReader cannot read VTK Mesh file : ";
        errMsg.append( this->getFile().string() );
        throw( errMsg );
    }
}

//------------------------------------------------------------------------------

std::string  TriangularMeshReader::extension()
{
   return ".trian";
}

} // namespace fwVtkIO

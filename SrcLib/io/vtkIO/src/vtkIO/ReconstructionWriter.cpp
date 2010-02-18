/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/filesystem.hpp>
#include <boost/bind.hpp>
#include <boost/lambda/lambda.hpp>

#include <vtkPolyData.h>
#include <vtkDataSetMapper.h>
#include <vtkPolyDataNormals.h>
#include <vtkProperty.h>
#include <vtkOBJExporter.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkPolyDataMapper.h>

#include <fwTools/ClassRegistrar.hpp>

#include <fwCore/base.hpp>

#include "vtkIO/vtk.hpp"
#include "vtkIO/ReconstructionWriter.hpp"

REGISTER_BINDING_BYCLASSNAME( ::fwDataIO::writer::IObjectWriter , ::vtkIO::ReconstructionWriter, ::vtkIO::ReconstructionWriter );


namespace vtkIO
{
//------------------------------------------------------------------------------

ReconstructionWriter::ReconstructionWriter()
: ::fwData::location::enableSingleFile< ::fwDataIO::writer::IObjectWriter >(this)
{
    SLM_TRACE("vtkIO::ReconstructionWriter::ReconstructionWriter");
}

//------------------------------------------------------------------------------

ReconstructionWriter::~ReconstructionWriter()
{
    SLM_TRACE("vtkIO::ReconstructionWriter::~ReconstructionWriter");
}

//------------------------------------------------------------------------------

void ReconstructionWriter::write()
{
    assert( m_object.use_count() );
    assert( !m_object.expired() );
    assert( m_object.lock() );

    std::string prefix =  this->getFile().string();
    if (prefix.find(".obj") != std::string::npos)
    {
        prefix = prefix.substr(0, prefix.length()-4);
    }
    ::fwData::Acquisition::sptr pAcquisition = getConcreteObject();

    ::fwData::Acquisition::ReconstructionIterator iter;
    vtkRenderer *renderer = vtkRenderer::New();

    for (iter = pAcquisition->getReconstructions().first; iter != pAcquisition->getReconstructions().second; ++iter)
    {
        vtkActor * actor = createActor(*iter);
        renderer->AddActor(actor);
        actor->Delete();
    }

    vtkRenderWindow *renderWindow = vtkRenderWindow::New();
    renderWindow->AddRenderer(renderer);

    vtkOBJExporter *exporter = vtkOBJExporter::New();
    exporter->SetRenderWindow(renderWindow);
    exporter->SetFilePrefix(prefix.c_str());
    exporter->Write();

    renderer->Delete();
    renderWindow->Delete();
    exporter->Delete();
}

vtkActor * ReconstructionWriter::createActor( ::fwData::Reconstruction::sptr pReconstruction )
{
    vtkActor* actor = vtkActor::New();

    ::fwData::TriangularMesh::sptr mesh = pReconstruction->getTriangularMesh();
    ::fwData::Material::sptr material = pReconstruction->getMaterial();

    vtkPolyData * polyData       = ::vtkIO::toVTKMesh(mesh);
    vtkPolyDataMapper  * mapper  = vtkPolyDataMapper::New();
    vtkPolyDataNormals* normals = vtkPolyDataNormals::New();
    normals->SetInput(polyData);
    normals->ComputePointNormalsOn ();
    normals->ComputeCellNormalsOn ();
    normals->ConsistencyOn ();
    normals->SplittingOn ();
    normals->SetFeatureAngle(180);
    mapper->SetInputConnection(normals->GetOutputPort());
    actor->SetMapper(mapper);
    mapper->Delete();
    polyData->Delete();
    normals->Delete();

    ::fwData::Color::sptr color = material->ambient();
    vtkProperty *property = actor->GetProperty();
    property->SetColor( color->red(), color->green(), color->blue());
    property->SetSpecularColor(1.,1.,1.);
    property->SetSpecularPower(100.); //Shininess
    property->SetAmbient(.05);
    property->SetDiffuse(1.);
    property->SetSpecular(1.);
    property->SetInterpolationToPhong();
    property->SetOpacity( color->alpha() );

    return actor;
}

//------------------------------------------------------------------------------

std::string ReconstructionWriter::extension()
{
    return ".obj";
}

} // namespace dicomIO

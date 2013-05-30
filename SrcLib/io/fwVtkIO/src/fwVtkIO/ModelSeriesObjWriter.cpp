/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/filesystem.hpp>

#include <vtkActor.h>
#include <vtkPolyData.h>
#include <vtkProperty.h>
#include <vtkOBJExporter.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkPolyDataMapper.h>
#include <vtkSmartPointer.h>

#include <fwCore/base.hpp>

#include <fwData/Reconstruction.hpp>
#include <fwData/Material.hpp>

#include <fwDataIO/writer/registry/macros.hpp>

#include <fwMedData/ModelSeries.hpp>

#include <fwTools/UUID.hpp>

#include "fwVtkIO/helper/Mesh.hpp"
#include "fwVtkIO/vtk.hpp"
#include "fwVtkIO/ModelSeriesObjWriter.hpp"

fwDataIOWriterRegisterMacro( ::fwVtkIO::ModelSeriesObjWriter );


namespace fwVtkIO
{
//------------------------------------------------------------------------------

ModelSeriesObjWriter::ModelSeriesObjWriter(::fwDataIO::writer::IObjectWriter::Key key)
: ::fwData::location::enableFolder< ::fwDataIO::writer::IObjectWriter >(this)
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

ModelSeriesObjWriter::~ModelSeriesObjWriter()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

vtkSmartPointer< vtkActor > createActor( const ::fwData::Reconstruction::sptr & pReconstruction )
{
    vtkSmartPointer< vtkActor >  actor = vtkSmartPointer< vtkActor >::New();

    ::fwData::Mesh::sptr mesh = pReconstruction->getMesh();
    ::fwData::Material::sptr material = pReconstruction->getMaterial();

    vtkSmartPointer< vtkPolyData > polyData = vtkSmartPointer< vtkPolyData >::New();
    ::fwVtkIO::helper::Mesh::toVTKMesh( mesh, polyData);
    vtkSmartPointer<vtkPolyDataMapper> mapper  = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInput(polyData);
    actor->SetMapper(mapper);

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

void ModelSeriesObjWriter::write()
{
    assert( !m_object.expired() );
    assert( m_object.lock() );

    ::boost::filesystem::path prefix =  this->getFolder();

    ::fwMedData::ModelSeries::sptr modelSeries = getConcreteObject();

    BOOST_FOREACH( ::fwData::Reconstruction::sptr rec, modelSeries->getReconstructionDB() )
    {
        vtkSmartPointer< vtkRenderer > renderer = vtkSmartPointer< vtkRenderer >::New();
        vtkSmartPointer< vtkActor >  actor = createActor(rec);
        renderer->AddActor(actor);

        vtkSmartPointer< vtkRenderWindow > renderWindow = vtkSmartPointer< vtkRenderWindow >::New();
        renderWindow->AddRenderer(renderer);

        std::string filename = (prefix / (rec->getOrganName() + "_" + ::fwTools::UUID::get(rec))).string();

        vtkSmartPointer< vtkOBJExporter > exporter = vtkSmartPointer< vtkOBJExporter >::New();
        exporter->SetRenderWindow(renderWindow);
        exporter->SetFilePrefix(filename.c_str());
        exporter->Write();

        // can not observe progression, not a vtkAlgorithm ...
    }
}

//------------------------------------------------------------------------------

std::string ModelSeriesObjWriter::extension()
{
    return ".obj";
}

} // namespace fwVtkIO


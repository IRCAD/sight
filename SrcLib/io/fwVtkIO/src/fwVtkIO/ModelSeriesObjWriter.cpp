/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwVtkIO/ModelSeriesObjWriter.hpp"

#include "fwVtkIO/helper/Mesh.hpp"
#include "fwVtkIO/vtk.hpp"

#include <fwCore/base.hpp>

#include <fwData/Material.hpp>
#include <fwData/Reconstruction.hpp>

#include <fwDataIO/writer/registry/macros.hpp>

#include <fwJobs/IJob.hpp>
#include <fwJobs/Observer.hpp>

#include <fwMedData/ModelSeries.hpp>

#include <fwTools/UUID.hpp>

#include <boost/filesystem.hpp>

#include <vtkActor.h>
#include <vtkOBJExporter.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkSmartPointer.h>

fwDataIOWriterRegisterMacro( ::fwVtkIO::ModelSeriesObjWriter );

namespace fwVtkIO
{
//------------------------------------------------------------------------------

ModelSeriesObjWriter::ModelSeriesObjWriter(::fwDataIO::writer::IObjectWriter::Key key) :
    ::fwData::location::enableFolder< ::fwDataIO::writer::IObjectWriter >(this),
    m_job(::fwJobs::Observer::New("ModelSeries Writer"))
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

ModelSeriesObjWriter::~ModelSeriesObjWriter()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

vtkSmartPointer< vtkActor > createActor( const ::fwData::Reconstruction::sptr& pReconstruction )
{
    vtkSmartPointer< vtkActor >  actor = vtkSmartPointer< vtkActor >::New();

    ::fwData::Mesh::sptr mesh         = pReconstruction->getMesh();
    ::fwData::Material::sptr material = pReconstruction->getMaterial();

    vtkSmartPointer< vtkPolyData > polyData = vtkSmartPointer< vtkPolyData >::New();
    ::fwVtkIO::helper::Mesh::toVTKMesh( mesh, polyData);
    vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputData(polyData);
    actor->SetMapper(mapper);

    vtkProperty* property = actor->GetProperty();

    ::fwData::Color::sptr diffuse = material->diffuse();
    property->SetDiffuseColor(diffuse->red(), diffuse->green(), diffuse->blue());
    property->SetOpacity( diffuse->alpha() );

    ::fwData::Color::sptr ambient = material->ambient();
    property->SetAmbientColor(ambient->red(), ambient->green(), ambient->blue());

    property->SetSpecularColor(1., 1., 1.);
    property->SetSpecularPower(100.); //Shininess

    property->SetInterpolationToPhong();

    return actor;
}

//------------------------------------------------------------------------------

void ModelSeriesObjWriter::write()
{
    assert( !m_object.expired() );
    assert( m_object.lock() );

    ::boost::filesystem::path prefix = this->getFolder();

    ::fwMedData::ModelSeries::csptr modelSeries = getConcreteObject();

    m_job->setTotalWorkUnits(modelSeries->getReconstructionDB().size());
    std::uint64_t units = 0;
    for(const ::fwData::Reconstruction::sptr& rec :  modelSeries->getReconstructionDB() )
    {
        vtkSmartPointer< vtkRenderer > renderer = vtkSmartPointer< vtkRenderer >::New();
        vtkSmartPointer< vtkActor >  actor      = createActor(rec);
        renderer->AddActor(actor);

        vtkSmartPointer< vtkRenderWindow > renderWindow = vtkSmartPointer< vtkRenderWindow >::New();
        renderWindow->AddRenderer(renderer);

        std::string filename = (prefix / (rec->getOrganName() + "_" + ::fwTools::UUID::get(rec))).string();

        vtkSmartPointer< vtkOBJExporter > exporter = vtkSmartPointer< vtkOBJExporter >::New();
        exporter->SetRenderWindow(renderWindow);
        exporter->SetFilePrefix(filename.c_str());
        exporter->Write();
        m_job->doneWork(++units);
        // can not observe progression, not a vtkAlgorithm ...
    }
    m_job->finish();
}

//------------------------------------------------------------------------------

std::string ModelSeriesObjWriter::extension()
{
    return ".obj";
}

//------------------------------------------------------------------------------

::fwJobs::IJob::sptr ModelSeriesObjWriter::getJob() const
{
    return m_job;
}

} // namespace fwVtkIO

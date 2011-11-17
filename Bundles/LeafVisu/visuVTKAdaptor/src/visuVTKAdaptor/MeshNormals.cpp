/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <vtkCellCenters.h>
#include <vtkActor.h>
#include <vtkArrowSource.h>
#include <vtkGlyph3D.h>
#include <vtkMaskPoints.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>

#include <fwData/Mesh.hpp>

#include <fwServices/macros.hpp>
#include <fwComEd/MeshMsg.hpp>

#include <vtkIO/helper/Mesh.hpp>

#include "visuVTKAdaptor/MeshNormals.hpp"

REGISTER_SERVICE( ::fwRenderVTK::IVtkAdaptorService, ::visuVTKAdaptor::MeshNormals, ::fwData::Mesh ) ;

namespace visuVTKAdaptor
{

//------------------------------------------------------------------------------

MeshNormals::MeshNormals() throw()
{
    m_actor = vtkActor::New();

    addNewHandledEvent (::fwComEd::MeshMsg::NEW_MESH );
    addNewHandledEvent (::fwComEd::MeshMsg::VERTEX_MODIFIED );
    addNewHandledEvent (::fwComEd::MeshMsg::POINT_COLORS_MODIFIED );
    addNewHandledEvent (::fwComEd::MeshMsg::CELL_COLORS_MODIFIED );
    addNewHandledEvent (::fwComEd::MeshMsg::POINT_NORMALS_MODIFIED );
    addNewHandledEvent (::fwComEd::MeshMsg::CELL_NORMALS_MODIFIED );
}

//------------------------------------------------------------------------------

MeshNormals::~MeshNormals() throw()
{
    m_actor->Delete();
    m_actor = 0;
}

//------------------------------------------------------------------------------

void MeshNormals::configuring() throw( ::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    assert(m_configuration->getName() == "config");
    this->setPickerId( m_configuration->getAttributeValue("picker") );
    this->setRenderId( m_configuration->getAttributeValue("renderer") );
}

//------------------------------------------------------------------------------

void MeshNormals::doStart() throw( ::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    this->doUpdate();
    this->addToRenderer(this->getActor());
}

//------------------------------------------------------------------------------

void MeshNormals::doSwap() throw( ::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    this->doUpdate();
}

//------------------------------------------------------------------------------

void MeshNormals::doUpdate() throw( ::fwTools::Failed)
{
    this->updateMeshNormals();
}

//------------------------------------------------------------------------------

vtkActor* MeshNormals::getActor()
{
    return m_actor;
}

//------------------------------------------------------------------------------

void MeshNormals::setPolyData(vtkSmartPointer< vtkPolyData > polydata)
{
    if (polydata)
    {
        m_polyData = polydata;
    }
}
//
//------------------------------------------------------------------------------

void MeshNormals::updateMeshNormals()
{
    //if(!m_polyData)
    {
        ::fwData::Mesh::sptr mesh = this->getObject < ::fwData::Mesh >();
        m_polyData = vtkSmartPointer< vtkPolyData >::New();
        ::vtkIO::helper::Mesh::toVTKMesh(mesh, m_polyData);
    }

    vtkCellCenters* ptMask = vtkCellCenters::New();
//    vtkMaskPoints* ptMask = vtkMaskPoints::New();
    ptMask->SetInput(m_polyData);
//    ptMask->SetOnRatio(1);
//    ptMask->RandomModeOn();

    vtkArrowSource* arrow = vtkArrowSource::New();

    vtkGlyph3D* glyph = vtkGlyph3D::New();
    glyph->SetInputConnection(ptMask->GetOutputPort());
    glyph->SetSource(arrow->GetOutput());
    glyph->SetVectorModeToUseNormal();
    glyph->SetScaleModeToScaleByVector();
    glyph->SetScaleFactor(10.0);
    vtkPolyDataMapper* glyphMapper = vtkPolyDataMapper::New();
    glyphMapper->SetInputConnection(glyph->GetOutputPort());

    this->getActor()->SetMapper(glyphMapper);
    this->getActor()->GetProperty()->SetColor(1., 1., 1.);

    glyphMapper->Delete();
    glyph->Delete();
    arrow->Delete();
    this->setVtkPipelineModified();
}

//------------------------------------------------------------------------------

void MeshNormals::doUpdate( ::fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    this->doUpdate();
}

//------------------------------------------------------------------------------
void MeshNormals::doStop() throw( ::fwTools::Failed)
{
    this->removeAllPropFromRenderer();
}

//------------------------------------------------------------------------------

} //namespace visuVTKAdaptor

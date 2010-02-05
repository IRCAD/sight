/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwData/TriangularMesh.hpp>
#include <fwData/Material.hpp>

#include <fwServices/macros.hpp>
#include <fwComEd/ReconstructionMsg.hpp>

#include <vtkActor.h>
#include <vtkActor2D.h>
#include <vtkArrowSource.h>
#include <vtkCamera.h>
#include <vtkGlyph3D.h>
#include <vtkIdFilter.h>
#include <vtkCellCenters.h>
#include <vtkLabeledDataMapper.h>
#include <vtkMaskPoints.h>
#include <vtkMath.h>
#include <vtkPicker.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataNormals.h>
#include <vtkTextProperty.h>
#include <vtkProperty.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkTransform.h>

#include <vtkIO/vtk.hpp>

#include "visuVTKAdaptor/Normals.hpp"



REGISTER_SERVICE( ::fwRenderVTK::IVtkAdaptorService, ::visuVTKAdaptor::Normals, ::fwData::TriangularMesh ) ;

namespace visuVTKAdaptor
{

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
Normals::Normals() throw()
{
    m_actor = vtkActor::New();
#ifdef SHOW_ID_LABELS
    m_ptsIdActor2D = vtkActor2D::New();
    m_cellsIdActor2D = vtkActor2D::New();
#endif
}

//------------------------------------------------------------------------------

Normals::~Normals() throw()
{
    m_actor->Delete();
    m_actor = 0;
#ifdef SHOW_ID_LABELS
    m_ptsIdActor2D->Delete();
    m_ptsIdActor2D = 0;
    m_cellsIdActor2D->Delete();
    m_cellsIdActor2D = 0;
#endif
}

//------------------------------------------------------------------------------

void Normals::configuring() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();

    assert(m_configuration->getName() == "config");
    this->setPickerId( m_configuration->getAttributeValue("picker") );
    this->setRenderId( m_configuration->getAttributeValue("renderer") );
}

//------------------------------------------------------------------------------

void Normals::doStart() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();

    this->doUpdate();

    this->addToRenderer(this->getActor());

#ifdef SHOW_ID_LABELS
    this->addToRenderer(m_ptsIdActor2D);
    this->addToRenderer(m_cellsIdActor2D);
#endif
}

//------------------------------------------------------------------------------

void Normals::doSwap() throw(fwTools::Failed)
{
    SLM_TRACE("SWAPPING Normals");
    this->doUpdate();
}

//------------------------------------------------------------------------------
void Normals::doUpdate() throw(fwTools::Failed)
{
    updateNormals();
}

//------------------------------------------------------------------------------

vtkActor* Normals::getActor()
{
    return m_actor;
}


//------------------------------------------------------------------------------
void Normals::setMapperInput(vtkAlgorithmOutput *input)
{
    if (input)
    {
        m_mapperInput = input;
    }
}
//
//------------------------------------------------------------------------------

void Normals::updateNormals()
{

#ifdef SHOW_ID_LABELS
    { //Points Ids
        vtkIdFilter *ids = vtkIdFilter::New();
        vtkLabeledDataMapper *idsMapper = vtkLabeledDataMapper::New();
        idsMapper->GetLabelTextProperty()->ShadowOn();
        idsMapper->GetLabelTextProperty()->SetFontSize(18);

        ids->SetInput(m_mapperInput->GetOutput());
        idsMapper->SetInputConnection(ids->GetOutputPort());
        m_ptsIdActor2D->SetMapper(idsMapper);

        idsMapper->Delete();
        ids->Delete();
    }
    { //Points cell
        vtkCellCenters *cc = vtkCellCenters::New();
        vtkLabeledDataMapper *ccMapper = vtkLabeledDataMapper::New();
        ccMapper->GetLabelTextProperty()->ShadowOn();
        ccMapper->GetLabelTextProperty()->SetFontSize(18);
        ccMapper->GetLabelTextProperty()->SetColor(0, 1, 0);


        cc->SetInput(m_mapperInput->GetOutput());
        ccMapper->SetInputConnection(cc->GetOutputPort());
        m_cellsIdActor2D->SetMapper(ccMapper);

        ccMapper->Delete();
        cc->Delete();
    }
#endif

    vtkMaskPoints* ptMask = vtkMaskPoints::New();
    ptMask->SetInputConnection(m_mapperInput);
    ptMask->SetOnRatio(1);
    ptMask->RandomModeOn();

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

void Normals::doUpdate( ::fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------
void Normals::doStop() throw(fwTools::Failed)
{
    this->removeAllPropFromRenderer();
}

//------------------------------------------------------------------------------



} //namespace visuVTKAdaptor

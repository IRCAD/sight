/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/assign/list_of.hpp>

#include <vtkCellCenters.h>
#include <vtkActor.h>
#include <vtkArrowSource.h>
#include <vtkGlyph3D.h>
#include <vtkGlyphSource2D.h>
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

std::map< std::string, MeshNormals::NormalRepresentation >
MeshNormals::m_normalRepresentationConversion
        = ::boost::assign::map_list_of(std::string("POINT"), POINT_NORMAL)
                                      (std::string("CELL"), CELL_NORMAL)
                                      (std::string("NONE"), NONE);



//------------------------------------------------------------------------------

MeshNormals::MeshNormals() throw() : m_normalRepresentation(CELL_NORMAL)
{
    m_actor = vtkActor::New();

    addNewHandledEvent (::fwComEd::MeshMsg::NEW_MESH );
    addNewHandledEvent (::fwComEd::MeshMsg::VERTEX_MODIFIED );
    addNewHandledEvent (::fwComEd::MeshMsg::POINT_NORMALS_MODIFIED );
    addNewHandledEvent (::fwComEd::MeshMsg::CELL_NORMALS_MODIFIED );

    addNewHandledEvent ("SHOW_CELL_NORMALS");
    addNewHandledEvent ("SHOW_POINT_NORMALS");
    addNewHandledEvent ("HIDE_NORMALS");
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
    if(m_configuration->hasAttribute("normal") )
    {
        std::string normal = m_configuration->getExistingAttributeValue("normal");
        SLM_ASSERT("Wrong normal representation '"<<normal << "' (required POINT, CELL or NONE)",
                m_normalRepresentationConversion.find(normal) != m_normalRepresentationConversion.end());

        m_normalRepresentation = m_normalRepresentationConversion[normal];
    }
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

//------------------------------------------------------------------------------

void MeshNormals::updateMeshNormals()
{
    ::fwData::Mesh::sptr mesh = this->getObject < ::fwData::Mesh >();

    if(m_normalRepresentation == NONE)
    {
        m_actor->SetVisibility( false );
    }
    else
    {
        m_polyData = vtkSmartPointer< vtkPolyData >::New();
        ::vtkIO::helper::Mesh::toVTKMesh(mesh, m_polyData);

        vtkSmartPointer<vtkPolyDataAlgorithm> algo;
        if(m_normalRepresentation == CELL_NORMAL)
        {
            algo = vtkSmartPointer<vtkCellCenters>::New();
        }
        else if(m_normalRepresentation == POINT_NORMAL)
        {
            vtkSmartPointer<vtkMaskPoints> ptMask = vtkSmartPointer<vtkMaskPoints>::New();
            ptMask->SetOnRatio(1);
            ptMask->RandomModeOn();
            algo = ptMask;
        }

        algo->SetInput(m_polyData);

        //vtkSmartPointer<vtkArrowSource> arrow = vtkSmartPointer<vtkArrowSource>::New();
        vtkSmartPointer<vtkGlyphSource2D> arrow = vtkSmartPointer<vtkGlyphSource2D>::New();

        //arrow->SetGlyphTypeToVertex ();
        //arrow->SetGlyphTypeToDash ();
        //arrow->SetGlyphTypeToCross ();
        //arrow->SetGlyphTypeToThickCross ();
        //arrow->SetGlyphTypeToTriangle ();
        //arrow->SetGlyphTypeToSquare ();
        //arrow->SetGlyphTypeToCircle ();
        //arrow->SetGlyphTypeToDiamond ();
        arrow->SetGlyphTypeToArrow ();
        //arrow->SetGlyphTypeToThickArrow ();
        //arrow->SetGlyphTypeToHookedArrow ();
        //arrow->SetGlyphTypeToEdgeArrow ();

        arrow->FilledOff();

        vtkSmartPointer<vtkGlyph3D> glyph = vtkSmartPointer<vtkGlyph3D>::New();
        glyph->SetInputConnection(algo->GetOutputPort());
        glyph->SetSource(arrow->GetOutput());
        glyph->SetVectorModeToUseNormal();
        glyph->SetScaleModeToScaleByVector();
        glyph->SetScaleFactor(10.0);

        vtkSmartPointer<vtkPolyDataMapper> glyphMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
        glyphMapper->SetInputConnection(glyph->GetOutputPort());

        m_actor->SetVisibility( true );
        this->getActor()->SetMapper(glyphMapper);
        this->getActor()->GetProperty()->SetColor(1., 1., 1.);
    }
    this->setVtkPipelineModified();
}

//------------------------------------------------------------------------------

void MeshNormals::doUpdate( ::fwServices::ObjectMsg::csptr msg ) throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    ::fwComEd::MeshMsg::csptr meshMsg = ::fwComEd::MeshMsg::dynamicConstCast(msg);
    ::fwData::Mesh::sptr mesh = this->getObject < ::fwData::Mesh >();

    if( meshMsg && meshMsg->hasEvent(::fwComEd::MeshMsg::NEW_MESH))
    {
        this->updateMeshNormals();
    }
    if( meshMsg && meshMsg->hasEvent(::fwComEd::MeshMsg::VERTEX_MODIFIED) )
    {
        ::vtkIO::helper::Mesh::updatePolyDataPoints(m_polyData, mesh);
        ::vtkIO::helper::Mesh::updatePolyDataPointNormals(m_polyData, mesh);
        ::vtkIO::helper::Mesh::updatePolyDataPointNormals(m_polyData, mesh);
        this->setVtkPipelineModified();
    }
    if( meshMsg && meshMsg->hasEvent(::fwComEd::MeshMsg::POINT_NORMALS_MODIFIED))
    {
        ::fwData::Mesh::sptr mesh = this->getObject < ::fwData::Mesh >();
        ::vtkIO::helper::Mesh::updatePolyDataPointNormals(m_polyData, mesh);
        this->setVtkPipelineModified();
    }
    if( meshMsg && meshMsg->hasEvent(::fwComEd::MeshMsg::CELL_NORMALS_MODIFIED))
    {
        ::fwData::Mesh::sptr mesh = this->getObject < ::fwData::Mesh >();
        ::vtkIO::helper::Mesh::updatePolyDataCellNormals(m_polyData, mesh);
        this->setVtkPipelineModified();
    }

    if( meshMsg && meshMsg->hasEvent("SHOW_CELL_NORMALS"))
    {
        m_normalRepresentation = CELL_NORMAL;
        this->updateMeshNormals();
    }

    if( meshMsg && meshMsg->hasEvent("SHOW_POINT_NORMALS"))
    {
        m_normalRepresentation = POINT_NORMAL;
        this->updateMeshNormals();
    }

    if( meshMsg && meshMsg->hasEvent("HIDE_NORMALS"))
    {
        m_normalRepresentation = NONE;
        this->updateMeshNormals();
    }
}

//------------------------------------------------------------------------------
void MeshNormals::doStop() throw( ::fwTools::Failed)
{
    this->removeAllPropFromRenderer();
}

//------------------------------------------------------------------------------

} //namespace visuVTKAdaptor

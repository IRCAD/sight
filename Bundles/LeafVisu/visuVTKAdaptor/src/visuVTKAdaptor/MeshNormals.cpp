/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuVTKAdaptor/MeshNormals.hpp"

#include <fwCom/Slot.hpp>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwData/Mesh.hpp>

#include <fwServices/macros.hpp>

#include <fwVtkIO/helper/Mesh.hpp>

#include <boost/assign/list_of.hpp>

#include <vtkActor.h>
#include <vtkArrowSource.h>
#include <vtkCellCenters.h>
#include <vtkGlyph3D.h>
#include <vtkGlyphSource2D.h>
#include <vtkMaskPoints.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>

fwServicesRegisterMacro( ::fwRenderVTK::IAdaptor, ::visuVTKAdaptor::MeshNormals, ::fwData::Mesh );

namespace visuVTKAdaptor
{

std::map< std::string, MeshNormals::NormalRepresentation >
MeshNormals::m_normalRepresentationConversion
    = ::boost::assign::map_list_of(std::string("POINT"), POINT_NORMAL)
          (std::string("CELL"), CELL_NORMAL)
          (std::string("NONE"), NONE);

//------------------------------------------------------------------------------

const ::fwCom::Slots::SlotKeyType MeshNormals::s_UPDATE_VERTEX_SLOT        = "updateVertex";
const ::fwCom::Slots::SlotKeyType MeshNormals::s_UPDATE_POINT_NORMALS_SLOT = "updatePointNormals";
const ::fwCom::Slots::SlotKeyType MeshNormals::s_UPDATE_CELL_NORMALS_SLOT  = "updateCellNormals";
const ::fwCom::Slots::SlotKeyType MeshNormals::s_SHOW_POINT_NORMALS_SLOT   = "showPointNormals";
const ::fwCom::Slots::SlotKeyType MeshNormals::s_SHOW_CELL_NORMALS_SLOT    = "showCellNormals";
const ::fwCom::Slots::SlotKeyType MeshNormals::s_HIDE_NORMALS_SLOT         = "hideNormals";
const ::fwCom::Slots::SlotKeyType MeshNormals::s_UPDATE_NORMAL_MODE_SLOT   = "updateNormalMode";

//------------------------------------------------------------------------------

MeshNormals::MeshNormals() noexcept :
    m_normalRepresentation(CELL_NORMAL)
{
    m_actor = vtkActor::New();

    m_slotUpdateVertex       = ::fwCom::newSlot(&MeshNormals::updateVertex, this);
    m_slotUpdatePointNormals = ::fwCom::newSlot(&MeshNormals::updatePointNormals, this);
    m_slotUpdateCellNormals  = ::fwCom::newSlot(&MeshNormals::updateCellNormals, this);
    m_slotShowPointNormals   = ::fwCom::newSlot(&MeshNormals::showPointNormals, this);
    m_slotShowCellNormals    = ::fwCom::newSlot(&MeshNormals::showCellNormals, this);
    m_slotHideNormals        = ::fwCom::newSlot(&MeshNormals::hideNormals, this);
    m_slotUpdateNormalMode   = ::fwCom::newSlot(&MeshNormals::updateNormalMode, this);

    ::fwCom::HasSlots::m_slots(s_UPDATE_VERTEX_SLOT, m_slotUpdateVertex)
        (s_UPDATE_POINT_NORMALS_SLOT, m_slotUpdatePointNormals)
        (s_UPDATE_CELL_NORMALS_SLOT, m_slotUpdateCellNormals)
        (s_SHOW_POINT_NORMALS_SLOT, m_slotShowPointNormals)
        (s_SHOW_CELL_NORMALS_SLOT, m_slotShowCellNormals)
        (s_HIDE_NORMALS_SLOT, m_slotHideNormals)
        (s_UPDATE_NORMAL_MODE_SLOT, m_slotUpdateNormalMode)
    ;

    ::fwCom::HasSlots::m_slots.setWorker( m_associatedWorker );
}

//------------------------------------------------------------------------------

MeshNormals::~MeshNormals() noexcept
{
    m_actor->Delete();
    m_actor = 0;
}

//------------------------------------------------------------------------------

void MeshNormals::doConfigure()
{
    SLM_TRACE_FUNC();

    SLM_ASSERT("Configuration must begin with <config>", m_configuration->getName() == "config");
    if(m_configuration->hasAttribute("normal") )
    {
        std::string normal = m_configuration->getExistingAttributeValue("normal");
        SLM_ASSERT("Wrong normal representation '"<<normal << "' (required POINT, CELL or NONE)",
                   m_normalRepresentationConversion.find(normal) != m_normalRepresentationConversion.end());

        m_normalRepresentation = m_normalRepresentationConversion[normal];
    }
}

//------------------------------------------------------------------------------

void MeshNormals::doStart()
{
    SLM_TRACE_FUNC();
    this->doUpdate();
    this->addToRenderer(this->getActor());
}

//------------------------------------------------------------------------------

void MeshNormals::doSwap()
{
    SLM_TRACE_FUNC();
    this->doUpdate();
}

//------------------------------------------------------------------------------

void MeshNormals::doUpdate()
{
    this->updateMeshNormals();
}

//------------------------------------------------------------------------------

vtkActor* MeshNormals::getActor() const
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
        ::fwVtkIO::helper::Mesh::toVTKMesh(mesh, m_polyData);

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
            ptMask->SetMaximumNumberOfPoints(mesh->getNumberOfPoints());
            algo = ptMask;
        }

        algo->SetInputData(m_polyData);

        vtkSmartPointer<vtkGlyphSource2D> arrow = vtkSmartPointer<vtkGlyphSource2D>::New();
        arrow->SetGlyphTypeToArrow();
        arrow->FilledOff();

        vtkSmartPointer<vtkGlyph3D> glyph = vtkSmartPointer<vtkGlyph3D>::New();
        glyph->SetInputConnection(algo->GetOutputPort());
        glyph->SetSourceConnection(arrow->GetOutputPort());
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

void MeshNormals::doStop()
{
    this->removeAllPropFromRenderer();
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsType MeshNormals::getObjSrvConnections() const
{
    KeyConnectionsType connections;
    connections.push_back( std::make_pair( ::fwData::Mesh::s_MODIFIED_SIG, s_UPDATE_SLOT ) );
    connections.push_back( std::make_pair( ::fwData::Mesh::s_VERTEX_MODIFIED_SIG,
                                           s_UPDATE_VERTEX_SLOT ) );
    connections.push_back( std::make_pair( ::fwData::Mesh::s_POINT_NORMALS_MODIFIED_SIG,
                                           s_UPDATE_POINT_NORMALS_SLOT) );
    connections.push_back( std::make_pair( ::fwData::Mesh::s_CELL_NORMALS_MODIFIED_SIG,
                                           s_UPDATE_CELL_NORMALS_SLOT) );

    return connections;
}

//------------------------------------------------------------------------------

void MeshNormals::updateVertex()
{
    ::fwData::Mesh::sptr mesh = this->getObject < ::fwData::Mesh >();
    ::fwVtkIO::helper::Mesh::updatePolyDataPoints(m_polyData, mesh);
    ::fwVtkIO::helper::Mesh::updatePolyDataPointNormals(m_polyData, mesh);
    ::fwVtkIO::helper::Mesh::updatePolyDataPointNormals(m_polyData, mesh);
    this->setVtkPipelineModified();
    this->requestRender();
}

//------------------------------------------------------------------------------

void MeshNormals::updatePointNormals()
{
    ::fwData::Mesh::sptr mesh = this->getObject < ::fwData::Mesh >();
    ::fwVtkIO::helper::Mesh::updatePolyDataPointNormals(m_polyData, mesh);
    this->setVtkPipelineModified();
    this->requestRender();
}

//------------------------------------------------------------------------------

void MeshNormals::updateCellNormals()
{
    ::fwData::Mesh::sptr mesh = this->getObject < ::fwData::Mesh >();
    ::fwVtkIO::helper::Mesh::updatePolyDataCellNormals(m_polyData, mesh);
    this->setVtkPipelineModified();
    this->requestRender();
}

//------------------------------------------------------------------------------

void MeshNormals::showPointNormals()
{
    m_normalRepresentation = POINT_NORMAL;
    this->updateMeshNormals();
}

//------------------------------------------------------------------------------

void MeshNormals::showCellNormals()
{
    m_normalRepresentation = CELL_NORMAL;
    this->updateMeshNormals();
}

//------------------------------------------------------------------------------

void MeshNormals::hideNormals()
{
    m_normalRepresentation = NONE;
    this->updateMeshNormals();
}

//------------------------------------------------------------------------------

void MeshNormals::updateNormalMode(std::uint8_t mode)
{
    switch (mode)
    {
        case 0:
        {
            this->hideNormals();
            break;
        }
        case 1:
        {
            this->showPointNormals();
            break;
        }
        case 2:
        {
            this->showCellNormals();
            break;
        }
        default:
        {
            OSLM_ERROR("mode " << mode << " is not allowed");
            break;
        }
    }
}

//------------------------------------------------------------------------------

} //namespace visuVTKAdaptor

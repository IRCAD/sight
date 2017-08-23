/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuVTKAdaptor/SMeshNormals.hpp"

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

fwServicesRegisterMacro( ::fwRenderVTK::IAdaptor, ::visuVTKAdaptor::SMeshNormals);

namespace visuVTKAdaptor
{

const ::fwServices::IService::KeyType SMeshNormals::s_MESH_INPUT = "mesh";

//------------------------------------------------------------------------------

std::map< std::string, SMeshNormals::NormalRepresentation >
SMeshNormals::m_normalRepresentationConversion
    = ::boost::assign::map_list_of(std::string("POINT"), POINT_NORMAL)
          (std::string("CELL"), CELL_NORMAL)
          (std::string("NONE"), NONE);

//------------------------------------------------------------------------------

const ::fwCom::Slots::SlotKeyType SMeshNormals::s_UPDATE_VERTEX_SLOT        = "updateVertex";
const ::fwCom::Slots::SlotKeyType SMeshNormals::s_UPDATE_POINT_NORMALS_SLOT = "updatePointNormals";
const ::fwCom::Slots::SlotKeyType SMeshNormals::s_UPDATE_CELL_NORMALS_SLOT  = "updateCellNormals";
const ::fwCom::Slots::SlotKeyType SMeshNormals::s_SHOW_POINT_NORMALS_SLOT   = "showPointNormals";
const ::fwCom::Slots::SlotKeyType SMeshNormals::s_SHOW_CELL_NORMALS_SLOT    = "showCellNormals";
const ::fwCom::Slots::SlotKeyType SMeshNormals::s_HIDE_NORMALS_SLOT         = "hideNormals";
const ::fwCom::Slots::SlotKeyType SMeshNormals::s_UPDATE_NORMAL_MODE_SLOT   = "updateNormalMode";

//------------------------------------------------------------------------------

SMeshNormals::SMeshNormals() noexcept :
    m_normalRepresentation(CELL_NORMAL)
{
    m_actor = vtkActor::New();

    newSlot(s_UPDATE_VERTEX_SLOT, &SMeshNormals::updateVertex, this);
    newSlot(s_UPDATE_POINT_NORMALS_SLOT, &SMeshNormals::updatePointNormals, this);
    newSlot(s_UPDATE_CELL_NORMALS_SLOT, &SMeshNormals::updateCellNormals, this);
    newSlot(s_SHOW_POINT_NORMALS_SLOT, &SMeshNormals::showPointNormals, this);
    newSlot(s_SHOW_CELL_NORMALS_SLOT, &SMeshNormals::showCellNormals, this);
    newSlot(s_HIDE_NORMALS_SLOT, &SMeshNormals::hideNormals, this);
    newSlot(s_UPDATE_NORMAL_MODE_SLOT, &SMeshNormals::updateNormalMode, this);
}

//------------------------------------------------------------------------------

SMeshNormals::~SMeshNormals() noexcept
{
    m_actor->Delete();
    m_actor = 0;
}

//------------------------------------------------------------------------------

void SMeshNormals::configuring()
{
    this->configureParams();

    const ConfigType config = this->getConfigTree().get_child("service.config.<xmlattr>");

    if(config.count("normal") )
    {
        const std::string normal = config.get<std::string>("normal");
        SLM_ASSERT("Wrong normal representation '" + normal + "' (required POINT, CELL or NONE)",
                   m_normalRepresentationConversion.find(normal) != m_normalRepresentationConversion.end());

        m_normalRepresentation = m_normalRepresentationConversion[normal];
    }
}

//------------------------------------------------------------------------------

void SMeshNormals::starting()
{
    this->initialize();
    this->updating();
    this->addToRenderer(this->getActor());
    this->requestRender();
}

//------------------------------------------------------------------------------

void SMeshNormals::updating()
{
    this->updateMeshNormals();
    this->requestRender();
}

//------------------------------------------------------------------------------

vtkActor* SMeshNormals::getActor() const
{
    return m_actor;
}

//------------------------------------------------------------------------------

void SMeshNormals::setPolyData(vtkSmartPointer< vtkPolyData > polydata)
{
    if (polydata)
    {
        m_polyData = polydata;
    }
}

//------------------------------------------------------------------------------

void SMeshNormals::updateMeshNormals()
{
    ::fwData::Mesh::csptr mesh = this->getInput< ::fwData::Mesh >(s_MESH_INPUT);
    SLM_ASSERT("Missing mesh", mesh);

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
            ptMask->SetMaximumNumberOfPoints(static_cast<vtkIdType>(mesh->getNumberOfPoints()));
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
    this->requestRender();
}

//------------------------------------------------------------------------------

void SMeshNormals::stopping()
{
    this->removeAllPropFromRenderer();
    this->requestRender();
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SMeshNormals::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push( s_MESH_INPUT, ::fwData::Mesh::s_MODIFIED_SIG, s_UPDATE_SLOT);
    connections.push( s_MESH_INPUT, ::fwData::Mesh::s_VERTEX_MODIFIED_SIG, s_UPDATE_VERTEX_SLOT);
    connections.push( s_MESH_INPUT, ::fwData::Mesh::s_POINT_NORMALS_MODIFIED_SIG, s_UPDATE_POINT_NORMALS_SLOT);
    connections.push( s_MESH_INPUT, ::fwData::Mesh::s_CELL_NORMALS_MODIFIED_SIG, s_UPDATE_CELL_NORMALS_SLOT);

    return connections;
}

//------------------------------------------------------------------------------

void SMeshNormals::updateVertex()
{
    ::fwData::Mesh::sptr mesh = this->getObject < ::fwData::Mesh >();
    ::fwVtkIO::helper::Mesh::updatePolyDataPoints(m_polyData, mesh);
    ::fwVtkIO::helper::Mesh::updatePolyDataPointNormals(m_polyData, mesh);
    ::fwVtkIO::helper::Mesh::updatePolyDataPointNormals(m_polyData, mesh);
    this->setVtkPipelineModified();
    this->requestRender();
}

//------------------------------------------------------------------------------

void SMeshNormals::updatePointNormals()
{
    ::fwData::Mesh::sptr mesh = this->getObject < ::fwData::Mesh >();
    ::fwVtkIO::helper::Mesh::updatePolyDataPointNormals(m_polyData, mesh);
    this->setVtkPipelineModified();
    this->requestRender();
}

//------------------------------------------------------------------------------

void SMeshNormals::updateCellNormals()
{
    ::fwData::Mesh::sptr mesh = this->getObject < ::fwData::Mesh >();
    ::fwVtkIO::helper::Mesh::updatePolyDataCellNormals(m_polyData, mesh);
    this->setVtkPipelineModified();
    this->requestRender();
}

//------------------------------------------------------------------------------

void SMeshNormals::showPointNormals()
{
    m_normalRepresentation = POINT_NORMAL;
    this->updateMeshNormals();
}

//------------------------------------------------------------------------------

void SMeshNormals::showCellNormals()
{
    m_normalRepresentation = CELL_NORMAL;
    this->updateMeshNormals();
}

//------------------------------------------------------------------------------

void SMeshNormals::hideNormals()
{
    m_normalRepresentation = NONE;
    this->updateMeshNormals();
}

//------------------------------------------------------------------------------

void SMeshNormals::updateNormalMode(std::uint8_t mode)
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

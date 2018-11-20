/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "uiReconstructionQml/SRepresentationEditor.hpp"

#include <fwCom/Signal.hxx>

#include <fwData/Material.hpp>
#include <fwData/Mesh.hpp>
#include <fwData/Reconstruction.hpp>

namespace uiReconstructionQml
{

static const ::fwServices::IService::KeyType s_RECONSTRUCTION_INOUT = "reconstruction";

//------------------------------------------------------------------------------

SRepresentationEditor::SRepresentationEditor() noexcept
{
    this->registerObject(s_RECONSTRUCTION_INOUT, AccessType::INOUT, true);
}

//------------------------------------------------------------------------------

SRepresentationEditor::~SRepresentationEditor() noexcept
{
}

//------------------------------------------------------------------------------

void SRepresentationEditor::configuring()
{
}

//------------------------------------------------------------------------------

void SRepresentationEditor::starting()
{
    this->fwQml::IQmlEditor::starting();
}

//------------------------------------------------------------------------------

void SRepresentationEditor::stopping()
{
    this->fwQml::IQmlEditor::stopping();
}

//------------------------------------------------------------------------------

void SRepresentationEditor::updating()
{
    ::fwData::Reconstruction::sptr reconstruction = this->getInOut< ::fwData::Reconstruction >(s_RECONSTRUCTION_INOUT);
    SLM_ASSERT("'" + s_RECONSTRUCTION_INOUT + "' must be set as 'inout'", reconstruction);
    m_material = reconstruction->getMaterial();

    int representationMode = m_material->getRepresentationMode();
    int shadingMode        = m_material->getShadingMode();
    int normal             = m_material->getOptionsMode();
    Q_EMIT materialChanged(representationMode, shadingMode, normal);
}

//------------------------------------------------------------------------------

void SRepresentationEditor::onChangeRepresentation( int id )
{

    ::fwData::Material::RepresentationType selectedMode = ::fwData::Material::SURFACE;

    switch(id)
    {
        case 1:
        {
            selectedMode = ::fwData::Material::SURFACE;
            break;
        }
        case 2:
        {
            selectedMode = ::fwData::Material::POINT;
            break;
        }
        case 3:
        {
            selectedMode = ::fwData::Material::WIREFRAME;
            break;
        }
        case 4:
        {
            selectedMode = ::fwData::Material::EDGE;
            break;
        }
        default:
            selectedMode = ::fwData::Material::SURFACE;
    }

    m_material->setRepresentationMode( selectedMode );
    this->notifyMaterial();
}

//------------------------------------------------------------------------------

void SRepresentationEditor::onChangeShading(  int id )
{
    ::fwData::Material::ShadingType selectedMode = ::fwData::Material::PHONG;

    switch(id)
    {
        case 0:
        {
            selectedMode = ::fwData::Material::AMBIENT;
            break;
        }
        case 1:
        {
            selectedMode = ::fwData::Material::FLAT;
            break;
        }
        case 2:
        {
            selectedMode = ::fwData::Material::GOURAUD;
            break;
        }
        case 3:
        {
            selectedMode = ::fwData::Material::PHONG;
            break;
        }
        default:
            selectedMode = ::fwData::Material::PHONG;
    }

    m_material->setShadingMode( selectedMode );
    this->notifyMaterial();
}

//------------------------------------------------------------------------------

void SRepresentationEditor::onShowNormals(int state )
{
    ::fwData::Reconstruction::sptr reconstruction = this->getInOut< ::fwData::Reconstruction >(s_RECONSTRUCTION_INOUT);
    SLM_ASSERT("No Reconstruction!", reconstruction);

    switch(state)
    {
        case 1:
            m_material->setOptionsMode( ::fwData::Material::STANDARD );
            break;
        case 2:
            m_material->setOptionsMode( ::fwData::Material::NORMALS );
            break;
        case 3:
            m_material->setOptionsMode( ::fwData::Material::CELLS_NORMALS );
            break;
        default:
            m_material->setOptionsMode( ::fwData::Material::STANDARD );
    }

    this->notifyMaterial();

    // In VTK backend the normals is handled by the mesh and not by the material
    auto sig = reconstruction->signal< ::fwData::Reconstruction::MeshChangedSignalType >(
        ::fwData::Reconstruction::s_MESH_CHANGED_SIG);
    sig->asyncEmit(reconstruction->getMesh());
}

//------------------------------------------------------------------------------

void SRepresentationEditor::notifyMaterial()
{
    ::fwData::Reconstruction::sptr reconstruction = this->getInOut< ::fwData::Reconstruction >(s_RECONSTRUCTION_INOUT);
    SLM_ASSERT("No Reconstruction!", reconstruction);

    ::fwData::Object::ModifiedSignalType::sptr sig;
    sig = reconstruction->getMaterial()->signal< ::fwData::Object::ModifiedSignalType >(
        ::fwData::Object::s_MODIFIED_SIG);
    sig->asyncEmit();
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SRepresentationEditor::getAutoConnections() const
{
    KeyConnectionsMap connections;

    connections.push(s_RECONSTRUCTION_INOUT, ::fwData::Object::s_MODIFIED_SIG, s_UPDATE_SLOT);

    return connections;
}

//------------------------------------------------------------------------------

}

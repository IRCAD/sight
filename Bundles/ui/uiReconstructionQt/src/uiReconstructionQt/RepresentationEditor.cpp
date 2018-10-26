/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "uiReconstructionQt/RepresentationEditor.hpp"

#include <fwCom/Signal.hxx>

#include <fwData/Material.hpp>
#include <fwData/Mesh.hpp>
#include <fwData/Reconstruction.hpp>

#include <fwGuiQt/container/QtContainer.hpp>

#include <fwRuntime/operations.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/op/Get.hpp>

#include <QAbstractButton>
#include <QButtonGroup>
#include <QGroupBox>
#include <QRadioButton>
#include <QVBoxLayout>

namespace uiReconstructionQt
{

fwServicesRegisterMacro( ::fwGui::editor::IEditor, ::uiReconstructionQt::RepresentationEditor,
                         ::fwData::Reconstruction );

//------------------------------------------------------------------------------

static const ::fwServices::IService::KeyType s_RECONSTRUCTION_INOUT = "reconstruction";

//------------------------------------------------------------------------------

RepresentationEditor::RepresentationEditor() noexcept
{
    this->registerObject(s_RECONSTRUCTION_INOUT, ::fwServices::IService::AccessType::INOUT, true);
}

//------------------------------------------------------------------------------

RepresentationEditor::~RepresentationEditor() noexcept
{
}

//------------------------------------------------------------------------------

void RepresentationEditor::starting()
{
    SLM_TRACE_FUNC();
    this->create();
    ::fwGuiQt::container::QtContainer::sptr qtContainer = ::fwGuiQt::container::QtContainer::dynamicCast(
        this->getContainer() );

    QVBoxLayout* layout = new QVBoxLayout();

    QGroupBox* groupBox         = new QGroupBox(tr("Representation"));
    QVBoxLayout* layoutGroupBox = new QVBoxLayout();
    groupBox->setLayout(layoutGroupBox);

    m_buttonGroup = new QButtonGroup(groupBox);

    QRadioButton* buttonSurface = new QRadioButton( tr("Surface"), groupBox );
    buttonSurface->setMinimumSize(buttonSurface->sizeHint());
    m_buttonGroup->addButton(buttonSurface, 0);
    layoutGroupBox->addWidget(buttonSurface);
    buttonSurface->setChecked(true);

    QRadioButton* buttonPoint = new QRadioButton( tr("Point"), groupBox );
    buttonPoint->setMinimumSize(buttonPoint->sizeHint());
    m_buttonGroup->addButton(buttonPoint, 1);
    layoutGroupBox->addWidget(buttonPoint);

    QRadioButton* buttonWireframe = new QRadioButton( tr("Wireframe"), groupBox );
    buttonWireframe->setMinimumSize(buttonWireframe->sizeHint());
    m_buttonGroup->addButton(buttonWireframe, 2);
    layoutGroupBox->addWidget(buttonWireframe);

    QRadioButton* buttonEdge = new QRadioButton( tr("Edge"), groupBox );
    buttonEdge->setMinimumSize(buttonEdge->sizeHint());
    m_buttonGroup->addButton(buttonEdge, 3);
    layoutGroupBox->addWidget(buttonEdge);

    // Shading group box
    QGroupBox* groupBoxShading         = new QGroupBox(tr("Shading"));
    QVBoxLayout* layoutGroupBoxShading = new QVBoxLayout();
    groupBoxShading->setLayout(layoutGroupBoxShading);
    m_buttonGroupShading = new QButtonGroup(groupBoxShading);

    QRadioButton* buttonAmbient = new QRadioButton( tr("Ambient"), groupBoxShading );
    buttonAmbient->setMinimumSize(buttonAmbient->sizeHint());
    m_buttonGroupShading->addButton(buttonAmbient, 0);
    layoutGroupBoxShading->addWidget(buttonAmbient);
    buttonAmbient->setChecked(true);

    QRadioButton* buttonFlat = new QRadioButton( tr("Flat"), groupBoxShading );
    buttonFlat->setMinimumSize(buttonFlat->sizeHint());
    m_buttonGroupShading->addButton(buttonFlat, 1);
    layoutGroupBoxShading->addWidget(buttonFlat);
    buttonFlat->setChecked(true);

    QRadioButton* buttonGouraud = new QRadioButton( tr("Gouraud"), groupBoxShading );
    buttonGouraud->setMinimumSize(buttonGouraud->sizeHint());
    m_buttonGroupShading->addButton(buttonGouraud, 2);
    layoutGroupBoxShading->addWidget(buttonGouraud);

    QRadioButton* buttonPhong = new QRadioButton( tr("Phong"), groupBoxShading );
    buttonPhong->setMinimumSize(buttonPhong->sizeHint());
    m_buttonGroupShading->addButton(buttonPhong, 3);
    layoutGroupBoxShading->addWidget(buttonPhong);

    layout->addWidget( groupBox);
    layout->addWidget( groupBoxShading);

    QGroupBox* groupBoxNormals         = new QGroupBox(tr("Normals"));
    QVBoxLayout* layoutGroupBoxNormals = new QVBoxLayout(groupBoxNormals);
    m_normalsRadioBox = new QButtonGroup();
    QRadioButton* pointNormalsButton = new QRadioButton(tr("Show point normals"));
    QRadioButton* cellNormalsButton  = new QRadioButton(tr("Show cell normals"));
    QRadioButton* hideNormalsButton  = new QRadioButton(tr("Hide normals"));

    m_normalsRadioBox->addButton(pointNormalsButton, 1);
    m_normalsRadioBox->addButton(cellNormalsButton, 2);
    m_normalsRadioBox->addButton(hideNormalsButton, 0);

    layoutGroupBoxNormals->addWidget( pointNormalsButton);
    layoutGroupBoxNormals->addWidget( cellNormalsButton);
    layoutGroupBoxNormals->addWidget( hideNormalsButton);

    layout->addWidget(groupBoxNormals);

    QObject::connect(m_normalsRadioBox, SIGNAL(buttonClicked(int)), this, SLOT(onShowNormals(int)));

    qtContainer->setLayout( layout );
    qtContainer->setEnabled(false);

    QObject::connect(m_buttonGroup, SIGNAL(buttonClicked(int)), this, SLOT(onChangeRepresentation(int)));
    QObject::connect(m_buttonGroupShading, SIGNAL(buttonClicked(int)), this, SLOT(onChangeShading(int)));

    this->updating();
}

//------------------------------------------------------------------------------

void RepresentationEditor::stopping()
{
    SLM_TRACE_FUNC();

    QObject::disconnect(m_buttonGroup, SIGNAL(buttonClicked(int)), this, SLOT(onChangeRepresentation(int)));
    QObject::disconnect(m_buttonGroupShading, SIGNAL(buttonClicked(int)), this, SLOT(onChangeShading(int)));

    QObject::connect(m_normalsRadioBox, SIGNAL(buttonClicked(int)), this, SLOT(onShowNormals(int)));

    this->destroy();
}

//------------------------------------------------------------------------------

void RepresentationEditor::configuring()
{
    SLM_TRACE_FUNC();
    this->initialize();
}

//------------------------------------------------------------------------------

void RepresentationEditor::updating()
{
    ::fwData::Reconstruction::sptr reconstruction = this->getInOut< ::fwData::Reconstruction >(s_RECONSTRUCTION_INOUT);
    SLM_ASSERT("The inout key '" + s_RECONSTRUCTION_INOUT + "' is not defined.", reconstruction);

    ::fwGuiQt::container::QtContainer::sptr qtContainer = ::fwGuiQt::container::QtContainer::dynamicCast(
        this->getContainer() );
    QWidget* const container = qtContainer->getQtContainer();
    SLM_ASSERT("container not instanced", container);

    m_material = reconstruction->getMaterial();
    container->setEnabled(!reconstruction->getOrganName().empty());

    this->refreshRepresentation();
    this->refreshNormals();
    this->refreshShading();
}

//------------------------------------------------------------------------------

void RepresentationEditor::onChangeRepresentation( int id )
{

    ::fwData::Material::RepresentationType selectedMode = ::fwData::Material::SURFACE;

    switch(id)
    {
        case 0:
        {
            selectedMode = ::fwData::Material::SURFACE;
            break;
        }
        case 1:
        {
            selectedMode = ::fwData::Material::POINT;
            break;
        }
        case 2:
        {
            selectedMode = ::fwData::Material::WIREFRAME;
            break;
        }
        case 3:
        {
            selectedMode = ::fwData::Material::EDGE;
            break;
        }
    }

    m_material->setRepresentationMode( selectedMode );
    this->notifyMaterial();
}

//------------------------------------------------------------------------------

void RepresentationEditor::onChangeShading(  int id )
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
    }

    m_material->setShadingMode( selectedMode );
    this->notifyMaterial();
}

//------------------------------------------------------------------------------

void RepresentationEditor::refreshRepresentation()
{
    int representationMode = m_material->getRepresentationMode();
    QAbstractButton* button;

    switch(representationMode)
    {
        case ::fwData::Material::SURFACE:
        {
            button = m_buttonGroup->button(0);
            button->setChecked(true);
            break;
        }
        case ::fwData::Material::POINT:
        {
            button = m_buttonGroup->button(1);
            button->setChecked(true);
            break;
        }
        case ::fwData::Material::WIREFRAME:
        {
            button = m_buttonGroup->button(2);
            button->setChecked(true);
            break;
        }
        case ::fwData::Material::EDGE:
        {
            button = m_buttonGroup->button(3);
            button->setChecked(true);
            break;
        }
        default:
            button = m_buttonGroup->button(0);
            button->setChecked(true);
    }
}

//------------------------------------------------------------------------------

void RepresentationEditor::refreshShading()
{
    int shadingMode = m_material->getShadingMode();
    QAbstractButton* button;

    switch(shadingMode)
    {
        case ::fwData::Material::AMBIENT:
        {
            button = m_buttonGroupShading->button(0);
            button->setChecked(true);
            break;
        }
        case ::fwData::Material::FLAT:
        {
            button = m_buttonGroupShading->button(1);
            button->setChecked(true);
            break;
        }
        case ::fwData::Material::GOURAUD:
        {
            button = m_buttonGroupShading->button(2);
            button->setChecked(true);
            break;
        }
        case ::fwData::Material::PHONG:
        {
            button = m_buttonGroupShading->button(3);
            button->setChecked(true);
            break;
        }
        default:
            button = m_buttonGroupShading->button(2);
            button->setChecked(true);
    }
}

//------------------------------------------------------------------------------

void RepresentationEditor::refreshNormals()
{
    QAbstractButton* buttonHide = m_normalsRadioBox->button(0);
    buttonHide->setChecked(m_material->getOptionsMode() == ::fwData::Material::STANDARD);
    QAbstractButton* buttonNormals = m_normalsRadioBox->button(1);
    buttonNormals->setChecked(m_material->getOptionsMode() == ::fwData::Material::NORMALS);
}

//------------------------------------------------------------------------------

void RepresentationEditor::onShowNormals(int state )
{
    ::fwData::Reconstruction::sptr reconstruction = this->getInOut< ::fwData::Reconstruction >(s_RECONSTRUCTION_INOUT);
    SLM_ASSERT("The inout key '" + s_RECONSTRUCTION_INOUT + "' is not defined.", reconstruction);

    switch(state)
    {
        case 0:
            m_material->setOptionsMode( ::fwData::Material::STANDARD );
            break;
        case 1:
            m_material->setOptionsMode( ::fwData::Material::NORMALS );
            break;
        case 2:
            m_material->setOptionsMode( ::fwData::Material::CELLS_NORMALS );
            break;
    }

    this->notifyMaterial();

    // In VTK backend the normals is handled by the mesh and not by the material
    auto sig = reconstruction->signal< ::fwData::Reconstruction::MeshChangedSignalType >(
        ::fwData::Reconstruction::s_MESH_CHANGED_SIG);
    sig->asyncEmit(reconstruction->getMesh());
}

//------------------------------------------------------------------------------

void RepresentationEditor::notifyMaterial()
{
    ::fwData::Reconstruction::sptr reconstruction = this->getInOut< ::fwData::Reconstruction >(s_RECONSTRUCTION_INOUT);
    SLM_ASSERT("The inout key '" + s_RECONSTRUCTION_INOUT + "' is not defined.", reconstruction);

    ::fwData::Object::ModifiedSignalType::sptr sig;
    sig = reconstruction->getMaterial()->signal< ::fwData::Object::ModifiedSignalType >(
        ::fwData::Object::s_MODIFIED_SIG);
    sig->asyncEmit();
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap RepresentationEditor::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push(s_RECONSTRUCTION_INOUT, ::fwData::Object::s_MODIFIED_SIG, s_UPDATE_SLOT);
    return connections;
}

//------------------------------------------------------------------------------

}

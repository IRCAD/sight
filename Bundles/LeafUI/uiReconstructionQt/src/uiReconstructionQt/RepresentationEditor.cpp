/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "uiReconstructionQt/RepresentationEditor.hpp"

#include <fwCore/base.hpp>

#include <fwData/Image.hpp>
#include <fwData/Material.hpp>
#include <fwData/Mesh.hpp>
#include <fwData/Reconstruction.hpp>

#include <fwComEd/MaterialMsg.hpp>
#include <fwComEd/MeshMsg.hpp>

#include <fwRuntime/ConfigurationElement.hpp>
#include <fwRuntime/operations.hpp>

#include <fwServices/Base.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/registry/ObjectService.hpp>
#include <fwServices/IService.hpp>
#include <fwServices/op/Get.hpp>

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>

#include <fwGuiQt/container/QtContainer.hpp>

#include <QAbstractButton>
#include <QRadioButton>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QButtonGroup>

namespace uiReconstruction
{

fwServicesRegisterMacro( ::gui::editor::IEditor, ::uiReconstruction::RepresentationEditor, ::fwData::Reconstruction );


RepresentationEditor::RepresentationEditor() throw()
{
    //handlingEventOff();
}

//------------------------------------------------------------------------------

RepresentationEditor::~RepresentationEditor() throw()
{
}

//------------------------------------------------------------------------------

void RepresentationEditor::starting() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    this->create();
    ::fwGuiQt::container::QtContainer::sptr qtContainer = ::fwGuiQt::container::QtContainer::dynamicCast(
        this->getContainer() );
    QWidget* const container = qtContainer->getQtContainer();
    SLM_ASSERT("container not instanced", container);

    QVBoxLayout * layout = new QVBoxLayout();

    QGroupBox *groupBox          = new QGroupBox(tr("Representation"), container);
    QVBoxLayout * layoutGroupBox = new QVBoxLayout();
    groupBox->setLayout(layoutGroupBox);

    m_buttonGroup = new QButtonGroup(groupBox);

    QRadioButton *buttonSurface = new QRadioButton ( tr("Surface"), groupBox );
    buttonSurface->setMinimumSize(buttonSurface->sizeHint());
    m_buttonGroup->addButton(buttonSurface, 0);
    layoutGroupBox->addWidget(buttonSurface);
    buttonSurface->setChecked(true);

    QRadioButton *buttonPoint = new QRadioButton ( tr("Point"), groupBox );
    buttonPoint->setMinimumSize(buttonPoint->sizeHint());
    m_buttonGroup->addButton(buttonPoint, 1);
    layoutGroupBox->addWidget(buttonPoint);

    QRadioButton *buttonWireframe = new QRadioButton ( tr("Wireframe"), groupBox );
    buttonWireframe->setMinimumSize(buttonWireframe->sizeHint());
    m_buttonGroup->addButton(buttonWireframe, 2);
    layoutGroupBox->addWidget(buttonWireframe);

    QRadioButton *buttonEdge = new QRadioButton ( tr("Edge"), groupBox );
    buttonEdge->setMinimumSize(buttonEdge->sizeHint());
    m_buttonGroup->addButton(buttonEdge, 3);
    layoutGroupBox->addWidget(buttonEdge);

    // Shading group box
    QGroupBox *groupBoxShading          = new QGroupBox(tr("Shading"), container);
    QVBoxLayout * layoutGroupBoxShading = new QVBoxLayout();
    groupBoxShading->setLayout(layoutGroupBoxShading);
    m_buttonGroupShading = new QButtonGroup(groupBoxShading);

    QRadioButton *buttonFlat = new QRadioButton ( tr("Flat"), groupBoxShading );
    buttonFlat->setMinimumSize(buttonFlat->sizeHint());
    m_buttonGroupShading->addButton(buttonFlat, 0);
    layoutGroupBoxShading->addWidget(buttonFlat);
    buttonFlat->setChecked(true);

    QRadioButton *buttonGouraud = new QRadioButton ( tr("Gouraud"), groupBoxShading );
    buttonGouraud->setMinimumSize(buttonGouraud->sizeHint());
    m_buttonGroupShading->addButton(buttonGouraud, 1);
    layoutGroupBoxShading->addWidget(buttonGouraud);

    QRadioButton *buttonPhong = new QRadioButton ( tr("Phong"), groupBoxShading );
    buttonPhong->setMinimumSize(buttonPhong->sizeHint());
    m_buttonGroupShading->addButton(buttonPhong, 2);
    layoutGroupBoxShading->addWidget(buttonPhong);

    layout->addWidget( groupBox);
    layout->addWidget( groupBoxShading);

#ifdef _DEBUG
    QGroupBox *groupBoxNormals          = new QGroupBox(tr("Normals"), container);
    QVBoxLayout * layoutGroupBoxNormals = new QVBoxLayout(groupBoxNormals);
    m_normalsRadioBox = new QButtonGroup(container);
    QRadioButton* pointNormalsButton = new QRadioButton(tr("Show point normals"), container);
    QRadioButton* cellNormalsButton  = new QRadioButton(tr("Show cell normals"), container);
    QRadioButton* hideNormalsButton  = new QRadioButton(tr("Hide normals"), container);

    m_normalsRadioBox->addButton(pointNormalsButton, 2);
    m_normalsRadioBox->addButton(cellNormalsButton, 1);
    m_normalsRadioBox->addButton(hideNormalsButton, 0);

    layoutGroupBoxNormals->addWidget( pointNormalsButton);
    layoutGroupBoxNormals->addWidget( cellNormalsButton);
    layoutGroupBoxNormals->addWidget( hideNormalsButton);

    layout->addWidget(groupBoxNormals);

    QObject::connect(m_normalsRadioBox, SIGNAL(buttonClicked(int)), this, SLOT(onShowNormals(int)));

#endif

    container->setLayout( layout );
    container->setEnabled(false);

    QObject::connect(m_buttonGroup, SIGNAL(buttonClicked ( int )), this, SLOT(onChangeRepresentation( int )));
    QObject::connect(m_buttonGroupShading, SIGNAL(buttonClicked ( int )), this, SLOT(onChangeShading( int )));

    this->updating();
}

//------------------------------------------------------------------------------

void RepresentationEditor::stopping() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    QObject::disconnect(m_buttonGroup, SIGNAL(buttonClicked ( int )), this, SLOT(onChangeRepresentation(int )));
    QObject::disconnect(m_buttonGroupShading, SIGNAL(buttonClicked ( int )), this, SLOT(onChangeShading(int )));

#ifdef _DEBUG
    QObject::connect(m_normalsRadioBox, SIGNAL(buttonClicked(int)), this, SLOT(onShowNormals(int)));
#endif

    this->getContainer()->clean();
    this->destroy();
}

//------------------------------------------------------------------------------

void RepresentationEditor::configuring() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();
    this->initialize();
}

//------------------------------------------------------------------------------

void RepresentationEditor::updating() throw(::fwTools::Failed)
{
    ::fwData::Reconstruction::sptr reconstruction = this->getObject< ::fwData::Reconstruction>();
    SLM_ASSERT("No Reconstruction!", reconstruction);
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

void RepresentationEditor::swapping() throw(::fwTools::Failed)
{
    this->updating();
}

//------------------------------------------------------------------------------

void RepresentationEditor::receiving( ::fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed)
{
}

//------------------------------------------------------------------------------

void RepresentationEditor::info( std::ostream &_sstream )
{
}

//------------------------------------------------------------------------------

void RepresentationEditor::onChangeRepresentation( int id )
{

    ::fwData::Material::REPRESENTATION_MODE selectedMode = ::fwData::Material::MODE_SURFACE;

    switch(id)
    {
        case 0:
        {
            selectedMode = ::fwData::Material::MODE_SURFACE;
            break;
        }
        case 1:
        {
            selectedMode = ::fwData::Material::MODE_POINT;
            break;
        }
        case 2:
        {
            selectedMode = ::fwData::Material::MODE_WIREFRAME;
            break;
        }
        case 3:
        {
            selectedMode = ::fwData::Material::MODE_EDGE;
            break;
        }
    }

    m_material->setRepresentationMode( selectedMode );
    this->notifyMaterial();
}

//------------------------------------------------------------------------------

void RepresentationEditor::onChangeShading(  int id )
{
    ::fwData::Material::SHADING_MODE selectedMode = ::fwData::Material::MODE_PHONG;

    switch(id)
    {
        case 0:
        {
            selectedMode = ::fwData::Material::MODE_FLAT;
            break;
        }
        case 1:
        {
            selectedMode = ::fwData::Material::MODE_GOURAUD;
            break;
        }
        case 2:
        {
            selectedMode = ::fwData::Material::MODE_PHONG;
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
    QAbstractButton * button;

    switch(representationMode)
    {
        case ::fwData::Material::MODE_SURFACE:
        {
            button = m_buttonGroup->button(0);
            button->setChecked(true);
            break;
        }
        case ::fwData::Material::MODE_POINT:
        {
            button = m_buttonGroup->button(1);
            button->setChecked(true);
            break;
        }
        case ::fwData::Material::MODE_WIREFRAME:
        {
            button = m_buttonGroup->button(2);
            button->setChecked(true);
            break;
        }
        case ::fwData::Material::MODE_EDGE:
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
    QAbstractButton * button;

    switch(shadingMode)
    {
        case ::fwData::Material::MODE_FLAT:
        {
            button = m_buttonGroupShading->button(0);
            button->setChecked(true);
            break;
        }
        case ::fwData::Material::MODE_GOURAUD:
        {
            button = m_buttonGroupShading->button(1);
            button->setChecked(true);
            break;
        }
        case ::fwData::Material::MODE_PHONG:
        {
            button = m_buttonGroupShading->button(2);
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
#ifdef _DEBUG
    QAbstractButton *buttonHide = m_normalsRadioBox->button(0);
    buttonHide->setChecked(m_material->getOptionsMode() == ::fwData::Material::MODE_STANDARD);
    QAbstractButton *buttonNormals = m_normalsRadioBox->button(1);
    buttonNormals->setChecked(m_material->getOptionsMode() == ::fwData::Material::MODE_NORMALS);
#endif
}

//------------------------------------------------------------------------------

void RepresentationEditor::onShowNormals(int state )
{
    ::fwData::Reconstruction::sptr reconstruction = this->getObject< ::fwData::Reconstruction>();
    ::fwComEd::MeshMsg::sptr meshMsg              = ::fwComEd::MeshMsg::New();
    switch (state)
    {
        case 0:
            m_material->setOptionsMode( ::fwData::Material::MODE_STANDARD );
            meshMsg->addEvent("HIDE_NORMALS");
            break;
        case 1:
            m_material->setOptionsMode( ::fwData::Material::MODE_NORMALS );
            meshMsg->addEvent("SHOW_CELL_NORMALS");
            break;
        case 2:
            m_material->setOptionsMode( ::fwData::Material::MODE_NORMALS );
            meshMsg->addEvent("SHOW_POINT_NORMALS");
            break;
    }
    ::fwComEd::MaterialMsg::sptr msg = ::fwComEd::MaterialMsg::New();
    msg->addEvent( ::fwComEd::MaterialMsg::MATERIAL_IS_MODIFIED );
    msg->setSource(this->getSptr());
    msg->setSubject( reconstruction->getMesh());
    ::fwData::Object::ObjectModifiedSignalType::sptr sig;
    sig = reconstruction->getMesh()->signal< ::fwData::Object::ObjectModifiedSignalType >(
        ::fwData::Object::s_OBJECT_MODIFIED_SIG);
    {
        ::fwCom::Connection::Blocker block(sig->getConnection(m_slotReceive));
        sig->asyncEmit( msg);
    }
    meshMsg->setSource(this->getSptr());
    meshMsg->setSubject( reconstruction->getMesh());
    sig = reconstruction->getMesh()->signal< ::fwData::Object::ObjectModifiedSignalType >(
        ::fwData::Object::s_OBJECT_MODIFIED_SIG);
    {
        ::fwCom::Connection::Blocker block(sig->getConnection(m_slotReceive));
        sig->asyncEmit( meshMsg);
    }
}

//------------------------------------------------------------------------------

void RepresentationEditor::notifyMaterial()
{
    ::fwData::Reconstruction::sptr reconstruction = this->getObject< ::fwData::Reconstruction>();

    ::fwComEd::MaterialMsg::sptr msg = ::fwComEd::MaterialMsg::New();
    msg->addEvent( ::fwComEd::MaterialMsg::MATERIAL_IS_MODIFIED );
    msg->setSource(this->getSptr());
    msg->setSubject( reconstruction->getMaterial());
    ::fwData::Object::ObjectModifiedSignalType::sptr sig;
    sig = reconstruction->getMaterial()->signal< ::fwData::Object::ObjectModifiedSignalType >(
        ::fwData::Object::s_OBJECT_MODIFIED_SIG);
    {
        ::fwCom::Connection::Blocker block(sig->getConnection(m_slotReceive));
        sig->asyncEmit( msg);
    }
}

//------------------------------------------------------------------------------

void RepresentationEditor::notifyMesh()
{
    ::fwData::Reconstruction::sptr reconstruction = this->getObject< ::fwData::Reconstruction>();

    ::fwComEd::MaterialMsg::sptr msg = ::fwComEd::MaterialMsg::New();
    msg->addEvent( ::fwComEd::MaterialMsg::MATERIAL_IS_MODIFIED );
    msg->setSource(this->getSptr());
    msg->setSubject( reconstruction->getMesh());
    ::fwData::Object::ObjectModifiedSignalType::sptr sig;
    sig = reconstruction->getMesh()->signal< ::fwData::Object::ObjectModifiedSignalType >(
        ::fwData::Object::s_OBJECT_MODIFIED_SIG);
    {
        ::fwCom::Connection::Blocker block(sig->getConnection(m_slotReceive));
        sig->asyncEmit( msg);
    }
}
}


/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <QAbstractButton>
#include <QRadioButton>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QButtonGroup>
#include <QCheckBox>

#include <fwCore/base.hpp>

#include <fwTools/Object.hpp>

#include <fwData/Reconstruction.hpp>

#include <fwComEd/MaterialMsg.hpp>

#include <fwRuntime/ConfigurationElement.hpp>
#include <fwRuntime/operations.hpp>

#include <fwServices/Base.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/registry/ObjectService.hpp>
#include <fwServices/IService.hpp>
#include <fwServices/op/Get.hpp>
#include <fwServices/IEditionService.hpp>

#include <fwGuiQt/container/QtContainer.hpp>

#include "uiReconstructionQt/RepresentationEditor.hpp"

namespace uiReconstruction
{

REGISTER_SERVICE( ::gui::editor::IEditor , ::uiReconstruction::RepresentationEditor , ::fwData::Reconstruction ) ;


RepresentationEditor::RepresentationEditor() throw()
{
    handlingEventOff();
}

//------------------------------------------------------------------------------

RepresentationEditor::~RepresentationEditor() throw()
{}

//------------------------------------------------------------------------------

void RepresentationEditor::starting() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    this->create();
    ::fwGuiQt::container::QtContainer::sptr qtContainer =  ::fwGuiQt::container::QtContainer::dynamicCast( this->getContainer() );
    QWidget* const container = qtContainer->getQtContainer();
    SLM_ASSERT("container not instanced", container);

    QVBoxLayout * layout = new QVBoxLayout(container);

    QGroupBox *groupBox =new QGroupBox(tr("Representation"), container);
    QVBoxLayout * layoutGroupBox = new QVBoxLayout(groupBox);
    groupBox->setLayout(layoutGroupBox);

    m_buttonGroup = new QButtonGroup(groupBox);

    QRadioButton *buttonSurface = new QRadioButton ( tr("Surface"), groupBox );
    m_buttonGroup->addButton(buttonSurface, 0);
    layoutGroupBox->addWidget(buttonSurface);
    buttonSurface->setChecked(true);

    QRadioButton *buttonPoint = new QRadioButton ( tr("Point"), groupBox );
    m_buttonGroup->addButton(buttonPoint, 1);
    layoutGroupBox->addWidget(buttonPoint);

    QRadioButton *buttonWireframe = new QRadioButton ( tr("Wireframe"), groupBox );
    m_buttonGroup->addButton(buttonWireframe, 2);
    layoutGroupBox->addWidget(buttonWireframe);

    QRadioButton *buttonEdge = new QRadioButton ( tr("Edge"), groupBox );
    m_buttonGroup->addButton(buttonEdge, 3);
    layoutGroupBox->addWidget(buttonEdge);

    // Shading group box
    QGroupBox *groupBoxShading =new QGroupBox(tr("Shading"), container);
    QVBoxLayout * layoutGroupBoxShading = new QVBoxLayout(groupBoxShading);
    groupBoxShading->setLayout(layoutGroupBoxShading);
    m_buttonGroupShading = new QButtonGroup(groupBoxShading);

    QRadioButton *buttonFlat = new QRadioButton ( tr("Flat"), groupBoxShading );
    m_buttonGroupShading->addButton(buttonFlat, 0);
    layoutGroupBoxShading->addWidget(buttonFlat);
    buttonFlat->setChecked(true);

    QRadioButton *buttonGouraud = new QRadioButton ( tr("Gouraud"), groupBoxShading );
    m_buttonGroupShading->addButton(buttonGouraud, 1);
    layoutGroupBoxShading->addWidget(buttonGouraud);

    QRadioButton *buttonPhong = new QRadioButton ( tr("Phong"), groupBoxShading );
    m_buttonGroupShading->addButton(buttonPhong, 2);
    layoutGroupBoxShading->addWidget(buttonPhong);


    m_normalsCheckBox = new QCheckBox( tr("Show normals"), container);
    m_normalsCheckBox->setToolTip(tr("Show or hide normals"));

    layout->addWidget( groupBox);
    layout->addWidget( groupBoxShading);
    layout->addWidget( m_normalsCheckBox);

    container->setLayout( layout );
    container->setEnabled(false);

    QObject::connect(m_buttonGroup, SIGNAL(buttonClicked ( int )), this, SLOT(onChangeRepresentation( int )));
    QObject::connect(m_buttonGroupShading, SIGNAL(buttonClicked ( int )), this, SLOT(onChangeShading( int )));
    QObject::connect(m_normalsCheckBox, SIGNAL(stateChanged( int )), this, SLOT(onShowNormals( int )));

    this->updating();
}

//------------------------------------------------------------------------------

void RepresentationEditor::stopping() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    QObject::disconnect(m_buttonGroup, SIGNAL(buttonClicked ( QAbstractButton *)), this, SLOT(onChangeRepresentation(QAbstractButton *)));
    QObject::disconnect(m_buttonGroupShading, SIGNAL(buttonClicked ( QAbstractButton *)), this, SLOT(onChangeShading(QAbstractButton *)));
    QObject::disconnect(m_normalsCheckBox, SIGNAL(stateChanged(int )), this, SLOT(onShowNormals(int)));

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
    ::fwGuiQt::container::QtContainer::sptr qtContainer =  ::fwGuiQt::container::QtContainer::dynamicCast( this->getContainer() );
    QWidget* const container = qtContainer->getQtContainer();
    SLM_ASSERT("container not instanced", container);

    m_material = reconstruction->getMaterial() ;
    container->setEnabled(!reconstruction->getOrganName().empty());

    this->refreshRepresentation();
    this->refreshNormals();
    this->refreshShading() ;
}

//------------------------------------------------------------------------------

void RepresentationEditor::swapping() throw(::fwTools::Failed)
{
    this->updating();
}

//------------------------------------------------------------------------------

void RepresentationEditor::updating( ::fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed)
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
        case 0 :
        {
            selectedMode = ::fwData::Material::MODE_SURFACE;
            break ;
        }
        case 1 :
        {
            selectedMode = ::fwData::Material::MODE_POINT;
            break ;
        }
        case 2 :
        {
            selectedMode = ::fwData::Material::MODE_WIREFRAME;
            break ;
        }
        case 3 :
        {
            selectedMode = ::fwData::Material::MODE_EDGE;
            break ;
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
        case 0 :
        {
            selectedMode = ::fwData::Material::MODE_FLAT;
            break ;
        }
        case 1 :
        {
            selectedMode = ::fwData::Material::MODE_GOURAUD;
            break ;
        }
        case 2 :
        {
            selectedMode = ::fwData::Material::MODE_PHONG;
            break ;
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
        break ;
    }
    case ::fwData::Material::MODE_POINT:
    {
        button = m_buttonGroup->button(1);
        button->setChecked(true);
        break ;
    }
    case ::fwData::Material::MODE_WIREFRAME:
    {
        button= m_buttonGroup->button(2);
        button->setChecked(true);
        break ;
    }
    case ::fwData::Material::MODE_EDGE:
    {
        button = m_buttonGroup->button(3);
        button->setChecked(true);
        break ;
    }
    default :
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
        break ;
    }
    case ::fwData::Material::MODE_GOURAUD:
    {
        button = m_buttonGroupShading->button(1);
        button->setChecked(true);
        break ;
    }
    case ::fwData::Material::MODE_PHONG:
    {
        button = m_buttonGroupShading->button(2);
        button->setChecked(true);
        break ;
    }
    default :
        button = m_buttonGroupShading->button(2);
        button->setChecked(true);
    }
}

//------------------------------------------------------------------------------

void RepresentationEditor::refreshNormals()
{
    m_normalsCheckBox->setCheckState(m_material->getOptionsMode() == ::fwData::Material::MODE_NORMALS ? Qt::Checked : Qt::Unchecked);
}

//------------------------------------------------------------------------------

void RepresentationEditor::onShowNormals(int state )
{
    if ( state == Qt::Checked)
    {
        m_material->setOptionsMode( ::fwData::Material::MODE_NORMALS );
    }
    else
    {
        m_material->setOptionsMode( ::fwData::Material::MODE_STANDARD );
    }

    this->notifyTriangularMesh();
}

//------------------------------------------------------------------------------

void RepresentationEditor::notifyMaterial()
{
    ::fwData::Reconstruction::sptr reconstruction = this->getObject< ::fwData::Reconstruction>();

    ::fwComEd::MaterialMsg::NewSptr msg;
    msg->addEvent( ::fwComEd::MaterialMsg::MATERIAL_IS_MODIFIED ) ;
    ::fwServices::IEditionService::notify(this->getSptr(), reconstruction->getMaterial(), msg);
}

//------------------------------------------------------------------------------

void RepresentationEditor::notifyTriangularMesh()
{
    ::fwData::Reconstruction::sptr reconstruction = this->getObject< ::fwData::Reconstruction>();

    ::fwComEd::MaterialMsg::NewSptr msg;
    msg->addEvent( ::fwComEd::MaterialMsg::MATERIAL_IS_MODIFIED ) ;
    ::fwServices::IEditionService::notify(this->getSptr(), reconstruction->getTriangularMesh(), msg);
}
}


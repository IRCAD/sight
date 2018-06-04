/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "uiReconstructionQt/OrganMaterialEditor.hpp"

#include <fwCore/base.hpp>

#include <fwData/Material.hpp>
#include <fwData/Mesh.hpp>
#include <fwData/Reconstruction.hpp>

#include <fwGuiQt/container/QtContainer.hpp>

#include <fwRuntime/ConfigurationElement.hpp>
#include <fwRuntime/operations.hpp>

#include <fwServices/IService.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/op/Get.hpp>

#include <QColor>
#include <QColorDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QPixmap>
#include <QPushButton>
#include <QSlider>
#include <QStyle>
#include <QVBoxLayout>

namespace uiReconstructionQt
{

fwServicesRegisterMacro( ::fwGui::editor::IEditor, ::uiReconstructionQt::OrganMaterialEditor,
                         ::fwData::Reconstruction );

static const ::fwServices::IService::KeyType s_RECONSTRUCTION_INOUT = "reconstruction";

OrganMaterialEditor::OrganMaterialEditor() noexcept
{
    //handlingEventOff();
}

//------------------------------------------------------------------------------

OrganMaterialEditor::~OrganMaterialEditor() noexcept
{
}

//------------------------------------------------------------------------------

void OrganMaterialEditor::starting()
{
    SLM_TRACE_FUNC();
    this->create();
    ::fwGuiQt::container::QtContainer::sptr qtContainer
        = ::fwGuiQt::container::QtContainer::dynamicCast(this->getContainer() );

    m_colourButton = new QPushButton(tr("Color"));
    m_colourButton->setToolTip(tr("Selected organ's color"));
    m_colourButton->setMinimumSize(m_colourButton->sizeHint());

    QLabel* transparencyLabel = new QLabel(tr("Transparency : "));
    m_opacitySlider = new QSlider( Qt::Horizontal);
    m_opacitySlider->setToolTip(tr("Selected organ's opacity"));
    m_opacitySlider->setRange(0, 100);
    m_opacitySlider->setTickInterval(20);
    m_opacitySlider->setTickPosition(QSlider::TicksBelow);
    m_opacitySlider->setMinimumSize(m_opacitySlider->sizeHint());

    m_transparencyValue = new QLabel("");
    m_transparencyValue->setMinimumSize(m_transparencyValue->sizeHint());

    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget( m_colourButton, 0 );

    QHBoxLayout* transparencyLayout = new QHBoxLayout( );
    transparencyLayout->addWidget( transparencyLabel, 0);
    transparencyLayout->addWidget( m_opacitySlider, 1 );
    transparencyLayout->addWidget( m_transparencyValue, 0);
    layout->addLayout( transparencyLayout, 0);

    qtContainer->setLayout( layout );
    qtContainer->setEnabled(false);

    QObject::connect(m_opacitySlider, SIGNAL(valueChanged(int)), this, SLOT(onOpacitySlider(int)));
    QObject::connect(m_colourButton, SIGNAL(clicked()), this, SLOT(onColorButton()));

    this->updating();
}

//------------------------------------------------------------------------------

void OrganMaterialEditor::stopping()
{
    SLM_TRACE_FUNC();

    QObject::disconnect(m_opacitySlider, SIGNAL(valueChanged(int)), this, SLOT(onOpacitySlider(int)));
    QObject::disconnect(m_colourButton, SIGNAL(clicked()), this, SLOT(onColorButton()));

    this->destroy();
}

//------------------------------------------------------------------------------

void OrganMaterialEditor::configuring()
{
    SLM_TRACE_FUNC();
    this->initialize();
}

//------------------------------------------------------------------------------

void OrganMaterialEditor::updating()
{
    this->refreshMaterial();
}

//------------------------------------------------------------------------------

void OrganMaterialEditor::onColorButton()
{
    ::fwData::Reconstruction::sptr reconstruction = this->getInOut< ::fwData::Reconstruction >(s_RECONSTRUCTION_INOUT);
    if (!reconstruction)
    {
        FW_DEPRECATED_KEY(s_RECONSTRUCTION_INOUT, "inout", "18.0");
        reconstruction = this->getObject< ::fwData::Reconstruction >();
    }
    SLM_ASSERT("No Reconstruction!", reconstruction);

    ::fwData::Material::sptr material = reconstruction->getMaterial();
    int red   = material->diffuse()->red()*255;
    int green = material->diffuse()->green()*255;
    int blue  = material->diffuse()->blue()*255;

    // Create Color choice dialog.
    ::fwGuiQt::container::QtContainer::sptr qtContainer = ::fwGuiQt::container::QtContainer::dynamicCast(
        this->getContainer() );
    QWidget* const container = qtContainer->getQtContainer();
    SLM_ASSERT("container not instanced", container);

    QColor oldColor(red, green, blue);
    QColor color = QColorDialog::getColor(oldColor, container);
    if(color.isValid())
    {
        material->diffuse()->red()   = color.redF();
        material->diffuse()->green() = color.greenF();
        material->diffuse()->blue()  = color.blueF();
        this->materialNotification();
        refreshMaterial();
    }

}

//------------------------------------------------------------------------------

void OrganMaterialEditor::onOpacitySlider(int value )
{
    ::fwData::Reconstruction::sptr reconstruction = this->getInOut< ::fwData::Reconstruction >(s_RECONSTRUCTION_INOUT);
    if (!reconstruction)
    {
        FW_DEPRECATED_KEY(s_RECONSTRUCTION_INOUT, "inout", "18.0");
        reconstruction = this->getObject< ::fwData::Reconstruction >();
    }
    SLM_ASSERT("No Reconstruction!", reconstruction);

    ::fwData::Material::sptr material = reconstruction->getMaterial();
    material->diffuse()->alpha()      = value/100.0;
    std::stringstream ss;
    ss << value << "%";
    m_transparencyValue->setText(QString::fromStdString(ss.str()));

    this->materialNotification();
}

//------------------------------------------------------------------------------

void OrganMaterialEditor::refreshMaterial( )
{
    ::fwData::Reconstruction::csptr reconstruction = this->getInOut< ::fwData::Reconstruction >(s_RECONSTRUCTION_INOUT);
    if (!reconstruction)
    {
        FW_DEPRECATED_KEY(s_RECONSTRUCTION_INOUT, "inout", "18.0");
        reconstruction = this->getObject< ::fwData::Reconstruction >();
    }
    SLM_ASSERT("No Reconstruction!", reconstruction);

    ::fwGuiQt::container::QtContainer::sptr qtContainer = ::fwGuiQt::container::QtContainer::dynamicCast(
        this->getContainer() );
    QWidget* const container = qtContainer->getQtContainer();
    SLM_ASSERT("container not instanced", container);

    container->setEnabled(!reconstruction->getOrganName().empty());

    ::fwData::Material::csptr material = reconstruction->getMaterial();
    QColor materialColor = QColor(
        material->diffuse()->red()*255,
        material->diffuse()->green()*255,
        material->diffuse()->blue()*255,
        material->diffuse()->alpha()*255
        );

    int iconSize = m_colourButton->style()->pixelMetric(QStyle::PM_LargeIconSize);
    QPixmap pix(iconSize, iconSize);
    pix.fill(materialColor);

    m_colourButton->setIcon(QIcon(pix));

    int a = material->diffuse()->alpha()*100;
    m_opacitySlider->setValue( a );
    std::stringstream ss;
    ss << a << "%";
    m_transparencyValue->setText(QString::fromStdString(ss.str()));
}

//------------------------------------------------------------------------------

void OrganMaterialEditor::materialNotification( )
{
    ::fwData::Reconstruction::csptr reconstruction = this->getInOut< ::fwData::Reconstruction >(s_RECONSTRUCTION_INOUT);
    if (!reconstruction)
    {
        FW_DEPRECATED_KEY(s_RECONSTRUCTION_INOUT, "inout", "18.0");
        reconstruction = this->getObject< ::fwData::Reconstruction >();
    }
    SLM_ASSERT("No Reconstruction!", reconstruction);

    ::fwData::Object::ModifiedSignalType::sptr sig;
    sig = reconstruction->getMaterial()->signal< ::fwData::Object::ModifiedSignalType >(
        ::fwData::Object::s_MODIFIED_SIG);
    sig->asyncEmit();
}

//------------------------------------------------------------------------------

}

/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "uiCalibration/SOpticalCenterEditor.hpp"

#include <arData/Camera.hpp>

#include <fwData/mt/ObjectWriteLock.hpp>

#include <fwGuiQt/container/QtContainer.hpp>

#include <fwServices/macros.hpp>
#include <QComboBox>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>

namespace uiCalibration
{

fwServicesRegisterMacro( ::gui::editor::IEditor, ::uiCalibration::SOpticalCenterEditor, ::arData::Camera);

//------------------------------------------------------------------------------
SOpticalCenterEditor::SOpticalCenterEditor() throw()
{
}

//------------------------------------------------------------------------------

SOpticalCenterEditor::~SOpticalCenterEditor() throw()
{
}

//------------------------------------------------------------------------------

void SOpticalCenterEditor::configuring() throw(::fwTools::Failed)
{
    this->initialize();
}

//------------------------------------------------------------------------------

void SOpticalCenterEditor::starting() throw(::fwTools::Failed)
{
    this->create();
    ::fwGuiQt::container::QtContainer::sptr qtContainer =
        ::fwGuiQt::container::QtContainer::dynamicCast(getContainer());

    ::arData::Camera::sptr camera = this->getInOut< ::arData::Camera >("camera");
    OSLM_ASSERT("Camera " << this->getID() << " must be calibrated.", camera->getIsCalibrated());

    QWidget* const container = qtContainer->getQtContainer();

    QVBoxLayout* vLayout  = new QVBoxLayout();
    QHBoxLayout* cxLayout = new QHBoxLayout();
    QHBoxLayout* cyLayout = new QHBoxLayout();
    QHBoxLayout* fyLayout = new QHBoxLayout();

    int cx = static_cast<int>(camera->getCx());
    int cy = static_cast<int>(camera->getCy());
    int fy = static_cast<int>(camera->getFy());

    QLabel * cxLabel = new QLabel(tr("CCD X:"));
    m_cxSlider = new QSlider(Qt::Horizontal);
    m_cxLabel  = new QLabel();
    m_cxLabel->setText(QString("%1").arg(cx));

    cxLayout->addWidget(cxLabel);
    cxLayout->addWidget(m_cxSlider);
    cxLayout->addWidget(m_cxLabel);

    QLabel * cyLabel = new QLabel(tr("CCD Y:"));
    m_cySlider = new QSlider(Qt::Horizontal);
    m_cyLabel  = new QLabel();
    m_cyLabel->setText(QString("%1").arg(cy));

    cyLayout->addWidget(cyLabel);
    cyLayout->addWidget(m_cySlider);
    cyLayout->addWidget(m_cyLabel);

    QLabel * fyLabel = new QLabel(tr("Fy:"));
    m_fySlider = new QSlider(Qt::Horizontal);
    m_fyLabel  = new QLabel();
    m_fyLabel->setText(QString("%1").arg(fy));

    fyLayout->addWidget(fyLabel);
    fyLayout->addWidget(m_fySlider);
    fyLayout->addWidget(m_fyLabel);

    int deltaX  = static_cast<int>(camera->getWidth() / 5);
    int deltaY  = static_cast<int>(camera->getHeight() / 5);
    int deltaFY = static_cast<int>(camera->getFy() * .5);

    m_cxSlider->setRange(cx - deltaX,  cx + deltaX);
    m_cySlider->setRange(cy - deltaY,  cy + deltaY);
    m_fySlider->setRange(fy - deltaFY, fy + deltaFY);

    m_cxSlider->setValue(cx);
    m_cySlider->setValue(cy);
    m_fySlider->setValue(fy);

    vLayout->addLayout(cxLayout);
    vLayout->addLayout(cyLayout);
    vLayout->addLayout(fyLayout);

    container->setLayout(vLayout);

    QObject::connect(m_cxSlider, SIGNAL(valueChanged(int)), this, SLOT(onCxSliderChanged(int)));
    QObject::connect(m_cySlider, SIGNAL(valueChanged(int)), this, SLOT(onCySliderChanged(int)));
    QObject::connect(m_fySlider, SIGNAL(valueChanged(int)), this, SLOT(onFySliderChanged(int)));
}

//------------------------------------------------------------------------------

void SOpticalCenterEditor::stopping() throw(::fwTools::Failed)
{
    QObject::disconnect(m_cxSlider, SIGNAL(valueChanged(int)), this, SLOT(onCxSliderChanged(int)));
    QObject::disconnect(m_cySlider, SIGNAL(valueChanged(int)), this, SLOT(onCySliderChanged(int)));
    QObject::disconnect(m_fySlider, SIGNAL(valueChanged(int)), this, SLOT(onFySliderChanged(int)));

    this->getContainer()->clean();
    this->destroy();
}

//------------------------------------------------------------------------------

void SOpticalCenterEditor::updating() throw(::fwTools::Failed)
{
}

//------------------------------------------------------------------------------

void SOpticalCenterEditor::onCxSliderChanged(int value)
{
    ::arData::Camera::sptr camera = this->getObject< ::arData::Camera >();
    ::fwData::mt::ObjectWriteLock lock(camera);

    camera->setCx(value);

    m_cxLabel->setText(QString("%1").arg(value));

    auto sig = camera->signal< ::arData::Camera::IntrinsicCalibratedSignalType >(
        ::arData::Camera::s_INTRINSIC_CALIBRATED_SIG);
    sig->asyncEmit();
}

//------------------------------------------------------------------------------

void SOpticalCenterEditor::onCySliderChanged(int value)
{
    ::arData::Camera::sptr camera = this->getObject< ::arData::Camera >();
    ::fwData::mt::ObjectWriteLock lock(camera);

    camera->setCy(value);

    m_cyLabel->setText(QString("%1").arg(value));

    auto sig = camera->signal< ::arData::Camera::IntrinsicCalibratedSignalType >(
        ::arData::Camera::s_INTRINSIC_CALIBRATED_SIG);
    sig->asyncEmit();
}

//------------------------------------------------------------------------------

void SOpticalCenterEditor::onFySliderChanged(int value)
{
    ::arData::Camera::sptr camera = this->getObject< ::arData::Camera >();
    ::fwData::mt::ObjectWriteLock lock(camera);

    camera->setFy(value);

    m_fyLabel->setText(QString("%1").arg(value));

    auto sig = camera->signal< ::arData::Camera::IntrinsicCalibratedSignalType >(
        ::arData::Camera::s_INTRINSIC_CALIBRATED_SIG);
    sig->asyncEmit();
}

//------------------------------------------------------------------------------

} // namespace uiCalibration

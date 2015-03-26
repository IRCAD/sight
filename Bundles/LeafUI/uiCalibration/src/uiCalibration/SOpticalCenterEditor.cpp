/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "uiCalibration/SOpticalCenterEditor.hpp"

#include <arData/Camera.hpp>

#include <fwData/mt/ObjectWriteLock.hpp>

#include <fwGuiQt/container/QtContainer.hpp>

#include <fwServices/Base.hpp>
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

    ConfigurationType srcCfg = m_configuration->findConfigurationElement("cameraSrcUid");
    if (srcCfg)
    {
        m_cameraSrcUid = srcCfg->getValue();
    }
}

//------------------------------------------------------------------------------

void SOpticalCenterEditor::starting() throw(::fwTools::Failed)
{
    this->create();
    ::fwGuiQt::container::QtContainer::sptr qtContainer =
        ::fwGuiQt::container::QtContainer::dynamicCast(getContainer());

    ::arData::Camera::sptr camera = this->getObject< ::arData::Camera >();
    if (!m_cameraSrcUid.empty())
    {
        ::fwData::mt::ObjectWriteLock lock(camera);
        ::fwTools::Object::sptr obj      = ::fwTools::fwID::getObject(m_cameraSrcUid);
        ::arData::Camera::sptr srcCamera = ::arData::Camera::dynamicCast(obj);
        SLM_ASSERT("Missing source camera '" + m_cameraSrcUid + "'", srcCamera);

        camera->deepCopy(srcCamera);
    }

    SLM_ASSERT("Camera must be calibrated.", camera->getIsCalibrated());

    QWidget* const container = qtContainer->getQtContainer();

    QVBoxLayout* vLayout  = new QVBoxLayout();
    QHBoxLayout* cxLayout = new QHBoxLayout();
    QHBoxLayout* cyLayout = new QHBoxLayout();
    QHBoxLayout* fyLayout = new QHBoxLayout();

    QLabel * cxLabel = new QLabel(tr("CCD X:"));
    m_cxSlider = new QSlider(Qt::Horizontal);
    m_cxLabel  = new QLabel();
    m_cxLabel->setText(QString("%1").arg(camera->getCx()));

    cxLayout->addWidget(cxLabel);
    cxLayout->addWidget(m_cxSlider);
    cxLayout->addWidget(m_cxLabel);


    QLabel * cyLabel = new QLabel(tr("CCD Y:"));
    m_cySlider = new QSlider(Qt::Horizontal);
    m_cyLabel  = new QLabel();
    m_cyLabel->setText(QString("%1").arg(camera->getCy()));

    cyLayout->addWidget(cyLabel);
    cyLayout->addWidget(m_cySlider);
    cyLayout->addWidget(m_cyLabel);

    QLabel * fyLabel = new QLabel(tr("Fy:"));
    m_fySlider = new QSlider(Qt::Horizontal);
    m_fyLabel  = new QLabel();
    m_fyLabel->setText(QString("%1").arg(camera->getFy()));

    fyLayout->addWidget(fyLabel);
    fyLayout->addWidget(m_fySlider);
    fyLayout->addWidget(m_fyLabel);

    int deltaX = camera->getWidth() / 5;
    int deltaY = camera->getHeight() / 5;

    m_cxSlider->setRange(-deltaX, deltaX);
    m_cySlider->setRange(-deltaY, deltaY);
    m_fySlider->setRange(-deltaY, deltaY);

    m_cxSlider->setValue(camera->getWidth() / 2. - camera->getCx());
    m_cySlider->setValue(camera->getHeight() / 2. - camera->getCy());
    m_fySlider->setValue(camera->getHeight() - camera->getFy());

    vLayout->addLayout(cxLayout);
    vLayout->addLayout(cyLayout);
    vLayout->addLayout(fyLayout);

    container->setLayout(vLayout);

    QObject::connect(m_cxSlider, SIGNAL(valueChanged(int )), this, SLOT(onCxSliderChanged(int)));
    QObject::connect(m_cySlider, SIGNAL(valueChanged(int )), this, SLOT(onCySliderChanged(int)));
    QObject::connect(m_fySlider, SIGNAL(valueChanged(int )), this, SLOT(onFySliderChanged(int)));

    QObject::connect(m_cxSlider, SIGNAL(sliderReleased()), this, SLOT(onCxSliderReleased()));
    QObject::connect(m_cySlider, SIGNAL(sliderReleased()), this, SLOT(onCySliderReleased()));
    QObject::connect(m_fySlider, SIGNAL(sliderReleased()), this, SLOT(onFySliderReleased()));
}

//------------------------------------------------------------------------------

void SOpticalCenterEditor::stopping() throw(::fwTools::Failed)
{
    QObject::disconnect(m_cxSlider, SIGNAL(valueChanged(int )), this, SLOT(onCxSliderChanged(int)));
    QObject::disconnect(m_cySlider, SIGNAL(valueChanged(int )), this, SLOT(onCySliderChanged(int)));
    QObject::disconnect(m_fySlider, SIGNAL(valueChanged(int )), this, SLOT(onFySliderChanged(int)));

    QObject::disconnect(m_cxSlider, SIGNAL(sliderReleased()), this, SLOT(onCxSliderReleased()));
    QObject::disconnect(m_cySlider, SIGNAL(sliderReleased()), this, SLOT(onCySliderReleased()));
    QObject::disconnect(m_fySlider, SIGNAL(sliderReleased()), this, SLOT(onFySliderReleased()));

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

    double center = camera->getWidth() / 2. + value;
    m_cxLabel->setText(QString("%1").arg(center));
}

//------------------------------------------------------------------------------

void SOpticalCenterEditor::onCySliderChanged(int value)
{
    ::arData::Camera::sptr camera = this->getObject< ::arData::Camera >();
    ::fwData::mt::ObjectWriteLock lock(camera);

    double center = camera->getHeight() / 2. + value;
    m_cyLabel->setText(QString("%1").arg(center));
}

//------------------------------------------------------------------------------

void SOpticalCenterEditor::onFySliderChanged(int value)
{
    ::arData::Camera::sptr camera = this->getObject< ::arData::Camera >();
    ::fwData::mt::ObjectWriteLock lock(camera);

    double fy = camera->getHeight() + value;
    m_fyLabel->setText(QString("%1").arg(fy));
}

//------------------------------------------------------------------------------

void SOpticalCenterEditor::onCxSliderReleased()
{
    ::arData::Camera::sptr camera = this->getObject< ::arData::Camera >();
    ::fwData::mt::ObjectWriteLock lock(camera);

    double center = camera->getWidth() / 2. + m_cxSlider->value();

    camera->setCx(center);

    m_cxLabel->setText(QString("%1").arg(center));

    ::arData::Camera::IntrinsicCalibratedSignalType::sptr sig;
    sig = camera->signal< ::arData::Camera::IntrinsicCalibratedSignalType >(
        ::arData::Camera::s_INTRINSIC_CALIBRATED_SIG);

    sig->asyncEmit();
}

//------------------------------------------------------------------------------

void SOpticalCenterEditor::onCySliderReleased()
{
    ::arData::Camera::sptr camera = this->getObject< ::arData::Camera >();
    ::fwData::mt::ObjectWriteLock lock(camera);

    double center = camera->getHeight() / 2. + m_cySlider->value();

    camera->setCy(center);

    m_cyLabel->setText(QString("%1").arg(center));

    ::arData::Camera::IntrinsicCalibratedSignalType::sptr sig;
    sig = camera->signal< ::arData::Camera::IntrinsicCalibratedSignalType >(
        ::arData::Camera::s_INTRINSIC_CALIBRATED_SIG);

    sig->asyncEmit();
}

//------------------------------------------------------------------------------

void SOpticalCenterEditor::onFySliderReleased()
{
    ::arData::Camera::sptr camera = this->getObject< ::arData::Camera >();
    ::fwData::mt::ObjectWriteLock lock(camera);

    double fy = camera->getHeight() + m_fySlider->value();

    camera->setFy(fy);

    m_fyLabel->setText(QString("%1").arg(fy));

    ::arData::Camera::IntrinsicCalibratedSignalType::sptr sig;
    sig = camera->signal< ::arData::Camera::IntrinsicCalibratedSignalType >(
        ::arData::Camera::s_INTRINSIC_CALIBRATED_SIG);

    sig->asyncEmit();
}

//------------------------------------------------------------------------------

} // namespace uiCalibration

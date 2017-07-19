/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "uiCalibration/SOpticalCenterEditor.hpp"

#include <arData/Camera.hpp>

#include <fwData/mt/ObjectReadLock.hpp>
#include <fwData/mt/ObjectWriteLock.hpp>
#include <fwData/TransformationMatrix3D.hpp>

#include <fwGuiQt/container/QtContainer.hpp>

#include <fwServices/macros.hpp>

#include <QComboBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QWidget>

namespace uiCalibration
{

static const ::fwServices::IService::KeyType s_CAMERA_INPUT = "camera";
static const ::fwServices::IService::KeyType s_MATRIX_INOUT = "matrix";

fwServicesRegisterMacro( ::gui::editor::IEditor, ::uiCalibration::SOpticalCenterEditor, ::arData::Camera);

//------------------------------------------------------------------------------
SOpticalCenterEditor::SOpticalCenterEditor() noexcept
{
}

//------------------------------------------------------------------------------

SOpticalCenterEditor::~SOpticalCenterEditor() noexcept
{
}

//------------------------------------------------------------------------------

void SOpticalCenterEditor::configuring()
{
    this->initialize();
}

//------------------------------------------------------------------------------

void SOpticalCenterEditor::starting()
{
    this->create();
    ::fwGuiQt::container::QtContainer::sptr qtContainer =
        ::fwGuiQt::container::QtContainer::dynamicCast(getContainer());

    QVBoxLayout* vLayout  = new QVBoxLayout();
    QHBoxLayout* cxLayout = new QHBoxLayout();
    QHBoxLayout* cyLayout = new QHBoxLayout();
    QHBoxLayout* fyLayout = new QHBoxLayout();

    QLabel* cxLabel = new QLabel(tr("CCD X:"));
    m_cxSlider = new QSlider(Qt::Horizontal);
    m_cxLabel  = new QLabel();

    cxLayout->addWidget(cxLabel);
    cxLayout->addWidget(m_cxSlider);
    cxLayout->addWidget(m_cxLabel);

    QLabel* cyLabel = new QLabel(tr("CCD Y:"));
    m_cySlider = new QSlider(Qt::Horizontal);
    m_cyLabel  = new QLabel();

    cyLayout->addWidget(cyLabel);
    cyLayout->addWidget(m_cySlider);
    cyLayout->addWidget(m_cyLabel);

    QLabel* fyLabel = new QLabel(tr("Fy:"));
    m_fySlider = new QSlider(Qt::Horizontal);
    m_fyLabel  = new QLabel();

    fyLayout->addWidget(fyLabel);
    fyLayout->addWidget(m_fySlider);
    fyLayout->addWidget(m_fyLabel);

    vLayout->addLayout(cxLayout);
    vLayout->addLayout(cyLayout);
    vLayout->addLayout(fyLayout);

    qtContainer->setLayout(vLayout);

    QObject::connect(m_cxSlider, SIGNAL(valueChanged(int)), this, SLOT(onCxSliderChanged(int)));
    QObject::connect(m_cySlider, SIGNAL(valueChanged(int)), this, SLOT(onCySliderChanged(int)));
    QObject::connect(m_fySlider, SIGNAL(valueChanged(int)), this, SLOT(onFySliderChanged(int)));

    this->updating();
}

//------------------------------------------------------------------------------

void SOpticalCenterEditor::stopping()
{
    this->destroy();
}

//------------------------------------------------------------------------------

void SOpticalCenterEditor::updating()
{
    ::arData::Camera::csptr camera = this->getInput< ::arData::Camera >(s_CAMERA_INPUT);
    SLM_ASSERT("object '" + s_CAMERA_INPUT + "' is not defined.", camera);
    SLM_ASSERT("Camera " + camera->getID() + " must be calibrated.", camera->getIsCalibrated());

    ::fwData::TransformationMatrix3D::sptr matrix = this->getInOut< ::fwData::TransformationMatrix3D >(s_MATRIX_INOUT);
    SLM_ASSERT("object '" + s_MATRIX_INOUT + "' is not defined.", matrix);

    // Reset matrix if it isn't correctly formatted.
    if(matrix->getCoefficient(3, 3) == 1.)
    {
        auto& coeffs = matrix->getRefCoefficients();
        coeffs.fill(0.);
    }

    const double dCx = matrix->getCoefficient(0, 2);
    const double dCy = matrix->getCoefficient(1, 2);
    const double dFy = matrix->getCoefficient(1, 1);

    const int cx = static_cast<int>(camera->getCx() + dCx);
    const int cy = static_cast<int>(camera->getCy() + dCy);
    const int fy = static_cast<int>(camera->getFy() + dFy);

    const int deltaX  = static_cast<int>(camera->getWidth() / 5);
    const int deltaY  = static_cast<int>(camera->getHeight() / 5);
    const int deltaFY = static_cast<int>(camera->getFy() * .5);

    m_cxSlider->setRange(cx - deltaX,  cx + deltaX);
    m_cySlider->setRange(cy - deltaY,  cy + deltaY);
    m_fySlider->setRange(fy - deltaFY, fy + deltaFY);

    m_cxSlider->setValue(cx);
    m_cySlider->setValue(cy);
    m_fySlider->setValue(fy);

    m_cxLabel->setText(QString("%1").arg(cx));
    m_cyLabel->setText(QString("%1").arg(cy));
    m_fyLabel->setText(QString("%1").arg(fy));
}

//------------------------------------------------------------------------------

fwServices::IService::KeyConnectionsMap SOpticalCenterEditor::getAutoConnections() const
{
    KeyConnectionsMap connections;

    connections.push(s_CAMERA_INPUT, ::arData::Camera::s_INTRINSIC_CALIBRATED_SIG, s_UPDATE_SLOT);
    connections.push(s_CAMERA_INPUT, ::arData::Camera::s_MODIFIED_SIG, s_UPDATE_SLOT);
    connections.push(s_MATRIX_INOUT, ::fwData::TransformationMatrix3D::s_MODIFIED_SIG, s_UPDATE_SLOT);

    return connections;
}

//------------------------------------------------------------------------------

void SOpticalCenterEditor::onCxSliderChanged(int value)
{
    ::arData::Camera::csptr camera = this->getInput< ::arData::Camera >(s_CAMERA_INPUT);
    SLM_ASSERT("object '" + s_CAMERA_INPUT + "' is not defined.", camera);
    ::fwData::TransformationMatrix3D::sptr matrix = this->getInOut< ::fwData::TransformationMatrix3D >(s_MATRIX_INOUT);
    SLM_ASSERT("object '" + s_MATRIX_INOUT + "' is not defined.", matrix);

    ::fwData::mt::ObjectReadLock lockCam(camera);
    ::fwData::mt::ObjectWriteLock lockMat(matrix);

    matrix->setCoefficient(0, 2, value - camera->getCx());

    m_cxLabel->setText(QString("%1").arg(value));

    auto sig = matrix->signal< ::fwData::Object::ModifiedSignalType >(::fwData::Object::s_MODIFIED_SIG);
    {
        ::fwCom::Connection::Blocker block(sig->getConnection(m_slotUpdate));
        sig->asyncEmit();
    }
}

//------------------------------------------------------------------------------

void SOpticalCenterEditor::onCySliderChanged(int value)
{
    ::arData::Camera::csptr camera = this->getInput< ::arData::Camera >(s_CAMERA_INPUT);
    SLM_ASSERT("object '" + s_CAMERA_INPUT + "' is not defined.", camera);
    ::fwData::TransformationMatrix3D::sptr matrix = this->getInOut< ::fwData::TransformationMatrix3D >(s_MATRIX_INOUT);
    SLM_ASSERT("object '" + s_MATRIX_INOUT + "' is not defined.", matrix);

    ::fwData::mt::ObjectReadLock lockCam(camera);
    ::fwData::mt::ObjectWriteLock lockMat(matrix);

    matrix->setCoefficient(1, 2, value - camera->getCy());

    m_cyLabel->setText(QString("%1").arg(value));

    auto sig = matrix->signal< ::fwData::Object::ModifiedSignalType >(::fwData::Object::s_MODIFIED_SIG);
    {
        ::fwCom::Connection::Blocker block(sig->getConnection(m_slotUpdate));
        sig->asyncEmit();
    }
}

//------------------------------------------------------------------------------

void SOpticalCenterEditor::onFySliderChanged(int value)
{
    ::arData::Camera::csptr camera = this->getInput< ::arData::Camera >(s_CAMERA_INPUT);
    SLM_ASSERT("object '" + s_CAMERA_INPUT + "' is not defined.", camera);
    ::fwData::TransformationMatrix3D::sptr matrix = this->getInOut< ::fwData::TransformationMatrix3D >(s_MATRIX_INOUT);
    SLM_ASSERT("object '" + s_MATRIX_INOUT + "' is not defined.", matrix);

    ::fwData::mt::ObjectReadLock lockCam(camera);
    ::fwData::mt::ObjectWriteLock lockMat(matrix);

    matrix->setCoefficient(1, 1, value - camera->getFy());

    m_fyLabel->setText(QString("%1").arg(value));

    auto sig = matrix->signal< ::fwData::Object::ModifiedSignalType >(::fwData::Object::s_MODIFIED_SIG);
    {
        ::fwCom::Connection::Blocker block(sig->getConnection(m_slotUpdate));
        sig->asyncEmit();
    }
}

//------------------------------------------------------------------------------

} // namespace uiCalibration

/************************************************************************
 *
 * Copyright (C) 2014-2021 IRCAD France
 * Copyright (C) 2014-2018 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include "modules/ui/qt/calibration/SOpticalCenterEditor.hpp"

#include <data/Camera.hpp>
#include <data/mt/ObjectReadLock.hpp>
#include <data/mt/ObjectWriteLock.hpp>
#include <data/TransformationMatrix3D.hpp>

#include <service/macros.hpp>

#include <QComboBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QWidget>

#include <ui/qt/container/QtContainer.hpp>

namespace sight::modules::ui::qt::calibration
{

static const service::IService::KeyType s_CAMERA_INPUT = "camera";
static const service::IService::KeyType s_MATRIX_INOUT = "matrix";

fwServicesRegisterMacro( ::sight::ui::base::IEditor,
                         ::sight::modules::ui::qt::calibration::SOpticalCenterEditor,
                         ::sight::data::Camera)

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
    auto qtContainer = sight::ui::qt::container::QtContainer::dynamicCast(getContainer());

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
    data::Camera::csptr camera = this->getInput< data::Camera >(s_CAMERA_INPUT);
    SLM_ASSERT("object '" + s_CAMERA_INPUT + "' is not defined.", camera);
    SLM_ASSERT("Camera " + camera->getID() + " must be calibrated.", camera->getIsCalibrated());

    data::TransformationMatrix3D::sptr matrix = this->getInOut< data::TransformationMatrix3D >(s_MATRIX_INOUT);
    SLM_ASSERT("object '" + s_MATRIX_INOUT + "' is not defined.", matrix);

    // Reset matrix if it isn't correctly formatted.
    if(matrix->getCoefficient(3, 3) == 1.)
    {
        auto& coeffs = matrix->getCoefficients();
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

service::IService::KeyConnectionsMap SOpticalCenterEditor::getAutoConnections() const
{
    KeyConnectionsMap connections;

    connections.push(s_CAMERA_INPUT, data::Camera::s_INTRINSIC_CALIBRATED_SIG, s_UPDATE_SLOT);
    connections.push(s_CAMERA_INPUT, data::Camera::s_MODIFIED_SIG, s_UPDATE_SLOT);
    connections.push(s_MATRIX_INOUT, data::TransformationMatrix3D::s_MODIFIED_SIG, s_UPDATE_SLOT);

    return connections;
}

//------------------------------------------------------------------------------

void SOpticalCenterEditor::onCxSliderChanged(int value)
{
    data::Camera::csptr camera = this->getInput< data::Camera >(s_CAMERA_INPUT);
    SLM_ASSERT("object '" + s_CAMERA_INPUT + "' is not defined.", camera);
    data::TransformationMatrix3D::sptr matrix = this->getInOut< data::TransformationMatrix3D >(s_MATRIX_INOUT);
    SLM_ASSERT("object '" + s_MATRIX_INOUT + "' is not defined.", matrix);

    data::mt::ObjectReadLock lockCam(camera);
    data::mt::ObjectWriteLock lockMat(matrix);

    matrix->setCoefficient(0, 2, value - camera->getCx());

    m_cxLabel->setText(QString("%1").arg(value));

    auto sig = matrix->signal< data::Object::ModifiedSignalType >(data::Object::s_MODIFIED_SIG);
    {
        core::com::Connection::Blocker block(sig->getConnection(m_slotUpdate));
        sig->asyncEmit();
    }
}

//------------------------------------------------------------------------------

void SOpticalCenterEditor::onCySliderChanged(int value)
{
    data::Camera::csptr camera = this->getInput< data::Camera >(s_CAMERA_INPUT);
    SLM_ASSERT("object '" + s_CAMERA_INPUT + "' is not defined.", camera);
    data::TransformationMatrix3D::sptr matrix = this->getInOut< data::TransformationMatrix3D >(s_MATRIX_INOUT);
    SLM_ASSERT("object '" + s_MATRIX_INOUT + "' is not defined.", matrix);

    data::mt::ObjectReadLock lockCam(camera);
    data::mt::ObjectWriteLock lockMat(matrix);

    matrix->setCoefficient(1, 2, value - camera->getCy());

    m_cyLabel->setText(QString("%1").arg(value));

    auto sig = matrix->signal< data::Object::ModifiedSignalType >(data::Object::s_MODIFIED_SIG);
    {
        core::com::Connection::Blocker block(sig->getConnection(m_slotUpdate));
        sig->asyncEmit();
    }
}

//------------------------------------------------------------------------------

void SOpticalCenterEditor::onFySliderChanged(int value)
{
    data::Camera::csptr camera = this->getInput< data::Camera >(s_CAMERA_INPUT);
    SLM_ASSERT("object '" + s_CAMERA_INPUT + "' is not defined.", camera);
    data::TransformationMatrix3D::sptr matrix = this->getInOut< data::TransformationMatrix3D >(s_MATRIX_INOUT);
    SLM_ASSERT("object '" + s_MATRIX_INOUT + "' is not defined.", matrix);

    data::mt::ObjectReadLock lockCam(camera);
    data::mt::ObjectWriteLock lockMat(matrix);

    matrix->setCoefficient(1, 1, value - camera->getFy());

    m_fyLabel->setText(QString("%1").arg(value));

    auto sig = matrix->signal< data::Object::ModifiedSignalType >(data::Object::s_MODIFIED_SIG);
    {
        core::com::Connection::Blocker block(sig->getConnection(m_slotUpdate));
        sig->asyncEmit();
    }
}

//------------------------------------------------------------------------------

} // namespace sight::modules::ui::qt::calibration

/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
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

#include <service/macros.hpp>

#include <ui/qt/container/QtContainer.hpp>

#include <QComboBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QWidget>

namespace sight::module::ui::qt::calibration
{

//------------------------------------------------------------------------------
SOpticalCenterEditor::SOpticalCenterEditor() noexcept =
    default;

//------------------------------------------------------------------------------

SOpticalCenterEditor::~SOpticalCenterEditor() noexcept =
    default;

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

    auto* vLayout  = new QVBoxLayout();
    auto* cxLayout = new QHBoxLayout();
    auto* cyLayout = new QHBoxLayout();
    auto* fyLayout = new QHBoxLayout();

    auto* cxLabel = new QLabel(tr("CCD X:"));
    m_cxSlider = new QSlider(Qt::Horizontal);
    m_cxLabel  = new QLabel();

    cxLayout->addWidget(cxLabel);
    cxLayout->addWidget(m_cxSlider);
    cxLayout->addWidget(m_cxLabel);

    auto* cyLabel = new QLabel(tr("CCD Y:"));
    m_cySlider = new QSlider(Qt::Horizontal);
    m_cyLabel  = new QLabel();

    cyLayout->addWidget(cyLabel);
    cyLayout->addWidget(m_cySlider);
    cyLayout->addWidget(m_cyLabel);

    auto* fyLabel = new QLabel(tr("Fy:"));
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
    const auto camera = m_camera.lock();
    SIGHT_ASSERT("object '" << s_CAMERA << "' is not defined.", camera);
    SIGHT_ASSERT("Camera " + camera->getID() + " must be calibrated.", camera->getIsCalibrated());

    const auto matrix = m_matrix.lock();
    SIGHT_ASSERT("object '" << s_MATRIX << "' is not defined.", matrix);

    // Reset matrix if it isn't correctly formatted.
    if((*matrix)(3, 3) == 1.)
    {
        matrix->fill(0.);
    }

    const double dCx = (*matrix)(0, 2);
    const double dCy = (*matrix)(1, 2);
    const double dFy = (*matrix)(1, 1);

    const int cx = static_cast<int>(camera->getCx() + dCx);
    const int cy = static_cast<int>(camera->getCy() + dCy);
    const int fy = static_cast<int>(camera->getFy() + dFy);

    const int deltaX  = static_cast<int>(camera->getWidth() / 5);
    const int deltaY  = static_cast<int>(camera->getHeight() / 5);
    const int deltaFY = static_cast<int>(camera->getFy() * .5);

    m_cxSlider->setRange(cx - deltaX, cx + deltaX);
    m_cySlider->setRange(cy - deltaY, cy + deltaY);
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

    connections.push(s_CAMERA, data::Camera::s_INTRINSIC_CALIBRATED_SIG, IService::slots::s_UPDATE);
    connections.push(s_CAMERA, data::Camera::s_MODIFIED_SIG, IService::slots::s_UPDATE);
    connections.push(s_MATRIX, data::Matrix4::s_MODIFIED_SIG, IService::slots::s_UPDATE);

    return connections;
}

//------------------------------------------------------------------------------

void SOpticalCenterEditor::onCxSliderChanged(int value)
{
    const auto camera = m_camera.lock();
    SIGHT_ASSERT("object '" << s_CAMERA << "' is not defined.", camera);
    const auto matrix = m_matrix.lock();
    SIGHT_ASSERT("object '" << s_MATRIX << "' is not defined.", matrix);

    (*matrix)(0, 2) = value - camera->getCx();

    m_cxLabel->setText(QString("%1").arg(value));

    auto sig = matrix->signal<data::Object::ModifiedSignalType>(data::Object::s_MODIFIED_SIG);
    {
        core::com::Connection::Blocker block(sig->getConnection(slot(IService::slots::s_UPDATE)));
        sig->asyncEmit();
    }
}

//------------------------------------------------------------------------------

void SOpticalCenterEditor::onCySliderChanged(int value)
{
    const auto camera = m_camera.lock();
    SIGHT_ASSERT("object '" << s_CAMERA << "' is not defined.", camera);
    const auto matrix = m_matrix.lock();
    SIGHT_ASSERT("object '" << s_MATRIX << "' is not defined.", matrix);

    (*matrix)(1, 2) = value - camera->getCy();

    m_cyLabel->setText(QString("%1").arg(value));

    auto sig = matrix->signal<data::Object::ModifiedSignalType>(data::Object::s_MODIFIED_SIG);
    {
        core::com::Connection::Blocker block(sig->getConnection(slot(IService::slots::s_UPDATE)));
        sig->asyncEmit();
    }
}

//------------------------------------------------------------------------------

void SOpticalCenterEditor::onFySliderChanged(int value)
{
    const auto camera = m_camera.lock();
    SIGHT_ASSERT("object '" << s_CAMERA << "' is not defined.", camera);
    const auto matrix = m_matrix.lock();
    SIGHT_ASSERT("object '" << s_MATRIX << "' is not defined.", matrix);

    (*matrix)(1, 1) = value - camera->getFy();

    m_fyLabel->setText(QString("%1").arg(value));

    auto sig = matrix->signal<data::Object::ModifiedSignalType>(data::Object::s_MODIFIED_SIG);
    {
        core::com::Connection::Blocker block(sig->getConnection(slot(IService::slots::s_UPDATE)));
        sig->asyncEmit();
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::qt::calibration

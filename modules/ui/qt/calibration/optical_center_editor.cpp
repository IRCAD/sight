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

#include "modules/ui/qt/calibration/optical_center_editor.hpp"

#include <service/macros.hpp>

#include <ui/qt/container/widget.hpp>

#include <QComboBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QWidget>

namespace sight::module::ui::qt::calibration
{

//------------------------------------------------------------------------------
optical_center_editor::optical_center_editor() noexcept =
    default;

//------------------------------------------------------------------------------

optical_center_editor::~optical_center_editor() noexcept =
    default;

//------------------------------------------------------------------------------

void optical_center_editor::configuring()
{
    this->initialize();
}

//------------------------------------------------------------------------------

void optical_center_editor::starting()
{
    this->create();
    auto qtContainer = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(getContainer());

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

void optical_center_editor::stopping()
{
    this->destroy();
}

//------------------------------------------------------------------------------

void optical_center_editor::updating()
{
    const auto camera = m_camera.lock();
    SIGHT_ASSERT("object '" << s_CAMERA << "' is not defined.", camera);
    SIGHT_ASSERT("Camera " + camera->get_id() + " must be calibrated.", camera->getIsCalibrated());

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

service::connections_t optical_center_editor::auto_connections() const
{
    connections_t connections;

    connections.push(s_CAMERA, data::camera::INTRINSIC_CALIBRATED_SIG, service::slots::UPDATE);
    connections.push(s_CAMERA, data::camera::MODIFIED_SIG, service::slots::UPDATE);
    connections.push(s_MATRIX, data::matrix4::MODIFIED_SIG, service::slots::UPDATE);

    return connections;
}

//------------------------------------------------------------------------------

void optical_center_editor::onCxSliderChanged(int value)
{
    const auto camera = m_camera.lock();
    SIGHT_ASSERT("object '" << s_CAMERA << "' is not defined.", camera);
    const auto matrix = m_matrix.lock();
    SIGHT_ASSERT("object '" << s_MATRIX << "' is not defined.", matrix);

    (*matrix)(0, 2) = value - camera->getCx();

    m_cxLabel->setText(QString("%1").arg(value));

    auto sig = matrix->signal<data::object::ModifiedSignalType>(data::object::MODIFIED_SIG);
    {
        core::com::connection::blocker block(sig->get_connection(slot(service::slots::UPDATE)));
        sig->async_emit();
    }
}

//------------------------------------------------------------------------------

void optical_center_editor::onCySliderChanged(int value)
{
    const auto camera = m_camera.lock();
    SIGHT_ASSERT("object '" << s_CAMERA << "' is not defined.", camera);
    const auto matrix = m_matrix.lock();
    SIGHT_ASSERT("object '" << s_MATRIX << "' is not defined.", matrix);

    (*matrix)(1, 2) = value - camera->getCy();

    m_cyLabel->setText(QString("%1").arg(value));

    auto sig = matrix->signal<data::object::ModifiedSignalType>(data::object::MODIFIED_SIG);
    {
        core::com::connection::blocker block(sig->get_connection(slot(service::slots::UPDATE)));
        sig->async_emit();
    }
}

//------------------------------------------------------------------------------

void optical_center_editor::onFySliderChanged(int value)
{
    const auto camera = m_camera.lock();
    SIGHT_ASSERT("object '" << s_CAMERA << "' is not defined.", camera);
    const auto matrix = m_matrix.lock();
    SIGHT_ASSERT("object '" << s_MATRIX << "' is not defined.", matrix);

    (*matrix)(1, 1) = value - camera->getFy();

    m_fyLabel->setText(QString("%1").arg(value));

    auto sig = matrix->signal<data::object::ModifiedSignalType>(data::object::MODIFIED_SIG);
    {
        core::com::connection::blocker block(sig->get_connection(slot(service::slots::UPDATE)));
        sig->async_emit();
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::qt::calibration

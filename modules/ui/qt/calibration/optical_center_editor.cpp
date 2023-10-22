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
    auto qt_container = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(getContainer());

    auto* v_layout  = new QVBoxLayout();
    auto* cx_layout = new QHBoxLayout();
    auto* cy_layout = new QHBoxLayout();
    auto* fy_layout = new QHBoxLayout();

    auto* cx_label = new QLabel(tr("CCD X:"));
    m_cxSlider = new QSlider(Qt::Horizontal);
    m_cxLabel  = new QLabel();

    cx_layout->addWidget(cx_label);
    cx_layout->addWidget(m_cxSlider);
    cx_layout->addWidget(m_cxLabel);

    auto* cy_label = new QLabel(tr("CCD Y:"));
    m_cySlider = new QSlider(Qt::Horizontal);
    m_cyLabel  = new QLabel();

    cy_layout->addWidget(cy_label);
    cy_layout->addWidget(m_cySlider);
    cy_layout->addWidget(m_cyLabel);

    auto* fy_label = new QLabel(tr("Fy:"));
    m_fySlider = new QSlider(Qt::Horizontal);
    m_fyLabel  = new QLabel();

    fy_layout->addWidget(fy_label);
    fy_layout->addWidget(m_fySlider);
    fy_layout->addWidget(m_fyLabel);

    v_layout->addLayout(cx_layout);
    v_layout->addLayout(cy_layout);
    v_layout->addLayout(fy_layout);

    qt_container->setLayout(v_layout);

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

    const double d_cx = (*matrix)(0, 2);
    const double d_cy = (*matrix)(1, 2);
    const double d_fy = (*matrix)(1, 1);

    const int cx = static_cast<int>(camera->getCx() + d_cx);
    const int cy = static_cast<int>(camera->getCy() + d_cy);
    const int fy = static_cast<int>(camera->getFy() + d_fy);

    const int delta_x  = static_cast<int>(camera->getWidth() / 5);
    const int delta_y  = static_cast<int>(camera->getHeight() / 5);
    const int delta_fy = static_cast<int>(camera->getFy() * .5);

    m_cxSlider->setRange(cx - delta_x, cx + delta_x);
    m_cySlider->setRange(cy - delta_y, cy + delta_y);
    m_fySlider->setRange(fy - delta_fy, fy + delta_fy);

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

void optical_center_editor::onCxSliderChanged(int _value)
{
    const auto camera = m_camera.lock();
    SIGHT_ASSERT("object '" << s_CAMERA << "' is not defined.", camera);
    const auto matrix = m_matrix.lock();
    SIGHT_ASSERT("object '" << s_MATRIX << "' is not defined.", matrix);

    (*matrix)(0, 2) = _value - camera->getCx();

    m_cxLabel->setText(QString("%1").arg(_value));

    auto sig = matrix->signal<data::object::modified_signal_t>(data::object::MODIFIED_SIG);
    {
        core::com::connection::blocker block(sig->get_connection(slot(service::slots::UPDATE)));
        sig->async_emit();
    }
}

//------------------------------------------------------------------------------

void optical_center_editor::onCySliderChanged(int _value)
{
    const auto camera = m_camera.lock();
    SIGHT_ASSERT("object '" << s_CAMERA << "' is not defined.", camera);
    const auto matrix = m_matrix.lock();
    SIGHT_ASSERT("object '" << s_MATRIX << "' is not defined.", matrix);

    (*matrix)(1, 2) = _value - camera->getCy();

    m_cyLabel->setText(QString("%1").arg(_value));

    auto sig = matrix->signal<data::object::modified_signal_t>(data::object::MODIFIED_SIG);
    {
        core::com::connection::blocker block(sig->get_connection(slot(service::slots::UPDATE)));
        sig->async_emit();
    }
}

//------------------------------------------------------------------------------

void optical_center_editor::onFySliderChanged(int _value)
{
    const auto camera = m_camera.lock();
    SIGHT_ASSERT("object '" << s_CAMERA << "' is not defined.", camera);
    const auto matrix = m_matrix.lock();
    SIGHT_ASSERT("object '" << s_MATRIX << "' is not defined.", matrix);

    (*matrix)(1, 1) = _value - camera->getFy();

    m_fyLabel->setText(QString("%1").arg(_value));

    auto sig = matrix->signal<data::object::modified_signal_t>(data::object::MODIFIED_SIG);
    {
        core::com::connection::blocker block(sig->get_connection(slot(service::slots::UPDATE)));
        sig->async_emit();
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::qt::calibration

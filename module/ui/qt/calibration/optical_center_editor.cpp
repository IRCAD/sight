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

#include "module/ui/qt/calibration/optical_center_editor.hpp"

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
    auto qt_container = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(get_container());

    auto* v_layout  = new QVBoxLayout();
    auto* cx_layout = new QHBoxLayout();
    auto* cy_layout = new QHBoxLayout();
    auto* fy_layout = new QHBoxLayout();

    auto* cx_label = new QLabel(tr("CCD X:"));
    m_cx_slider = new QSlider(Qt::Horizontal);
    m_cx_label  = new QLabel();

    cx_layout->addWidget(cx_label);
    cx_layout->addWidget(m_cx_slider);
    cx_layout->addWidget(m_cx_label);

    auto* cy_label = new QLabel(tr("CCD Y:"));
    m_cy_slider = new QSlider(Qt::Horizontal);
    m_cy_label  = new QLabel();

    cy_layout->addWidget(cy_label);
    cy_layout->addWidget(m_cy_slider);
    cy_layout->addWidget(m_cy_label);

    auto* fy_label = new QLabel(tr("Fy:"));
    m_fy_slider = new QSlider(Qt::Horizontal);
    m_fy_label  = new QLabel();

    fy_layout->addWidget(fy_label);
    fy_layout->addWidget(m_fy_slider);
    fy_layout->addWidget(m_fy_label);

    v_layout->addLayout(cx_layout);
    v_layout->addLayout(cy_layout);
    v_layout->addLayout(fy_layout);

    qt_container->set_layout(v_layout);

    QObject::connect(m_cx_slider, SIGNAL(valueChanged(int)), this, SLOT(on_cx_slider_changed(int)));
    QObject::connect(m_cy_slider, SIGNAL(valueChanged(int)), this, SLOT(on_cy_slider_changed(int)));
    QObject::connect(m_fy_slider, SIGNAL(valueChanged(int)), this, SLOT(on_fy_slider_changed(int)));

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
    SIGHT_ASSERT("object '" << CAMERA << "' is not defined.", camera);
    SIGHT_ASSERT("Camera " + camera->get_id() + " must be calibrated.", camera->get_is_calibrated());

    const auto matrix = m_matrix.lock();
    SIGHT_ASSERT("object '" << MATRIX << "' is not defined.", matrix);

    // Reset matrix if it isn't correctly formatted.
    if((*matrix)(3, 3) == 1.)
    {
        matrix->fill(0.);
    }

    const double d_cx = (*matrix)(0, 2);
    const double d_cy = (*matrix)(1, 2);
    const double d_fy = (*matrix)(1, 1);

    const int cx = static_cast<int>(camera->get_cx() + d_cx);
    const int cy = static_cast<int>(camera->get_cy() + d_cy);
    const int fy = static_cast<int>(camera->get_fy() + d_fy);

    const int delta_x  = static_cast<int>(camera->get_width() / 5);
    const int delta_y  = static_cast<int>(camera->get_height() / 5);
    const int delta_fy = static_cast<int>(camera->get_fy() * .5);

    m_cx_slider->setRange(cx - delta_x, cx + delta_x);
    m_cy_slider->setRange(cy - delta_y, cy + delta_y);
    m_fy_slider->setRange(fy - delta_fy, fy + delta_fy);

    m_cx_slider->setValue(cx);
    m_cy_slider->setValue(cy);
    m_fy_slider->setValue(fy);

    m_cx_label->setText(QString("%1").arg(cx));
    m_cy_label->setText(QString("%1").arg(cy));
    m_fy_label->setText(QString("%1").arg(fy));
}

//------------------------------------------------------------------------------

service::connections_t optical_center_editor::auto_connections() const
{
    connections_t connections;

    connections.push(CAMERA, data::camera::INTRINSIC_CALIBRATED_SIG, service::slots::UPDATE);
    connections.push(CAMERA, data::camera::MODIFIED_SIG, service::slots::UPDATE);
    connections.push(MATRIX, data::matrix4::MODIFIED_SIG, service::slots::UPDATE);

    return connections;
}

//------------------------------------------------------------------------------

void optical_center_editor::on_cx_slider_changed(int _value)
{
    const auto camera = m_camera.lock();
    SIGHT_ASSERT("object '" << CAMERA << "' is not defined.", camera);
    const auto matrix = m_matrix.lock();
    SIGHT_ASSERT("object '" << MATRIX << "' is not defined.", matrix);

    (*matrix)(0, 2) = _value - camera->get_cx();

    m_cx_label->setText(QString("%1").arg(_value));

    auto sig = matrix->signal<data::object::modified_signal_t>(data::object::MODIFIED_SIG);
    {
        core::com::connection::blocker block(sig->get_connection(slot(service::slots::UPDATE)));
        sig->async_emit();
    }
}

//------------------------------------------------------------------------------

void optical_center_editor::on_cy_slider_changed(int _value)
{
    const auto camera = m_camera.lock();
    SIGHT_ASSERT("object '" << CAMERA << "' is not defined.", camera);
    const auto matrix = m_matrix.lock();
    SIGHT_ASSERT("object '" << MATRIX << "' is not defined.", matrix);

    (*matrix)(1, 2) = _value - camera->get_cy();

    m_cy_label->setText(QString("%1").arg(_value));

    auto sig = matrix->signal<data::object::modified_signal_t>(data::object::MODIFIED_SIG);
    {
        core::com::connection::blocker block(sig->get_connection(slot(service::slots::UPDATE)));
        sig->async_emit();
    }
}

//------------------------------------------------------------------------------

void optical_center_editor::on_fy_slider_changed(int _value)
{
    const auto camera = m_camera.lock();
    SIGHT_ASSERT("object '" << CAMERA << "' is not defined.", camera);
    const auto matrix = m_matrix.lock();
    SIGHT_ASSERT("object '" << MATRIX << "' is not defined.", matrix);

    (*matrix)(1, 1) = _value - camera->get_fy();

    m_fy_label->setText(QString("%1").arg(_value));

    auto sig = matrix->signal<data::object::modified_signal_t>(data::object::MODIFIED_SIG);
    {
        core::com::connection::blocker block(sig->get_connection(slot(service::slots::UPDATE)));
        sig->async_emit();
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::qt::calibration

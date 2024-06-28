/************************************************************************
 *
 * Copyright (C) 2014-2024 IRCAD France
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

#include "modules/ui/qt/calibration/camera_information_editor.hpp"

#include <core/com/slots.hxx>

#include <service/macros.hpp>

#include <ui/qt/container/widget.hpp>

#include <QBoxLayout>
#include <QGridLayout>

#include <sstream>

namespace sight::module::ui::qt::calibration
{

// -------------------------------------------------------------------------

const core::com::slots::key_t camera_information_editor::UPDATE_INFOS_SLOT = "updateInfos";

// -------------------------------------------------------------------------

camera_information_editor::camera_information_editor() noexcept
{
    new_slot(UPDATE_INFOS_SLOT, &camera_information_editor::update_informations, this);
}

// -------------------------------------------------------------------------

void camera_information_editor::configuring()
{
    sight::ui::service::initialize();
}

// -------------------------------------------------------------------------

void camera_information_editor::starting()
{
    const QString service_id = QString::fromStdString(base_id());

    sight::ui::service::create();
    auto qt_container = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(get_container());
    qt_container->get_qt_container()->setObjectName(service_id);

    auto* main_layout = new QBoxLayout(QBoxLayout::TopToBottom);
    main_layout->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    auto* grid_layout = new QGridLayout();
    auto* desc        = new QLabel("description: ");
    m_description = new QLabel();
    m_description->setObjectName(service_id + "/description");
    grid_layout->addWidget(desc, 0, 0);
    grid_layout->addWidget(m_description, 0, 1);

    auto* title_layout = new QBoxLayout(QBoxLayout::LeftToRight);
    m_is_calibrated = new QLabel();
    m_is_calibrated->setObjectName(service_id + "/isCalibrated");
    title_layout->addWidget(m_is_calibrated);

    auto* info_layout = new QGridLayout;
    info_layout->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    main_layout->addLayout(grid_layout);
    main_layout->addLayout(title_layout);
    main_layout->addLayout(info_layout);

    m_width = new QLabel();
    m_width->setObjectName(service_id + "/width");
    m_height = new QLabel();
    m_height->setObjectName(service_id + "/height");
    m_cx = new QLabel();
    m_cx->setObjectName(service_id + "/cx");
    m_cy = new QLabel();
    m_cy->setObjectName(service_id + "/cy");
    m_fx = new QLabel();
    m_fx->setObjectName(service_id + "/fx");
    m_fy = new QLabel();
    m_fy->setObjectName(service_id + "/fy");
    m_k1 = new QLabel();
    m_k1->setObjectName(service_id + "/k1");
    m_k2 = new QLabel();
    m_k2->setObjectName(service_id + "/k2");
    m_p1 = new QLabel();
    m_p1->setObjectName(service_id + "/p1");
    m_p2 = new QLabel();
    m_p2->setObjectName(service_id + "/p2");
    m_k3 = new QLabel();
    m_k3->setObjectName(service_id + "/k3");
    m_skew = new QLabel();
    m_skew->setObjectName(service_id + "/skew");

    info_layout->addWidget(m_width, 0, 0);
    info_layout->addWidget(m_height, 0, 1);
    info_layout->addWidget(m_skew, 0, 2);

    info_layout->addWidget(m_cx, 1, 0);
    info_layout->addWidget(m_cy, 1, 1);
    info_layout->addWidget(m_fx, 1, 2);
    info_layout->addWidget(m_fy, 1, 3);

    info_layout->addWidget(m_k1, 2, 0);
    info_layout->addWidget(m_k2, 2, 1);
    info_layout->addWidget(m_p1, 2, 2);
    info_layout->addWidget(m_p2, 2, 3);
    info_layout->addWidget(m_k3, 2, 4);

    qt_container->set_layout(main_layout);

    update_informations();
}

// -------------------------------------------------------------------------

void camera_information_editor::stopping()
{
    this->destroy();
}

// -------------------------------------------------------------------------

void camera_information_editor::update_informations()
{
    const auto camera = m_camera.lock();
    std::stringstream out;

    m_description->setText(QString::fromStdString(camera->get_description()));

    //IS CALIBRATED
    if(camera->get_is_calibrated())
    {
        m_is_calibrated->setText("<b>The camera is calibrated.</b>");
    }
    else
    {
        m_is_calibrated->setText("<b>The camera is not calibrated.</b>");
        this->clear_labels();
        return;
    }

    //Height
    out << "Height: <font color='#0066CC'>" << camera->get_height() << "</font>";
    m_height->setText(out.str().c_str());

    out.str("");

    //Width
    out << "Width: <font color='#0066CC'>" << camera->get_width() << "</font>";
    m_width->setText(out.str().c_str());

    out.str("");
    //CX
    out << "Cx: <font color='#0066CC'>" << camera->get_cx() << "</font>";
    m_cx->setText(out.str().c_str());

    out.str("");

    //CY
    out << "Cy: <font color='#0066CC'>" << camera->get_cy() << "</font>";
    m_cy->setText(out.str().c_str());

    out.str("");

    //FX
    out << "Fx: <font color='#0066CC'>" << camera->get_fx() << "</font>";
    m_fx->setText(out.str().c_str());

    out.str("");

    //FY
    out << "Fy: <font color='#0066CC'>" << camera->get_fy() << "</font>";
    m_fy->setText(out.str().c_str());

    const data::camera::dist_array_t& dist = camera->get_distortion_coefficient();

    out.str("");

    //K1
    out << "K1: <font color='#0066CC'>" << dist[0] << "</font>";
    m_k1->setText(out.str().c_str());

    out.str("");

    //K2
    out << "K2: <font color='#0066CC'>" << dist[1] << "</font>";
    m_k2->setText(out.str().c_str());

    out.str("");

    //P1
    out << "P1: <font color='#0066CC'>" << dist[2] << "</font>";
    m_p1->setText(out.str().c_str());

    out.str("");

    //P2
    out << "P2: <font color='#0066CC'>" << dist[3] << "</font>";
    m_p2->setText(out.str().c_str());

    out.str("");

    //K3
    out << "K3: <font color='#0066CC'>" << dist[4] << "</font>";
    m_k3->setText(out.str().c_str());

    out.str("");

    //SKEW
    out << "Skew: <font color='#0066CC'>" << camera->get_skew() << "</font>";
    m_skew->setText(out.str().c_str());
}

// -------------------------------------------------------------------------

void camera_information_editor::clear_labels()
{
    m_width->setText("");
    m_height->setText("");
    m_cx->setText("");
    m_cy->setText("");
    m_fx->setText("");
    m_fy->setText("");
    m_k1->setText("");
    m_k2->setText("");
    m_p1->setText("");
    m_p2->setText("");
    m_k3->setText("");
    m_skew->setText("");
}

// ----------------------------------------------------------------------------

service::connections_t camera_information_editor::auto_connections() const
{
    connections_t connections;

    connections.push(CAMERA, data::camera::ID_MODIFIED_SIG, UPDATE_INFOS_SLOT);
    connections.push(CAMERA, data::camera::INTRINSIC_CALIBRATED_SIG, UPDATE_INFOS_SLOT);

    return connections;
}

} // namespace sight::module::ui::qt::calibration

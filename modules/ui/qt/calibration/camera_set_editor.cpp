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

#include "modules/ui/qt/calibration/camera_set_editor.hpp"

#include <core/base.hpp>
#include <core/com/slots.hpp>
#include <core/com/slots.hxx>
#include <core/object.hpp>
#include <core/thread/worker.hpp>

#include <data/matrix4.hpp>

#include <service/macros.hpp>

#include <ui/qt/container/widget.hpp>

#include <QBoxLayout>
#include <QGridLayout>

#include <algorithm>
#include <sstream>

namespace sight::module::ui::qt::calibration
{

const core::com::slots::key_t camera_set_editor::UPDATE_INFOS_SLOT = "updateInfos";
// -------------------------------------------------------------------------

camera_set_editor::camera_set_editor() noexcept :
    m_cam_index(1)
{
    new_slot(UPDATE_INFOS_SLOT, &camera_set_editor::update_informations, this);
}

// -------------------------------------------------------------------------

void camera_set_editor::configuring()
{
    sight::ui::service::initialize();

    service::config_t config = this->get_config();
    m_cam_index = config.get<std::size_t>("index", 1);
}

// -------------------------------------------------------------------------

void camera_set_editor::starting()
{
    sight::ui::service::create();
    auto qt_container = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(get_container());

    auto* main_layout = new QBoxLayout(QBoxLayout::TopToBottom);
    main_layout->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    m_description = new QLabel("");

    main_layout->addWidget(m_description);

    auto* grid_layout = new QGridLayout();

    for(std::uint8_t i = 0 ; i < 4 ; ++i)
    {
        for(std::uint8_t j = 0 ; j < 4 ; ++j)
        {
            auto* label = new QLabel("");
            m_matrix_labels.push_back(label);
            grid_layout->addWidget(label, i, j);
        }
    }

    main_layout->addLayout(grid_layout);

    qt_container->set_layout(main_layout);

    this->update_informations();
}

// -------------------------------------------------------------------------

void camera_set_editor::stopping()
{
    this->destroy();
}

// -------------------------------------------------------------------------

void camera_set_editor::update_informations()
{
    const auto camera_set = m_camera_set.lock();

    //IS CALIBRATED
    data::matrix4::csptr matrix = camera_set->get_extrinsic_matrix(m_cam_index);
    if(matrix)
    {
        m_description->setText("<b>The cameras are calibrated.</b>");
    }
    else
    {
        m_description->setText("<b>The cameras are not calibrated.</b>");
        this->clear_labels();
        return;
    }

    for(std::uint8_t i = 0 ; i < 4 ; ++i)
    {
        for(std::uint8_t j = 0 ; j < 4 ; ++j)
        {
            m_matrix_labels[i * 4 + j]->setText(QString("%1").arg((*matrix)(i, j)));
        }
    }
}

// -------------------------------------------------------------------------

void camera_set_editor::clear_labels()
{
    for(int i = 0 ; i < 4 ; ++i)
    {
        for(int j = 0 ; j < 4 ; ++j)
        {
            m_matrix_labels[i * 4 + j]->setText(QString(""));
        }
    }
}

// ----------------------------------------------------------------------------

service::connections_t camera_set_editor::auto_connections() const
{
    service::connections_t connections;
    connections.push(CAMERASET, data::camera_set::ADDED_CAMERA_SIG, UPDATE_INFOS_SLOT);
    connections.push(CAMERASET, data::camera_set::EXTRINSIC_CALIBRATED_SIG, UPDATE_INFOS_SLOT);
    connections.push(CAMERASET, data::camera_set::REMOVED_CAMERA_SIG, UPDATE_INFOS_SLOT);
    return connections;
}

} // namespace sight::module::ui::qt::calibration

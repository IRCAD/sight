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

#include "modules/ui/qt/calibration/camera_set_editor.hpp"

#include <core/base.hpp>
#include <core/com/slots.hpp>
#include <core/com/slots.hxx>
#include <core/thread/worker.hpp>
#include <core/tools/object.hpp>

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
    m_camIndex(1)
{
    new_slot(UPDATE_INFOS_SLOT, &camera_set_editor::updateInformations, this);
}

// -------------------------------------------------------------------------

void camera_set_editor::configuring()
{
    sight::ui::service::initialize();

    service::config_t config = this->get_config();
    m_camIndex = config.get<std::size_t>("index", 1);
}

// -------------------------------------------------------------------------

void camera_set_editor::starting()
{
    sight::ui::service::create();
    auto qtContainer = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(getContainer());

    auto* mainLayout = new QBoxLayout(QBoxLayout::TopToBottom);
    mainLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    m_description = new QLabel("");

    mainLayout->addWidget(m_description);

    auto* gridLayout = new QGridLayout();

    for(std::uint8_t i = 0 ; i < 4 ; ++i)
    {
        for(std::uint8_t j = 0 ; j < 4 ; ++j)
        {
            auto* label = new QLabel("");
            m_matrixLabels.push_back(label);
            gridLayout->addWidget(label, i, j);
        }
    }

    mainLayout->addLayout(gridLayout);

    qtContainer->setLayout(mainLayout);

    this->updateInformations();
}

// -------------------------------------------------------------------------

void camera_set_editor::stopping()
{
    this->destroy();
}

// -------------------------------------------------------------------------

void camera_set_editor::updateInformations()
{
    const auto camera_set = m_camera_set.lock();

    //IS CALIBRATED
    data::matrix4::csptr matrix = camera_set->get_extrinsic_matrix(m_camIndex);
    if(matrix)
    {
        m_description->setText("<b>The cameras are calibrated.</b>");
    }
    else
    {
        m_description->setText("<b>The cameras are not calibrated.</b>");
        this->clearLabels();
        return;
    }

    for(std::uint8_t i = 0 ; i < 4 ; ++i)
    {
        for(std::uint8_t j = 0 ; j < 4 ; ++j)
        {
            m_matrixLabels[i * 4 + j]->setText(QString("%1").arg((*matrix)(i, j)));
        }
    }
}

// -------------------------------------------------------------------------

void camera_set_editor::clearLabels()
{
    for(int i = 0 ; i < 4 ; ++i)
    {
        for(int j = 0 ; j < 4 ; ++j)
        {
            m_matrixLabels[i * 4 + j]->setText(QString(""));
        }
    }
}

// ----------------------------------------------------------------------------

service::connections_t camera_set_editor::auto_connections() const
{
    service::connections_t connections;
    connections.push(s_CAMERASET, data::camera_set::ADDED_CAMERA_SIG, UPDATE_INFOS_SLOT);
    connections.push(s_CAMERASET, data::camera_set::EXTRINSIC_CALIBRATED_SIG, UPDATE_INFOS_SLOT);
    connections.push(s_CAMERASET, data::camera_set::REMOVED_CAMERA_SIG, UPDATE_INFOS_SLOT);
    return connections;
}

} // namespace sight::module::ui::qt::calibration

/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "modules/ui/qt/metrics/distance.hpp"

#include <core/com/signal.hxx>
#include <core/runtime/path.hpp>

#include <data/helper/medical_image.hpp>

#include <ui/qt/container/widget.hpp>

#include <QIcon>
#include <QVBoxLayout>

namespace sight::module::ui::qt::metrics
{

//------------------------------------------------------------------------------

const core::com::signals::key_t distance::DISTANCE_REQUESTED_SIG = "distanceRequested";

//------------------------------------------------------------------------------

distance::distance() noexcept
{
    m_sig_distance_requested = new_signal<distance_requested_signal_t>(DISTANCE_REQUESTED_SIG);
}

//------------------------------------------------------------------------------

distance::~distance() noexcept =
    default;

//------------------------------------------------------------------------------

void distance::starting()
{
    this->sight::ui::service::create();

    auto qt_container = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(
        this->get_container()
    );

    namespace fs = std::filesystem;
    fs::path path_image_dist = core::runtime::get_module_resource_file_path("sight::module::ui::qt", "distance.png");
    SIGHT_ASSERT("image " << path_image_dist << "is missing", fs::exists(path_image_dist));

    QIcon image_dist(QString::fromStdString(path_image_dist.string()));

    m_dist_button = new QPushButton(image_dist, tr(""));
    m_dist_button->setToolTip(tr("distance"));

    auto* layout = new QVBoxLayout();
    layout->addWidget(m_dist_button, 1);
    layout->setContentsMargins(0, 0, 0, 0);
    QObject::connect(m_dist_button, &QPushButton::clicked, this, &self_t::on_distance_button);

    qt_container->set_layout(layout);
}

//------------------------------------------------------------------------------

void distance::stopping()
{
    QObject::disconnect(m_dist_button, &QPushButton::clicked, this, &self_t::on_distance_button);

    this->destroy();
}

//------------------------------------------------------------------------------

void distance::configuring()
{
    this->sight::ui::service::initialize();
}

//------------------------------------------------------------------------------

void distance::updating()
{
}

//------------------------------------------------------------------------------

void distance::on_distance_button()
{
    const auto image = m_image.lock();
    SIGHT_ASSERT("'image' key is not found.", image);

    // force distance to be shown
    data::helper::medical_image::set_distance_visibility(*image, true);
    m_sig_distance_requested->async_emit();
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::qt::metrics

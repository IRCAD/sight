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

#include "modules/ui/qt/metrics/Distance.hpp"

#include <core/com/signal.hxx>
#include <core/runtime/path.hpp>

#include <data/helper/MedicalImage.hpp>

#include <ui/qt/container/widget.hpp>

#include <QIcon>
#include <QVBoxLayout>

namespace sight::module::ui::qt::metrics
{

//------------------------------------------------------------------------------

const core::com::signals::key_t Distance::DISTANCE_REQUESTED_SIG = "distanceRequested";

//------------------------------------------------------------------------------

Distance::Distance() noexcept
{
    m_sigDistanceRequested = new_signal<DistanceRequestedSignalType>(DISTANCE_REQUESTED_SIG);
}

//------------------------------------------------------------------------------

Distance::~Distance() noexcept =
    default;

//------------------------------------------------------------------------------

void Distance::starting()
{
    this->sight::ui::service::create();

    auto qtContainer = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(
        this->getContainer()
    );

    namespace fs = std::filesystem;
    fs::path pathImageDist = core::runtime::get_module_resource_file_path("sight::module::ui::qt", "distance.png");
    SIGHT_ASSERT("Image " << pathImageDist << "is missing", fs::exists(pathImageDist));

    QIcon imageDist(QString::fromStdString(pathImageDist.string()));

    m_distButton = new QPushButton(imageDist, tr(""));
    m_distButton->setToolTip(tr("Distance"));

    auto* layout = new QVBoxLayout();
    layout->addWidget(m_distButton, 1);
    layout->setContentsMargins(0, 0, 0, 0);
    QObject::connect(m_distButton, SIGNAL(clicked()), this, SLOT(onDistanceButton()));

    qtContainer->setLayout(layout);
}

//------------------------------------------------------------------------------

void Distance::stopping()
{
    QObject::disconnect(m_distButton, SIGNAL(clicked()), this, SLOT(onDistanceButton()));

    this->destroy();
}

//------------------------------------------------------------------------------

void Distance::configuring()
{
    this->sight::ui::service::initialize();
}

//------------------------------------------------------------------------------

void Distance::updating()
{
}

//------------------------------------------------------------------------------

void Distance::onDistanceButton()
{
    const auto image = m_image.lock();
    SIGHT_ASSERT("'image' key is not found.", image);

    // force distance to be shown
    data::helper::MedicalImage::setDistanceVisibility(*image, true);
    m_sigDistanceRequested->async_emit();
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::qt::metrics

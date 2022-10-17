/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include <core/base.hpp>
#include <core/com/Signal.hpp>
#include <core/com/Signal.hxx>
#include <core/runtime/ConfigurationElement.hpp>
#include <core/runtime/operations.hpp>

#include <data/Boolean.hpp>
#include <data/Composite.hpp>
#include <data/helper/MedicalImage.hpp>
#include <data/String.hpp>

#include <service/IService.hpp>
#include <service/macros.hpp>

#include <ui/qt/container/QtContainer.hpp>

#include <QIcon>
#include <QVBoxLayout>

#include <filesystem>

namespace sight::module::ui::qt::metrics
{

//------------------------------------------------------------------------------

const core::com::Signals::SignalKeyType Distance::s_DISTANCE_REQUESTED_SIG = "distanceRequested";

//------------------------------------------------------------------------------

Distance::Distance() noexcept
{
    m_sigDistanceRequested = newSignal<DistanceRequestedSignalType>(s_DISTANCE_REQUESTED_SIG);
}

//------------------------------------------------------------------------------

Distance::~Distance() noexcept =
    default;

//------------------------------------------------------------------------------

void Distance::starting()
{
    this->sight::ui::base::IGuiContainer::create();

    auto qtContainer = sight::ui::qt::container::QtContainer::dynamicCast(
        this->getContainer()
    );

    namespace fs = std::filesystem;
    fs::path pathImageDist = core::runtime::getModuleResourceFilePath("sight::module::ui::qt", "distance.png");
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
    this->sight::ui::base::IGuiContainer::initialize();
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
    m_sigDistanceRequested->asyncEmit();
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::qt::metrics

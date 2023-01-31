/************************************************************************
 *
 * Copyright (C) 2017-2023 IRCAD France
 * Copyright (C) 2017-2020 IHU Strasbourg
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

#include "SView.hpp"

#include <activity/extension/Activity.hpp>

#include <core/com/Signal.hxx>
#include <core/com/Slots.hxx>
#include <core/runtime/path.hpp>
#include <core/runtime/runtime.hpp>

#include <data/Composite.hpp>

#include <service/macros.hpp>

#include <ui/base/dialog/MessageDialog.hpp>

namespace sight::module::ui::qml::activity
{

static const core::com::Signals::SignalKeyType s_ACTIVITY_LAUNCHED_SIG = "activityLaunched";

static const core::com::Slots::SlotKeyType s_LAUNCH_ACTIVITY_SLOT = "launchActivity";

//------------------------------------------------------------------------------

SView::SView() :
    m_sigActivityLaunched(newSignal<ActivityLaunchedSignalType>(s_ACTIVITY_LAUNCHED_SIG))
{
    newSlot(s_LAUNCH_ACTIVITY_SLOT, &SView::launchActivity, this);
}

//------------------------------------------------------------------------------

void SView::configuring()
{
    const ConfigType config = this->getConfiguration();

    sight::activity::IActivityLauncher::InOutMapType inoutMap;
    std::for_each(
        m_data.begin(),
        m_data.end(),
        [&inoutMap](const auto& p)
        {
            const auto obj = p.second->lock();
            if(obj != nullptr)
            {
                inoutMap.push_back(obj->getID());
            }
        });
    this->parseConfiguration(config, inoutMap);
}

//------------------------------------------------------------------------------

void SView::starting()
{
}

//------------------------------------------------------------------------------

void SView::stopping()
{
}

//------------------------------------------------------------------------------

void SView::updating()
{
}

//------------------------------------------------------------------------------

void SView::notifyActivityCreation()
{
    m_sigActivityLaunched->asyncEmit();
}

//------------------------------------------------------------------------------

void SView::launchActivity(data::Activity::sptr activity)
{
    bool isValid = false;
    std::string message;

    // check if the activity can be launched
    std::tie(isValid, message) = sight::module::ui::qml::activity::SView::validateActivity(activity);

    if(isValid)
    {
        auto [info, replacementMap] = sight::activity::extension::Activity::getDefault()->getInfoAndReplacementMap(
            *activity,
            m_parameters
        );

        core::runtime::startModule(info.bundleId);

        // get Activity path, it allows to retrieve the associated Qml file
        const auto path = core::runtime::getModuleResourceFilePath(info.bundleId, info.appConfig.id + ".qml");

        // convert the replaceMap to Qt Map to send it to Qml(only QVariantMap type is implemented in Qml)
        QVariantMap variantMap;

        for(const auto& replacement : replacementMap)
        {
            variantMap.insert(
                QString::fromStdString(replacement.first),
                QString::fromStdString(replacement.second)
            );
        }

        Q_EMIT launchRequested(QUrl::fromLocalFile(QString::fromStdString(path.string())), variantMap);
    }
    else
    {
        sight::ui::base::dialog::MessageDialog::show(
            "Activity launch",
            message,
            sight::ui::base::dialog::IMessageDialog::CRITICAL
        );
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::qml::activity

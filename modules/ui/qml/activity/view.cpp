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

#include "view.hpp"

#include <activity/extension/activity.hpp>

#include <core/com/signal.hxx>
#include <core/com/slots.hxx>
#include <core/runtime/path.hpp>
#include <core/runtime/runtime.hpp>

#include <data/composite.hpp>

#include <service/macros.hpp>

#include <ui/__/dialog/message.hpp>

namespace sight::module::ui::qml::activity
{

static const core::com::signals::key_t ACTIVITY_LAUNCHED_SIG = "activity_launched";

static const core::com::slots::key_t LAUNCH_ACTIVITY_SLOT = "launch_activity";

//------------------------------------------------------------------------------

view::view() :
    m_sig_activity_launched(new_signal<activity_launched_signal_t>(ACTIVITY_LAUNCHED_SIG))
{
    new_slot(LAUNCH_ACTIVITY_SLOT, &view::launch_activity, this);
}

//------------------------------------------------------------------------------

void view::configuring()
{
    const config_t config = this->get_config();

    sight::activity::launcher::in_out_map_t inout_map;
    std::for_each(
        m_data.begin(),
        m_data.end(),
        [&inout_map](const auto& _p)
        {
            const auto obj = _p.second->lock();
            if(obj != nullptr)
            {
                inout_map.push_back(obj->get_id());
            }
        });
    this->parse_configuration(config, inout_map);
}

//------------------------------------------------------------------------------

void view::starting()
{
}

//------------------------------------------------------------------------------

void view::stopping()
{
}

//------------------------------------------------------------------------------

void view::updating()
{
}

//------------------------------------------------------------------------------

void view::notify_activity_creation()
{
    m_sig_activity_launched->async_emit();
}

//------------------------------------------------------------------------------

void view::launch_activity(data::activity::sptr _activity)
{
    bool is_valid = false;
    std::string message;

    // check if the activity can be launched
    std::tie(is_valid, message) = sight::module::ui::qml::activity::view::validate_activity(_activity);

    if(is_valid)
    {
        auto [info, replacementMap] = sight::activity::extension::activity::get_default()->get_info_and_replacement_map(
            *_activity,
            m_parameters
        );

        core::runtime::start_module(info.bundle_id);

        // get Activity path, it allows to retrieve the associated Qml file
        const auto path = core::runtime::get_module_resource_file_path(info.bundle_id, info.app_config.id + ".qml");

        // convert the replaceMap to Qt Map to send it to Qml(only QVariantMap type is implemented in Qml)
        QVariantMap variant_map;

        for(const auto& replacement : replacementMap)
        {
            variant_map.insert(
                QString::fromStdString(replacement.first),
                QString::fromStdString(replacement.second)
            );
        }

        Q_EMIT launch_requested(QUrl::fromLocalFile(QString::fromStdString(path.string())), variant_map);
    }
    else
    {
        sight::ui::dialog::message::show("Activity launch", message, sight::ui::dialog::message::critical);
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::qml::activity

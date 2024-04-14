/************************************************************************
 *
 * Copyright (C) 2017-2024 IRCAD France
 * Copyright (C) 2017-2019 IHU Strasbourg
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

#pragma once

#include <activity/launcher.hpp>

#include <data/activity.hpp>

#include <ui/qml/editor.hpp>

#include <QUrl>
#include <QVariantMap>

namespace sight::module::ui::qml::activity
{

/**
 * @brief   This editor displays activities in a single view (when a new activity is launched, it replaces the previous
 * one).
 *
 * This service should receive signals containing Activity connected to the slot \b launch_activity.
 *
 * @section Signals Signals
 * - \b activity_launched(): signal emitted when the activity is launched
 *
 * @section Slots Slots
 * - \b launch_activity( data::activity::sptr ): This slot allows to create a view for the given activity.
 *
 * @section config Configuration
 *
 * This service should be instantiated in a Qml file, but it should also be registered in C++ AppManager.
 * You may need to configure it to add extra information in the replace map (map to define the string that will be
 * replaced in the launched activities, for example channels, object ids, ...
 *
 * @subsection Qml Qml Configuration
 * @code{.qml}
 *  view {
        id: activityView

        onLaunchRequested: {
            activityStackView.clear(StackView.Immediate)
            activityStackView.push(Qt.createComponent(path), {"replaceMap": replace}, StackView.Immediate)
            notifyActivityCreation()
        }
    }

    StackView {
        id: activityStackView
        anchors.fill: parent
    }
    @endcode
 *
 * @subsection Cpp C++ Configuration
 * @code{.cpp}
    service::config_t parameterViewConfig;
    parameterViewConfig.add("<xmlattr>.replace", "nameToReplace");
    parameterViewConfig.add("<xmlattr>.by", this->getInputID("value"));
    m_activityViewConfig.add_child("parameters.parameter", parameterViewConfig);
   @endcode
 * - \b parameters (optional) : additional parameters used to launch the activities
 *    - \b parameter: defines a parameter
 *        - \b replace: name of the parameter as defined in the config
 *        - \b by: defines the string that will replace the parameter name.
 *
 * To simplify the use of view and sequencer, a Qml object ActivityLauncher can be used
 *  @code{.qml}
    ActivityLauncher {
        id: activityLauncher
        activityIdsList: ["ExImageReading", "ExMesher", "ExImageDisplaying"]
        activityNameList: ["Read", "Mesher", "Display"]
    }
   @endcode
 * - \n activityIdsList: identifiers of the activities to launch
 * - \n activityNameList: name of the activities to launch, that will be displays in the stepper
 */
class view : public sight::ui::qml::editor,
             public sight::activity::launcher
{
Q_OBJECT

public:

    SIGHT_DECLARE_SERVICE(view, sight::ui::qml::editor);

    /// Constructor. Do nothing.
    view();

    /// Destructor. Do nothing.

    ~view() override = default;

    /// Signal emited when the activity is launched
    using activity_launched_signal_t = core::com::signal<void ()>;

Q_SIGNALS:

    void launch_requested(QUrl _path, QVariantMap _replace);

public Q_SLOTS:

    /// Emit 'activityLaunched' signal. Should be called by Qml when the activity's Qml file is pushed in the stack view
    void notify_activity_creation();

protected:

    /// Parses the configuration (set the parameters to replace in the activities to launch
    void configuring() override;

    /// Do nothing
    void starting() override;

    /// Do nothing
    void stopping() override;

    /// Do nothing
    void updating() override;

private:

    /// Slot: Launch the given activity in the stackView.
    void launch_activity(data::activity::sptr _activity);

    activity_launched_signal_t::sptr m_sig_activity_launched;

    /// Input data to pass to the configuration
    data::ptr_vector<data::object, data::access::inout> m_data {this, "data"};
};

} // namespace sight::module::ui::qml::activity

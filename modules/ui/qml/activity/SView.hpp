/************************************************************************
 *
 * Copyright (C) 2017-2021 IRCAD France
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

#include "modules/ui/qml/config.hpp"

#include <activity/IActivityLauncher.hpp>

#include <data/ActivitySeries.hpp>

#include <ui/qml/IQmlEditor.hpp>

#include <QUrl>
#include <QVariantMap>

namespace sight::module::ui::qml::activity
{

/**
 * @brief   This editor displays activities in a single view (when a new activity is launched, it replaces the previous
 * one).
 *
 * This service should receive signals containing ActivitySeries connected to the slot \b launchActivity.
 *
 * @section Signals Signals
 * - \b activityLaunched(): signal emitted when the activity is launched
 *
 * @section Slots Slots
 * - \b launchActivity( data::ActivitySeries::sptr ): This slot allows to create a view for the given activity
 *   series.
 * - \b launchActivitySeries( data::Series::sptr ): This slot allows to create a view for the given activity
 *   series.
 *
 * @section Config Configuration
 *
 * This service should be instanciated in a Qml file, but it should also be registered in C++ AppManager.
 * You may need to configure it to add extra information in the replace map (map to define the string that will be
 * replaced in the launched activities, for example channels, object ids, ...
 *
 * @subsection Qml Qml Configuration
 * @code{.qml}
 *  SView {
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
    service::IService::ConfigType parameterViewConfig;
    parameterViewConfig.add("<xmlattr>.replace", "nameToReplace");
    parameterViewConfig.add("<xmlattr>.by", this->getInputID("value"));
    m_activityViewConfig.add_child("parameters.parameter", parameterViewConfig);
   @endcode
 * - \b parameters (optional) : additional parameters used to launch the activities
 *    - \b parameter: defines a parameter
 *        - \b replace: name of the parameter as defined in the AppConfig
 *        - \b by: defines the string that will replace the parameter name. It should be a simple string (ex.
 *          frontal) or define a camp path (ex. \@values.myImage). The root object of the sesh@ path if the
 *          composite contained in the ActivitySeries.
 *
 * To simplify the use of SView and SSequencer, a Qml object ActivityLauncher can be used
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
class MODULE_UI_QML_CLASS_API SView : public sight::ui::qml::IQmlEditor,
                                      public sight::activity::IActivityLauncher
{

Q_OBJECT
public:

    SIGHT_DECLARE_SERVICE(SView, ::sight::ui::qml::IQmlEditor)

    /// Constructor. Do nothing.
    MODULE_UI_QML_API SView();

    /// Destructor. Do nothing.

    MODULE_UI_QML_API ~SView() override;

    /// Signal emited when the activity is launched
    typedef core::com::Signal< void () > ActivityLaunchedSignalType;

Q_SIGNALS:
    void launchRequested(QUrl path, QVariantMap replace);

public Q_SLOTS:

    /// Emit 'activityLaunched' signal. Should be called by Qml when the activity's Qml file is pushed in the stack view
    void notifyActivityCreation();

protected:

    /// Parse the configuration (set the parameters to replace in the activities to launch
    void configuring() override;

    /// Do nothing
    void starting() override;

    /// Do nothing
    void stopping() override;

    /// Do nothing
    void updating() override;

private:

    /// Slot: Launch the given activity in the stackView.
    void launchActivity(data::ActivitySeries::sptr activitySeries);

    /// Slot: Launch the given activity in the stackView.
    void launchActivitySeries(data::Series::sptr series);

    ActivityLaunchedSignalType::sptr m_sigActivityLaunched;
};

} // uiActivitiesQml

/************************************************************************
 *
 * Copyright (C) 2017-2019 IRCAD France
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

#include "uiActivitiesQml/config.hpp"

#include <fwActivities/ActivityLauncher.hpp>

#include <fwMedData/ActivitySeries.hpp>

#include <fwQml/IQmlEditor.hpp>

#include <QUrl>
#include <QVariantMap>

namespace uiActivitiesQml
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
 * - \b launchActivity( ::fwMedData::ActivitySeries::sptr ): This slot allows to create a view for the given activity
 *   series.
 * - \b launchActivitySeries( ::fwMedData::Series::sptr ): This slot allows to create a view for the given activity
 *   series.
 *
 * @section Config Configuration
 *
 * This service sould be instanciated in a Qml file, but it may also be configured in C++ AppManager to add extra
 * information to replace in the activities that will be launched
 *
 * @subsection Qml Qml Configuration
 * @code{.qml}
 *  SActivityView {
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
    ::fwServices::IService::ConfigType parameterViewConfig;
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
 */
class UIACTIVITIESQML_CLASS_API SActivityView : public ::fwQml::IQmlEditor,
                                                public ::fwActivities::ActivityLauncher
{

Q_OBJECT
public:

    fwCoreServiceClassDefinitionsMacro( (SActivityView)(::fwQml::IQmlEditor) )

    /// Constructor. Do nothing.
    UIACTIVITIESQML_API SActivityView();

    /// Destructor. Do nothing.

    UIACTIVITIESQML_API virtual ~SActivityView() override;

    /// Signal emited when the activity is launched
    typedef ::fwCom::Signal< void () > ActivityLaunchedSignalType;

Q_SIGNALS:
    void launchRequested(QUrl path, QVariantMap replace);

public Q_SLOTS:

    /// Emit 'activityLaunched' signal. Should be called by Qml when the activity's Qml file is pushed in the stack view
    void notifyActivityCreation();

protected:

    /// Parse the configuration (set the parameters to replace in the activities to launch
    virtual void configuring() override;

    /// Do nothing
    virtual void starting() override;

    /// Do nothing
    virtual void stopping() override;

    /// Do nothing
    virtual void updating() override;

private:

    /// Slot: Launch the given activity in the stackView.
    void launchActivity(::fwMedData::ActivitySeries::sptr activitySeries);

    /// Slot: Launch the given activity in the stackView.
    void launchActivitySeries(::fwMedData::Series::sptr series);

    ActivityLaunchedSignalType::sptr m_sigActivityLaunched;
};

} // uiActivitiesQml

/************************************************************************
 *
 * Copyright (C) 2019 IRCAD France
 * Copyright (C) 2019 IHU Strasbourg
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

/**
 * @brief The namespace uiActivitiesQml contains helpers and services allowing to launch activities on Qml Applications.
 *
 * This namespace represents a Qml module that contains Qml objects to manage activities
 *
 * @section ActivitySequencer ActivitySequencer
 *
 *  ActivitySequencer object instantiate a SActivitySequencer service and allows to display the "stepper":
 *
 * @code{.qml}
 *  ActivitySequencer {
        activityIdsList: activityLauncher.activityIdsList
        activityNameList: activityLauncher.activityNameList

        onServiceCreated: {
            appManager.onServiceCreated(srv)
        }
    }
    @endcode
 * - \n activityIdsList: identifiers of the activities to launch
 * - \n activityNameList: name of the activities to launch, that will be displays in the stepper
 *
 * It should be associated to a SActivityView to launch the activities.
 *
 * @section ActivityLauncher ActivityLauncher
 *
 * ActivityLauncher uses the ActivitySequencer object with the SActivityView. It contains an AppManager an can be used
 * standalone to manage activitiy launching.
 *
 * It displays a "stepper" to launch the activities sequentially and display the current activity in the main container.
 *
 *  @code{.qml}
    ActivityLauncher {
        id: activityLauncher
        activityIdsList: ["ExImageReading", "ExMesher", "ExImageDisplaying"]
        activityNameList: ["Read", "Mesher", "Display"]
    }
   @endcode
 * - \n activityIdsList: identifiers of the activities to launch
 * - \n activityNameList: name of the activities to launch, that will be displays in the stepper
 *
 * @section Activity Activity
 *
 * This object provides a template for the activity that will be launched.
 *
 *  @code{.qml}
    Activity {
        id: exImageDisplaying
        appManager: MesherManager {
            id: appManager
            frameBuffer: scene3D
        }
        // Your layout, object, service...
        // ...
    }
   @endcode
 *
 **/
namespace uiActivitiesQml
{

} // namespace uiActivitiesQml

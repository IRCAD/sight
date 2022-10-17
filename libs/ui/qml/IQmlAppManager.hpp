/************************************************************************
 *
 * Copyright (C) 2019-2022 IRCAD France
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

#include "ui/qml/config.hpp"

#include <service/AppManager.hpp>
#include <service/IService.hpp>

#include <QObject>
#include <QVariant>

namespace sight::ui::qml
{

/**
 * @brief Base class for AppManager managing Qml services
 *
 * The AppManager inheriting from this class should be instantiated in Qml file.
 *
 * @code{.qml}
   Item {
       MyAppManager {
            id: appManager
        }

        SService {
            id: myService

            onStarted: {
                //...
            }
        }

        //...
        Component.onCompleted: {
            appManager.initialize()
            appManager.onServiceCreated(myService)
        }
    }
   @endcode
 * When the Qml component is created, the AppManager should be initialized, and the service can be registered (you can
 * do it in the order you want).
 */
class UI_QML_CLASS_QT_API IQmlAppManager : public QObject,
                                           public service::AppManager
{
Q_OBJECT

public:

    /// Constructor.
    UI_QML_QT_API IQmlAppManager() noexcept;

    /// Destructor. Do nothing.
    UI_QML_QT_API ~IQmlAppManager() noexcept override;

public Q_SLOTS:

    /// Initialize the manager
    UI_QML_QT_API virtual void initialize();

    /// Uninitialize the manager
    UI_QML_QT_API virtual void uninitialize();

    /**
     * @brief Retrieves the services instantiated in Qml. It should be reimplemented to properly configure and register
     * the services
     *
     * @param obj variant representing a service
     *
     * The service can be retrived like:
     * @code{.cpp}
        ui::qml::IQmlEditor::sptr srv(obj.value< ui::qml::IQmlEditor* >());
        if (srv && srv->isA("sight::module::ui::qml::activity::SView"))
        {
            // register input/inout
            srv->registerInOut(data, "myData", true);

            // configure the service
            ConfigType config;
            // ...
            srv->configure(config);

            // add proxy connections
            service::helper::ProxyConnections connection(this->getInputID("myChannel"));
            connection.addSlotConnection(srv->getID(), "mySlot");
            this->addProxyConnection(connection);

            // register the service
            this->addService(srv, true);
        }
       @endcode
     */
    UI_QML_QT_API virtual void onServiceCreated(const QVariant& obj);

    /**
     * @brief Retrieve the replaceMap from Qml and associate it to this AppManager
     * @see replaceInput()
     */
    UI_QML_QT_API virtual void replaceInputs(const QVariant& map) final;

protected:

    bool m_isInitialized {false};
};

} // namespace sight::ui::qml

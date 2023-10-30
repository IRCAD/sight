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

#pragma once

#include "modules/io/dimse/config.hpp"

#include <core/thread/worker.hpp>

#include <io/dimse/data/PacsConfiguration.hpp>
#include <io/dimse/SeriesEnquirer.hpp>

#include <service/notifier.hpp>

#include <ui/__/editor.hpp>

#include <QComboBox>
#include <QLineEdit>
#include <QObject>
#include <QPointer>
#include <QPushButton>
#include <QSpinBox>
#include <QWidget>

#include <filesystem>

namespace sight::module::io::dimse
{

/**
 * @brief This editor is used to edit a pacs configuration.
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service type="sight::module::io::dimse::pacs_configuration_editor">
        <inout key="config" uid="..." />
        <config showDialog="true" />
    </service>
   @endcode
 *
 * @subsection In-Out In-Out
 * - \b config [sight::io::dimse::data::PacsConfiguration]: pacs configuration.
 *
 * @subsection Configuration Configuration:
 * - \b showDialog (optional, bool, default=true): display dialog message for the ping result, or just send the
 *                                                 notification.
 */
class MODULE_IO_DIMSE_CLASS_API pacs_configuration_editor : public QObject,
                                                            public sight::ui::editor,
                                                            private sight::service::notifier
{
Q_OBJECT;

public:

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(pacs_configuration_editor, sight::ui::editor);

    /// Creates the service.
    MODULE_IO_DIMSE_API pacs_configuration_editor() noexcept;

    /// Destroyes the service.
    MODULE_IO_DIMSE_API ~pacs_configuration_editor() noexcept override = default;

protected:

    /// Configures the editor.
    MODULE_IO_DIMSE_API void configuring() override;

    /// Creates the UI.
    MODULE_IO_DIMSE_API void starting() override;

    /// Does nothing.
    MODULE_IO_DIMSE_API void updating() override;

    /// Destroyes the UI.
    MODULE_IO_DIMSE_API void stopping() override;

private:

    /**
     * @brief Sends a modified signal on the configuration.
     * @param _pacsConfiguration the modified data .
     */
    void modified_notify(sight::io::dimse::data::pacs_configuration::sptr _pacs_configuration);

    /**
     * @brief Displays an information dialog.
     * @param _title title of the dialog.
     * @param _message message of the dialog.
     */
    static void show_dialog(const std::string _title, const std::string _message);

    /// Contains the AET of the SCU (client name) editor.
    QPointer<QLineEdit> m_scu_app_entity_title_edit;

    /// Contains the AET of the SCP (server name) editor.
    QPointer<QLineEdit> m_scp_app_entity_title_edit;

    /// Contains the SCP host name (server address) editor.
    QPointer<QLineEdit> m_scp_host_name_edit;

    /// Contains the SCP port (server port) editor.
    QPointer<QSpinBox> m_scp_port_edit;

    /// Contains the request mode, GET or MOVE, editor.
    QPointer<QComboBox> m_retrieve_method_widget;

    /**
     * @brief Contains the move AET editor. This AET is use to receive C-MOVE responses.
     *
     * C-MOVE request are sent from the SCU to the SCP. The SCP will send its response based on its configuration.
     * Usually the configuration contains an IP and a port that match SCU configuration.
     * For more information, see the link bellow:
     * https://book.orthanc-server.com/dicom-guide.html#dicom-network-protocol.
     */
    QPointer<QLineEdit> m_move_app_entity_title_edit;

    /// Contains the move port editor. This port is use to receive C-MOVE responses.
    QPointer<QSpinBox> m_move_port;

    /// Contains the test button, sends a C-ECHO request to the PACS.
    QPointer<QPushButton> m_ping_pacs_button_widget;

    /// Contains the worker of the series enquire thread.
    core::thread::worker::sptr m_request_worker;

    /// Defines whether or not the dialog message should be displayed for the ping result.
    bool m_show_dialog {true};

    /// Contains the slot to show a dialog in the main thread.
    core::com::slot<void(const std::string, const std::string)>::sptr m_slot_show_dialog {nullptr};

private Q_SLOTS:

    /// Calls for pinging the pacs.
    void ping_pacs();

    /// Calls for changing AET of the SCU.
    void on_scu_app_entity_title_changed();

    /// Calls for changing AET of the SCP.
    void on_scp_app_entity_title_changed();

    /// Calls for changing the IP of the SCP.
    void on_scp_host_name_changed();

    /**
     * @brief Calls for changing the port of the SCP.
     * @param _value the PACS application port.
     */
    void on_scp_port_changed(int _value);

    /**
     * @brief Calls for changing the retrieve method.
     * @param _index the retrieve method index.
     */
    void on_retrieve_method_changed(int _index);

    /// Calls for changing the move AET.
    void on_move_app_entity_title_changed();

    /**
     * @brief Calls for changing the move IP.
     * @param _value the move application port.
     */
    void on_move_port_changed(int _value);

private:

    data::ptr<sight::io::dimse::data::pacs_configuration, data::access::inout> m_config {this, "config"};
};

} // namespace sight::module::io::dimse.

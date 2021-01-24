/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "ioPacs/config.hpp"

#include <core/thread/Worker.hpp>

#include <fwGui/editor/IEditor.hpp>

#include <fwPacsIO/data/PacsConfiguration.hpp>
#include <fwPacsIO/SeriesEnquirer.hpp>

#include <QComboBox>
#include <QLineEdit>
#include <QObject>
#include <QPointer>
#include <QPushButton>
#include <QSpinBox>
#include <QWidget>

#include <filesystem>

namespace ioPacs
{

/**
 * @brief This editor is used to edit a pacs configuration.
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service type="::ioPacs::SPacsConfigurationEditor">
        <inout key="config" uid="..." />
        <config showDialog="true" />
    </service>
   @endcode
 *
 * @subsection In-Out In-Out
 * - \b config [::fwPacsIOdata::PacsConfiguration]: pacs configuration.
 *
 * @subsection Configuration Configuration:
 * - \b showDialog (optional, bool, default=true): display dialog message for the ping result, or just send the
 *                                                 notification.
 */
class IOPACS_CLASS_API SPacsConfigurationEditor : public QObject,
                                                  public ::fwGui::editor::IEditor
{

Q_OBJECT;

public:

    /// Generates default methods as New, dynamicCast, ...
    fwCoreServiceMacro(SPacsConfigurationEditor,  ::fwGui::editor::IEditor )

    /// Creates the service.
    IOPACS_API SPacsConfigurationEditor() noexcept;

    /// Destroyes the service.
    IOPACS_API virtual ~SPacsConfigurationEditor() noexcept;

protected:

    /// Configures the editor.
    IOPACS_API void configuring() override;

    /// Creates the UI.
    IOPACS_API void starting() override;

    /// Does nothing.
    IOPACS_API void updating() override;

    /// Destroyes the UI.
    IOPACS_API void stopping() override;

private:

    /**
     * @brief Sends a modified signal on the configuration.
     * @param _pacsConfiguration the modified data .
     */
    void modifiedNotify(::fwPacsIOdata::PacsConfiguration::sptr _pacsConfiguration);

    /**
     * @brief Displays an information dialog.
     * @param _title title of the dialog.
     * @param _message message of the dialog.
     */
    void showDialog(const std::string _title, const std::string _message);

    /// Contains the AET of the SCU (client name) editor.
    QPointer< QLineEdit > m_SCUAppEntityTitleEdit;

    /// Contains the AET of the SCP (server name) editor.
    QPointer< QLineEdit > m_SCPAppEntityTitleEdit;

    /// Contains the SCP host name (server adress) editor.
    QPointer< QLineEdit > m_SCPHostNameEdit;

    /// Contains the SCP port (server port) editor.
    QPointer< QSpinBox > m_SCPPortEdit;

    /// Contains the request mode, GET or MOVE, editor.
    QPointer< QComboBox > m_retrieveMethodWidget;

    /**
     * @brief Contains the move AET editor. This AET is use to receive C-MOVE responses.
     *
     * C-MOVE request are sent from the SCU to the SCP. The SCP will send its response based on its configuration.
     * Usually the configuration contains an IP and a port that match SCU configuration.
     * For more information, see the link bellow:
     * https://book.orthanc-server.com/dicom-guide.html#dicom-network-protocol.
     */
    QPointer< QLineEdit > m_moveAppEntityTitleEdit;

    /// Contains the move port editor. This port is use to receive C-MOVE responses.
    QPointer< QSpinBox > m_movePort;

    /// Contains the test button, sends a C-ECHO request to the PACS.
    QPointer< QPushButton > m_pingPacsButtonWidget;

    /// Contains the worker of the series enquire thread.
    core::thread::Worker::sptr m_requestWorker;

    /// Defines whether or not the dialog message should be displayed for the ping result.
    bool m_showDialog { true };

    /// Contains the slot to show a dialog in the main thread.
    core::com::Slot< void(const std::string, const std::string) >::sptr m_slotShowDialog { nullptr };

private Q_SLOTS:

    /// Calls for pinging the pacs.
    void pingPACS();

    /// Calls for changing AET of the SCU.
    void onSCUAppEntityTitleChanged();

    /// Calls for changing AET of the SCP.
    void onSCPAppEntityTitleChanged();

    /// Calls for changing the IP of the SCP.
    void onSCPHostNameChanged();

    /**
     * @brief Calls for changing the port of the SCP.
     * @param _value the PACS application port.
     */
    void onSCPPortChanged(int _value);

    /**
     * @brief Calls for changing the retrieve method.
     * @param _index the retrieve method index.
     */
    void onRetrieveMethodChanged(int _index);

    /// Calls for changing the move AET.
    void onMoveAppEntityTitleChanged();

    /**
     * @brief Calls for changing the move IP.
     * @param _value the move application port.
     */
    void onMovePortChanged(int _value);

};

} // namespace ioPacs.

/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "modules/ui/qt/config.hpp"

#include <core/com/signal.hpp>
#include <core/com/signals.hpp>
#include <core/tools/failed.hpp>

#include <ui/__/editor.hpp>

#include <QObject>
#include <QPointer>

class QPushButton;

namespace sight::module::ui::qt::viz
{

/**
 * @brief   snapshot_editor service is represented by a button. It allows to snap shot a generic scene.
 *
 * Send a 'snapped' signal containing the filename used to save the snapshot.
 * @note You need to connect the 'snapped' signal to one visuVTKAdaptor::Snapshot to save the file.
 */
class MODULE_UI_QT_CLASS_API snapshot_editor : public QObject,
                                               public sight::ui::editor
{
Q_OBJECT

public:

    SIGHT_DECLARE_SERVICE(snapshot_editor, sight::ui::editor);

    /// Constructor. Do nothing.
    MODULE_UI_QT_API snapshot_editor() noexcept;

    /// Destructor. Do nothing.
    MODULE_UI_QT_API ~snapshot_editor() noexcept override;

protected:

    /**
     * @brief Install the layout.
     */
    void starting() override;

    /**
     * @brief Destroy the layout.
     */
    void stopping() override;

    /// Do nothing
    void updating() override;

    /**
     * @brief Configure the editor.
     *
     * Example of configuration
     * @code{.xml}
       <service uid="snapshot_editor" type="sight::module::ui::qt::viz::snapshot_editor" auto_connect="false" />
       @endcode
     */
    void configuring() override;

    /// Overrides
    void info(std::ostream& _sstream) override;

protected Q_SLOTS:

    /**
     * @brief Show a file dialog and notify the scene must be printed.
     */
    void on_snap_button();

private:

    static std::string request_file_name();

    /**
     * @name Signals
     * @{
     */

    /// Type of signal to snap shot
    using snapped_signal_t = core::com::signal<void (std::string)>;
    static const core::com::signals::key_t SNAPPED_SIG;

    snapped_signal_t::sptr m_sig_snapped; ///< snap shot signal
    /**
     * @}
     */

    QPointer<QPushButton> m_snap_button;
};

} // namespace sight::module::ui::qt::viz

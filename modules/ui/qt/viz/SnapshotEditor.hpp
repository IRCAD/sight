/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include <core/com/Signal.hpp>
#include <core/com/Signals.hpp>
#include <core/tools/Failed.hpp>

#include <ui/base/IEditor.hpp>

#include <QObject>
#include <QPointer>

class QPushButton;

namespace sight::module::ui::qt::viz
{

/**
 * @brief   SnapshotEditor service is represented by a button. It allows to snap shot a generic scene.
 *
 * Send a 'snapped' signal containing the filename used to save the snapshot.
 * @note You need to connect the 'snapped' signal to one visuVTKAdaptor::Snapshot to save the file.
 */
class MODULE_UI_QT_CLASS_API SnapshotEditor : public QObject,
                                              public sight::ui::base::IEditor
{
Q_OBJECT

public:

    SIGHT_DECLARE_SERVICE(SnapshotEditor, sight::ui::base::IEditor)

    /// Constructor. Do nothing.
    MODULE_UI_QT_API SnapshotEditor() noexcept;

    /// Destructor. Do nothing.
    MODULE_UI_QT_API virtual ~SnapshotEditor() noexcept;

protected:

    typedef core::runtime::ConfigurationElement::sptr Configuration;

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

    /// Do nothing
    void swapping() override;

    /**
     * @brief Configure the editor.
     *
     * Example of configuration
     * @code{.xml}
       <service uid="snapshotEditor" type="sight::module::ui::qt::viz::SnapshotEditor" autoConnect="false" />
       @endcode
     */
    void configuring() override;

    /// Overrides
    void info(std::ostream& _sstream) override;

protected Q_SLOTS:

    /**
     * @brief Show a file dialog and notify the scene must be printed.
     */
    void onSnapButton();

private:

    std::string requestFileName();

    /**
     * @name Signals
     * @{
     */

    /// Type of signal to snap shot
    typedef core::com::Signal<void (std::string)> SnappedSignalType;
    static const core::com::Signals::SignalKeyType s_SNAPPED_SIG;

    SnappedSignalType::sptr m_sigSnapped; ///< snap shot signal
    /**
     * @}
     */

    QPointer<QPushButton> m_snapButton;
};

} // uiVisuQt

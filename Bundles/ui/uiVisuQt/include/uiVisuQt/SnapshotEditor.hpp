/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#include "uiVisuQt/config.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signals.hpp>

#include <fwGui/editor/IEditor.hpp>

#include <fwTools/Failed.hpp>

#include <QObject>
#include <QPointer>

class QPushButton;

namespace uiVisuQt
{

/**
 * @brief   SnapshotEditor service is represented by a button. It allows to snap shot a generic scene.
 *
 * Send a 'snapped' signal containing the filename used to save the snapshot.
 * @note You need to connect the 'snapped' signal to one visuVTKAdaptor::Snapshot to save the file.
 */
class UIVISUQT_CLASS_API SnapshotEditor : public QObject,
                                          public ::fwGui::editor::IEditor
{

Q_OBJECT

public:

    fwCoreServiceClassDefinitionsMacro( (SnapshotEditor)(::fwGui::editor::IEditor) );

    /// Constructor. Do nothing.
    UIVISUQT_API SnapshotEditor() noexcept;

    /// Destructor. Do nothing.
    UIVISUQT_API virtual ~SnapshotEditor() noexcept;

protected:

    typedef ::fwRuntime::ConfigurationElement::sptr Configuration;

    /**
     * @brief Install the layout.
     */
    virtual void starting() override;

    /**
     * @brief Destroy the layout.
     */
    virtual void stopping() override;

    /// Do nothing
    virtual void updating() override;

    /// Do nothing
    virtual void swapping() override;

    /**
     * @brief Configure the editor.
     *
     * Example of configuration
     * @code{.xml}
       <service uid="snapshotEditor" type="::uiVisuQt::SnapshotEditor" autoConnect="no" />
       @endcode
     */
    virtual void configuring() override;

    /// Overrides
    virtual void info( std::ostream& _sstream ) override;

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
    typedef ::fwCom::Signal< void (std::string) > SnappedSignalType;
    static const ::fwCom::Signals::SignalKeyType s_SNAPPED_SIG;

    SnappedSignalType::sptr m_sigSnapped; ///< snap shot signal
    /**
     * @}
     */

    QPointer< QPushButton > m_snapButton;

};

} // uiVisuQt

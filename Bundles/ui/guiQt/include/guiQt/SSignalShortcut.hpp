/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "guiQt/config.hpp"

#include <fwServices/IService.hpp>

#include <QApplication>

namespace guiQt
{

/**
 * @brief   This service sends a signal when the associated shortcut is triggered.
 * @section XML XML configuration
 * @code{.xml}
   <service uid="..." impl="::guiQt::SSignalShortcut" >
        <config shortcut="..." wid="${WID_PARENT}" />
   </service>
   @endcode
 * @subsection Configuration Configuration
 * - \b shortcut: associated shortcut
 * - \b wid: window id in which the shortcut will be enabled
 *
 * @section Signals Signals
 * - \b triggered(): This signal is emitted when the shortcut is received.
 */
class GUIQT_CLASS_API SSignalShortcut : public QObject,
                                        public ::fwServices::IService
{
Q_OBJECT
public:

    fwCoreServiceClassDefinitionsMacro( (SSignalShortcut)(::fwServices::IService) );

    /// Signal emitted when the shortcut is received.
    typedef ::fwCom::Signal< void () > TriggeredShortcutSignalType;

    /// Constructor. Do nothing.
    GUIQT_API SSignalShortcut() noexcept;

    /// Destructor. Do nothing.
    GUIQT_API virtual ~SSignalShortcut() noexcept;

protected:

    /** @name Service methods ( override from ::fwServices::IService )
     * @{
     */

    /// This method configures the service
    GUIQT_API virtual void configuring() override;

    /**
     * @brief This method enables the eventFilter
     */
    GUIQT_API virtual void starting() override;

    /**
     * @brief This method deletes the eventFilter
     */
    GUIQT_API virtual void stopping() override;

    /**
     * @brief This method does nothing.
     */
    GUIQT_API virtual void updating() override;

    /// Filter specified shortcut
    bool eventFilter(QObject* obj, QEvent* event) override;

private:

    /// string containing the shortcut to trigger
    std::string m_shortcut;

    /// Window id used to get the QtContainer of the activity to set up a shortcut in
    std::string m_wid;
};

}

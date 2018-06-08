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
        <config shortcut="Ctrl+K" />
   </service>
   @endcode
 * @subsection Configuration Configuration
 * - \b shortcut: associated shortcut
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

    /// This method is used to configure the service
    GUIQT_API virtual void configuring() override;

    /// Filter specified shortcut
    bool eventFilter(QObject* obj, QEvent* event) override;

private:

    /// string containing the shortcut to trigger
    std::string m_shortcut;

};

}

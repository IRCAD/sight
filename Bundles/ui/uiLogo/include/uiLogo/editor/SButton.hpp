/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "uiLogo/config.hpp"

#include <fwCom/HasSignals.hpp>
#include <fwCom/Signal.hpp>

#include <fwGui/editor/IEditor.hpp>

#include <QObject>

namespace uiLogo
{

namespace editor
{

/**
 * @brief Configurable button
 * Creates a button with optional label, logo and tootip
 * Once the user clicks the button, a signal is sent
 */
class UILOGO_CLASS_API SButton : public QObject,
                                 public ::fwGui::editor::IEditor
{
Q_OBJECT
public:
    fwCoreServiceClassDefinitionsMacro( (SButton)(::fwGui::editor::IEditor) );

    /// Triggered signal type

    typedef ::fwCom::Signal< void () > TrigerredSignalType;

    /// Constructor. Initializes triggered signal
    UILOGO_API SButton() noexcept;

    /// Destructor
    UILOGO_API virtual ~SButton() noexcept;

    /**
     * @brief This method launches the IEditor::starting method
     * Initializes button according to xml configuration
     */
    UILOGO_API void starting() override;

    /// This method launches the IEditor::stopping method
    UILOGO_API void stopping() override;

    /// This method is used to update services. Sends triggered signal
    UILOGO_API void updating() override;

    /// This method is used to update services on notification. Do nothing
    UILOGO_API void receiving( );

    /**
     * @brief Configure the service.
     * @see fwGui::IGuiContainerSrv::initialize()
     *
     * @code{.xml}
            <service uid="${GENERIC_UID}_logo_button" impl="::uiLogo::editor::SButton" autoConnect="no">
                <image>media-0.1/icons/*.png</image>
                <label>Label</label>
                <tooltip>Tooltip</tooltip>
            </service>
       @endcode
     */
    UILOGO_API void configuring() override;

    /// This method is used to give information about the service. Do nothing
    UILOGO_API void info(std::ostream& _sstream) override;

protected Q_SLOTS:
    /// This method is called when the user clicks OK button
    void onClick();

private:

    /// The optional image displayed on the button
    std::string m_image;

    /// The optional label displayed on the button
    std::string m_label;

    /// The optional tooltip of the button
    std::string m_tooltip;
    SPTR(TrigerredSignalType) m_sigTriggered;

};

}   //namespace uiLogo
}   //namespace editor

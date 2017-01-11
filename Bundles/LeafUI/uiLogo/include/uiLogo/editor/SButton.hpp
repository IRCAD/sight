/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __UILOGO_EDITOR_SBUTTON_HPP__
#define __UILOGO_EDITOR_SBUTTON_HPP__

#include "uiLogo/config.hpp"

#include <gui/editor/IEditor.hpp>

#include <fwCom/HasSignals.hpp>
#include <fwCom/Signal.hpp>

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
                                 public ::gui::editor::IEditor
{
Q_OBJECT
public:
    fwCoreServiceClassDefinitionsMacro ( (SButton)(::gui::editor::IEditor) );


    /// Triggered signal type

    typedef ::fwCom::Signal< void () > TrigerredSignalType;

    /// Constructor. Initializes triggered signal
    UILOGO_API SButton() throw ();

    /// Destructor
    UILOGO_API virtual ~SButton() throw ();

    /**
     * @brief This method launches the IEditor::starting method
     * Initializes button according to xml configuration
     */
    UILOGO_API void starting() throw (::fwTools::Failed);

    /// This method launches the IEditor::stopping method
    UILOGO_API void stopping() throw (::fwTools::Failed);

    /// This method is used to update services. Sends triggered signal
    UILOGO_API void updating() throw (::fwTools::Failed);

    /// This method is used to update services on notification. Do nothing
    UILOGO_API void receiving( ) throw (::fwTools::Failed);

    /**
     * @brief Configure the service.
     * @see fwGui::IGuiContainerSrv::initialize()
     *
     * @code{.xml}
            <service uid="${GENERIC_UID}_logo_button" impl="::uiLogo::editor::SButton" autoConnect="no">
                <image>@BUNDLE_PREFIX@/media_0-1/icons/*.png</image>
                <label>Label</label>
                <tooltip>Tooltip</tooltip>
            </service>
       @endcode
     */
    UILOGO_API void configuring() throw (::fwTools::Failed);

    /// This method is used to give information about the service. Do nothing
    UILOGO_API void info(std::ostream& _sstream);

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

#endif /* __UILOGO_EDITOR_SBUTTON_HPP__ */

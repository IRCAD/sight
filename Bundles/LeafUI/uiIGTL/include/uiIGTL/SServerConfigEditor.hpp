/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __UIIGTL_SSERVERCONFIGEDITOR_HPP__
#define __UIIGTL_SSERVERCONFIGEDITOR_HPP__

#include "uiIGTL/config.hpp"

#include <gui/editor/IDialogEditor.hpp>
#include <fwCom/Signal.hpp>

#include <boost/type.hpp>

#include <QDialog>
#include <QSpinBox>
#include <QLineEdit>
#include <QObject>
#include <QPointer>


namespace uiIGTL
{
/**
 *
 * @brief Config editor for sender
 */
class UIIGTL_CLASS_API SServerConfigEditor :
    public QObject,
    public ::gui::editor::IDialogEditor
{
Q_OBJECT
public:

    fwCoreServiceClassDefinitionsMacro ( (SServerConfigEditor) (::gui::editor::IDialogEditor));

    /**
     * @brief Configuration updated signal is emitted when user confirm change
     */
    UIIGTL_API static const ::fwCom::Signals::SignalKeyType s_CONFIGURATION_UPDATED_SIGNAL;

    /**
     * @typedef ConfigurationUpdatedSignalType
     *
     * @brief ConfigurationUpdatedSignalType is stored and emitted when user confirm change
     */
    typedef ::fwCom::Signal< void (::boost::uint16_t const ) >        ConfigurationUpdatedSignalType;


    /// Constructor
    UIIGTL_API SServerConfigEditor();

    /// Destructor
    UIIGTL_API virtual ~SServerConfigEditor() throw();

protected:
    /// Overrides
    UIIGTL_API virtual void configuring() throw (::fwTools::Failed);

    /// Overrides
    UIIGTL_API virtual void starting() throw (::fwTools::Failed);

    /// Overrides
    UIIGTL_API virtual void stopping() throw (::fwTools::Failed);

    /**
     * @brief Show server config editor
     */
    UIIGTL_API virtual void updating() throw (::fwTools::Failed);

    /// Overrides
    UIIGTL_API virtual void swapping() throw (::fwTools::Failed);

private Q_SLOTS:

    /**
     * @brief when user click on validate button
     */
    void onValidate();
private:

    /// Configuration dialog
    QDialog m_configDialog;

    /// port spinbox max is a 16bit unsigned integer
    QPointer<QSpinBox> m_port;

    /// Configuration update signal
    ConfigurationUpdatedSignalType::sptr m_configurationUpdatedSignal;
};



} // namespace uiIGTL

#endif /*__UIIGTL_SSERVERCONFIGEDITOR_HPP__*/


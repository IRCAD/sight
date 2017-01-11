/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __UIIGTL_SCLIENTCONFIGEDITOR_HPP__
#define __UIIGTL_SCLIENTCONFIGEDITOR_HPP__

#include "uiIGTL/config.hpp"

#include <gui/editor/IDialogEditor.hpp>
#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <QDialog>
#include <QObject>
#include <QSpinBox>
#include <QLineEdit>
#include <QPointer>

namespace uiIGTL
{

/**
 *
 * @brief Widget class for configure OpenIGTLinkListener
 */
class UIIGTL_CLASS_API SClientConfigEditor :
    public QObject,
    public ::gui::editor::IDialogEditor
{

Q_OBJECT
public:

    /**
     * @brief Configuration updated signal is emitted only when user confirm change
     */
    UIIGTL_API static const ::fwCom::Signals::SignalKeyType s_CONFIGURATION_UPDATED_SIGNAL;

    /**
     * @typedef ConfigurationUpdatedSignalType
     *
     * @brief ConfigurationUpdatedSignalType is stored and emitted when user confirm change
     */
    typedef ::fwCom::Signal< void (std::string const, std::uint16_t const) >      ConfigurationUpdatedSignalType;

    fwCoreServiceClassDefinitionsMacro ( (SClientConfigEditor) (::gui::editor::IDialogEditor));

    /// Constructor
    UIIGTL_API SClientConfigEditor();

    /// Destructor
    UIIGTL_API virtual ~SClientConfigEditor() throw();

protected:
    /// Overrides
    UIIGTL_API virtual void configuring() throw (::fwTools::Failed);

    /// Overrides
    UIIGTL_API virtual void starting() throw (::fwTools::Failed);

    /// Overrides
    UIIGTL_API virtual void stopping() throw (::fwTools::Failed);

    /**
     * @brief Show client config editor
     */
    UIIGTL_API virtual void updating() throw (::fwTools::Failed);

    /// Overrides
    UIIGTL_API virtual void swapping() throw (::fwTools::Failed);

private Q_SLOTS:

    void onValidate();
private:
    /// configuration dialog instance
    QDialog m_configDialog;

    /// line edit to modifiy hostname
    QPointer<QLineEdit> m_hostname;

    /// spinbox to modifiy port
    QPointer<QSpinBox> m_port;

    /// configuration updarte signal is emitted when user confirm change
    ConfigurationUpdatedSignalType::sptr m_configurationUpdatedSignal;

    std::string m_defaultHostName;
    std::uint16_t m_defaultPort;

};



} // namespace uiIGTL

#endif /*__UIIGTL_SCLIENTCONFIGEDITOR_HPP__*/


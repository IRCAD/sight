/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __UIZMQ_SZMQCONFIGEDITOR_HPP__
#define __UIZMQ_SZMQCONFIGEDITOR_HPP__

#include "uiZMQ/config.hpp"

#include <gui/editor/IDialogEditor.hpp>
#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <zmqNetwork/Socket.hpp>

#include <QDialog>
#include <QObject>
#include <QComboBox>
#include <QSpinBox>
#include <QWidget>
#include <QLineEdit>
#include <QLabel>

namespace uiZMQ
{
/**
 *
 * @brief config editor for ioZMQ listener and sender services
 */
class UIZMQ_CLASS_API SZMQConfigEditor :
    public QObject,
    public ::gui::editor::IDialogEditor
{
Q_OBJECT
public:

    /**
     * @brief Configuration updated signal is emitted when a user confirm change
     */
    UIZMQ_API static const ::fwCom::Signals::SignalKeyType s_CONFIGURATION_UPDATED_SIGNAL;

    /**
     * @typedef ConfigurationUpdatedSignalType
     *
     * @brief ConfigurationUpdatedSignalType is stored and emitted when user confirm change
     */
    typedef ::fwCom::Signal < void (::zmqNetwork::Socket::PatternMode const,
                                    ::zmqNetwork::Socket::SocketMode const,
                                    std::string const& host) > ConfigurationUpdatedSignalType;

    fwCoreServiceClassDefinitionsMacro ( (SZMQConfigEditor) (::gui::editor::IDialogEditor));

    /**
     * @brief create gui interface
     */
    UIZMQ_API virtual void configuring() throw (::fwTools::Failed);

    /// Overrides
    UIZMQ_API virtual void starting() throw (::fwTools::Failed);

    /// Overrides
    UIZMQ_API virtual void stopping() throw (::fwTools::Failed);

    /**
     * @brief show the dialog
     */
    UIZMQ_API virtual void updating() throw (::fwTools::Failed);

    /// Overrides
    UIZMQ_API virtual void swapping() throw (::fwTools::Failed);

    /// Constructor
    UIZMQ_API SZMQConfigEditor();

    /**
     * @brief this method will be removed later implemented in child classes. These child classes only fill the
     *  m_patternBox
     */
    UIZMQ_API virtual void initPatternBox() = 0;

    /// Destructor
    UIZMQ_API virtual ~SZMQConfigEditor() throw();

private Q_SLOTS:
    /**
     * @brief called when a user click to validate button. This method send a ConfigurationUpdated signal.
     */
    void onValidate();

    /**
     * @brief called when user change protocol
     */
    void onChangeProtocol(QString const& protocol);
protected:
    QComboBox* m_patternBox;
private:

    QComboBox* m_modeBox;
    QComboBox* m_protocolBox;
    QLineEdit* m_host;
    QSpinBox* m_port;
    QLabel* m_title;
    QWidget* m_portWidget;
    QDialog m_dialog;
    std::string m_protocol;

    ConfigurationUpdatedSignalType::sptr m_configurationUpdatedSig;
};



} // namespace uiZMQ

#endif /*__UIZMQ_SZMQCONFIGEDITOR_HPP__*/


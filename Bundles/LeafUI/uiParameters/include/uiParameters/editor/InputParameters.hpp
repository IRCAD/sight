/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __UIPARAMETERS_EDITOR_INPUTPARAMETERS_HPP__
#define __UIPARAMETERS_EDITOR_INPUTPARAMETERS_HPP__

#include "uiParameters/config.hpp"

#include <gui/editor/IEditor.hpp>

#include <fwCom/HasSignals.hpp>
#include <fwCom/Signal.hpp>
#include <fwData/Object.hpp>
#include <fwData/Integer.hpp>
#include <fwData/Boolean.hpp>

#include <QObject>

namespace uiParameters
{

namespace editor
{

/**
 * @class InputParameters
 * @brief Editor to input Parameters.
 * It allow the user to input configurable parameters
 * Once the user validate the parameters, a signal is sent containing the key
 * and the value
 * It supports integer and boolean yet
 */
class UIPARAMETERS_CLASS_API InputParameters : public QObject,
                                               public ::gui::editor::IEditor
{
Q_OBJECT
public:
    fwCoreServiceClassDefinitionsMacro ( (InputParameters)(::gui::editor::IEditor) );

    /// Integer parameter type
    typedef std::pair< std::string, int > IntegerParameter;

    /// Boolean parameter type
    typedef std::pair< std::string, bool > BooleanParameter;

    /// Triggered signal type
    typedef ::fwCom::Signal< void () > TrigerredSignalType;

    /// Integer changed signal type
    typedef ::fwCom::Signal< void (IntegerParameter) > IntegerChangedSignalType;

    /// Boolean changed signal type
    typedef ::fwCom::Signal< void (BooleanParameter) > BooleanChangedSignalType;

    /// Constructor. Initializes signals
    UIPARAMETERS_API InputParameters() throw ();

    /// Destructor. Does nothing
    UIPARAMETERS_API virtual ~InputParameters() throw ();

    /**
     * @brief This method launches the IEditor::starting method
     * Initializes Qt input widgets for parameters according to xml configuration
     */
    UIPARAMETERS_API void starting() throw (::fwTools::Failed);

    /// This method launches the IEditor::stopping method
    UIPARAMETERS_API void stopping() throw (::fwTools::Failed);

    /// This method is used to update services. Does nothing
    UIPARAMETERS_API void updating() throw (::fwTools::Failed);

    /// This method is used to update services on notification. Does nothing
    UIPARAMETERS_API void receiving( ) throw (::fwTools::Failed);

    /**
     * @brief Configure the service.
     * @see fwGui::IGuiContainerSrv::initialize()
     *
     * @verbatim
       <service uid="${GENERIC_UID}_input_parameters" impl="::uiParameters::editor::InputParameters" autoConnect="no">
        <parameters>
            <param type="::fwData::Integer" name="Value min to mesh" key="valueMin" defaultValue="1" min="0" max = "255" />
            <param type="::fwData::Integer" name="Value max to mesh" key="valueMax" defaultValue="255" min="0" max = "255" />
            <param type="::fwData::Boolean" name="Close the mesh" key="closing" defaultValue="false" />
        </parameters>
       </service>
       @endverbatim
     */
    UIPARAMETERS_API void configuring() throw (::fwTools::Failed);

    /// This method is used to give information about the service. Does nothing
    UIPARAMETERS_API void info(std::ostream &_sstream);

protected Q_SLOTS:
    /// This method is called when an integer value changes
    void onChangeInteger(int value);

    /// This method is called when a boolean value is changes
    void onChangeBoolean(int value);

    /// This method is called when the user clicks OK button
    void onClick();

private:
    /// The signal emitted when the ok button is clicked
    SPTR(TrigerredSignalType) m_sigTriggered;

    /// The signal emitted when an integer value has been changed
    SPTR(IntegerChangedSignalType) m_sigIntegerChanged;

    /// The signal emitted when a boolean value has been changed
    SPTR(BooleanChangedSignalType) m_sigBooleanChanged;

};

}   //namespace uiParameters
}   //namespace editor

#endif /* __UIPARAMETERS_EDITOR_INPUTPARAMETERS_HPP__ */

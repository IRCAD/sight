/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __GUIQT_EDITOR_SPARAMETERS_HPP__
#define __GUIQT_EDITOR_SPARAMETERS_HPP__

#include "guiQt/config.hpp"

#include <gui/editor/IEditor.hpp>

#include <QGridLayout>
#include <QObject>
#include <QPointer>
#include <QPushButton>
#include <QSignalMapper>

namespace guiQt
{

namespace editor
{

/**
 * @brief Generic editor to interact with parameters.
 *
 * It allows to setup an editor with several parameters.
 * Once the user validates the parameters, a signal is sent containing the key and the value.
 * It supports booleans, doubles or integer at the moment.
 *
 * @note This service doesn't need any data.
 *
 * @section Signals Signals
 * - \b boolChanged(bool, std::string): Emitted when an boolean parameter changes.
 * - \b colorChanged(std::array<std::uint8_t, 4>, std::string): Emitted when a color parameter changes.
 * - \b doubleChanged(double, std::string): Emitted when a real parameter changes.
 * - \b double2Changed(double, double, std::string): Emitted when two real parameters change.
 * - \b double3Changed(double, double, double, std::string): Emitted when three real parameters change.
 * - \b intChanged(int, std::string): Emitted when an integer parameter changes.
 * - \b int2Changed(int, int, std::string): Emitted when two integer parameters change.
 * - \b int3Changed(int, int, int, std::string): Emitted when three integer parameters change.
 * - \b enumChanged(std::string, std::string): Emitted when enum parameter changes.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
       <service uid="..." type="::guiQt::editor::SParameters" >
        <parameters>
            <param type="bool" name="boolean parameter" key="boolParam" defaultValue="false" />
            <param type="double" name="real parameter" key="doubleParam" defaultValue="" min="1.5" max="42.42" />
            <param type="int" name="integer parameter" key="intParam" defaultValue="1" min="0" max="255" />
            <param type="enum" name="enum parameters" key="enumParam" defaultValue="p1" values="p1,p2,p3" />
        </parameters>
       </service>
   @endcode
 *
 * @subsection Configuration Configuration:
 * - \b type: bool, color, double, double2, double3, int, int2, int3.
 * - \b name: label to display.
 * - \b key: name used in the signal to identify the parameter.
 * - \b defaultValue: value used to initialize the parameter.
 * - \b min: minimum value, if relevant for the data type.
 * - \b max: maximum value, if relevant for the data type.
 * - \b widget: choose the type of the widget, if relevant for the data type. Currently the only type that provides
 * a choice is 'int' : you can choose either "spin" or "slider".
 * - \b values: list of possible values separated by a comma ',' a space ' ' or a semicolon ';' (only for enum type).
 */
class GUIQT_CLASS_API SParameters : public QObject,
                                    public ::gui::editor::IEditor
{
Q_OBJECT
public:
    fwCoreServiceClassDefinitionsMacro( (SParameters)(::gui::editor::IEditor) );

    /// Boolean changed signal type
    typedef ::fwCom::Signal< void (bool, std::string) > BooleanChangedSignalType;

    /// Color changed signal type
    typedef ::fwCom::Signal< void (std::array<std::uint8_t, 4>, std::string) > ColorChangedSignalType;

    /// Double changed signal type
    typedef ::fwCom::Signal< void (double, std::string) > DoubleChangedSignalType;
    typedef ::fwCom::Signal< void (double, double, std::string) > Double2ChangedSignalType;
    typedef ::fwCom::Signal< void (double, double, double, std::string) > Double3ChangedSignalType;

    /// Integer changed signal type
    typedef ::fwCom::Signal< void (int, std::string) > IntegerChangedSignalType;
    typedef ::fwCom::Signal< void (int, int, std::string) > Integer2ChangedSignalType;
    typedef ::fwCom::Signal< void (int, int, int, std::string) > Integer3ChangedSignalType;

    /// Enum changed signal type
    typedef ::fwCom::Signal< void (std::string, std::string) > EnumChangedSignalType;

    /// Constructor. Initializes signals
    GUIQT_API SParameters() throw ();

    /// Destructor. Does nothing
    GUIQT_API virtual ~SParameters() throw ();

    /// Configure the editor.
    GUIQT_API void configuring() throw (::fwTools::Failed);

    /// Initializes Qt input widgets for parameters according to xml configuration
    GUIQT_API void starting() throw (::fwTools::Failed);

    /// This method launches the IEditor::stopping method
    GUIQT_API void stopping() throw (::fwTools::Failed);

    /// This method is used to update services. Does nothing
    GUIQT_API void updating() throw (::fwTools::Failed);

private Q_SLOTS:

    /// This method is called when a boolean value changes
    void onChangeBoolean(int value);

    /// This method is called when a color button is clicked
    void onColorButton();

    /// This method is called when an integer value changes
    void onChangeInteger(int value);

    /// This method is called when an integer value changes
    void onChangeDouble(double value);

    /// This method is called when selection changes (QComboBox)
    void onChangeEnum(int value);

    /// This method is called to connect sliders and their labels
    void onSliderMapped(QWidget* widget);

    /// This method is called to connect reset buttons and checkboxes
    void onResetBooleanMapped(QWidget* widget);

    /// This method is called to connect reset buttons and color widgets
    void onResetColorMapped(QWidget* widget);

    /// This method is called to connect reset buttons and sliders
    void onResetIntegerMapped(QWidget* widget);

    /// This method is called to connect reset buttons and sliders
    void onResetDoubleMapped(QWidget* widget);

private:

    /// Snippet to create the reset button
    QPushButton* createResetButton();

    /// Create the widget associated with a boolean type
    void createBoolWidget(QGridLayout& layout, int row, const std::string& key, const std::string& defaultValue);

    /// Create the widget associated with a color type
    void createColorWidget(QGridLayout& layout, int row, const std::string& key, const std::string& defaultValue);

    /// Create the widget associated with a double type
    void createDoubleWidget(QGridLayout& layout, int row, const std::string& key, double defaultValue,
                            double min, double max, int count);

    /// Create the slider widget associated with an integer type
    void createIntegerSliderWidget(QGridLayout& layout, int row, const std::string& key,
                                   int defaultValue, int min, int max);

    /// Create the spin widget associated with an integer type
    void createIntegerSpinWidget(QGridLayout& layout, int row, const std::string& key,
                                 int defaultValue, int min, int max, int count);

    /// Create a multi choice widget
    void createEnumWidget(QGridLayout& layout, int row, const std::string& key, const std::string& defaultValue,
                          const std::vector< std::string>& values);

    /// Allows to connect sliders and their labels
    QPointer< QSignalMapper> m_signalMapper;
    QPointer< QSignalMapper> m_resetMapper;
};

}   //namespace guiQt
}   //namespace editor

#endif /* __GUIQT_EDITOR_SPARAMETERS_HPP__ */

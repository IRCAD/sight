/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __UIIMAGEQT_WINDOWLEVEL_HPP__
#define __UIIMAGEQT_WINDOWLEVEL_HPP__

#include "uiImageQt/config.hpp"

#include <fwData/Integer.hpp>

#include <fwDataTools/helper/MedicalImageAdaptor.hpp>

#include <fwTools/Failed.hpp>

#include <gui/editor/IEditor.hpp>

#include <QObject>
#include <QPointer>

class QAction;
class QComboBox;
class QLabel;
class QLineEdit;
class QMenu;
class QSlider;
class QToolButton;
class QSignalMapper;

fwCorePredeclare( (fwGuiQt)(widget)(QRangeSlider) );
namespace fwGuiQt
{
namespace widget
{
class QRangeSlider;
}
}

namespace uiImageQt
{

/**
 * @brief   WindowLevel service allows to change the min / max value of windowing.
 *
 * This is represented by two sliders to modify the min and max values of windowing
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
    <service uid="..." type="::uiImageQt::WindowLevel" autoConnect="yes">
        <inout key="image" uid="..."/>
        <inout key="tf" uid="..." optional="yes" />
        <config autoWindowing="yes" enableSquareTF="no" />
    </service>
   @endcode
 *
 * @subsection In-Out In-Out
 * - \b image [::fwData::Image]: image on which the windowing will be changed
 * - \b tf [::fwData::TransferFunction] (optional): current transfer function to modify, if it is not defined, the
 *      editor uses the default image's transfer function (CT-GreyLevel)
 *
 * @subsection Configuration Configuration
 * - \b autoWindowing(optional, default no) : if 'yes', image windowing will be automatically compute from image pixel
 * min/max
 *  intensity when this service receive BUFFER event
 * - \b enableSquareTF(optional, default: yes) : if 'yes' enable the button to switch between current TF and square TF
 */
class UIIMAGEQT_CLASS_API WindowLevel : public QObject,
                                        public ::fwDataTools::helper::MedicalImageAdaptor,
                                        public ::gui::editor::IEditor
{
Q_OBJECT

public:

    fwCoreServiceClassDefinitionsMacro( (WindowLevel)(::gui::editor::IEditor) );

    /// Constructor. Do nothing.
    UIIMAGEQT_API WindowLevel() noexcept;

    /// Destructor. Do nothing.
    UIIMAGEQT_API virtual ~WindowLevel() noexcept;

protected:

    /**
     * @brief Install the layout.
     */
    virtual void starting();

    /**
     * @brief Destroy the layout.
     */
    virtual void stopping();

    /// Update editor information from the image
    virtual void updating();

    /// Select the current tf
    virtual void swapping(const KeyType& key);

    /// Parse the xml configuration
    virtual void configuring();

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect Image::s_MODIFIED_SIG to this::s_UPDATE_SLOT
     * Connect Image::s_BUFFER_MODIFIED_SIG to this::s_UPDATE_SLOT
     */
    UIIMAGEQT_API virtual KeyConnectionsMap getAutoConnections() const;

    /// Overrides
    UIIMAGEQT_API virtual void info( std::ostream& _sstream );

    virtual void setEnabled(bool enable);

    /// Called when transfer function points are modified.
    UIIMAGEQT_API virtual void updatingTFPoints();

    /// Called when transfer function windowing is modified.
    UIIMAGEQT_API virtual void updatingTFWindowing(double window, double level);

protected Q_SLOTS:

    void onTextEditingFinished();
    void onToggleTF(bool squareTF);
    void onToggleAutoWL(bool autoWL);

    void onWindowLevelWidgetChanged(double _min, double _max);
    void onDynamicRangeSelectionChanged(QAction* action);

protected:
    typedef ::fwData::TransferFunction::TFValuePairType WindowLevelMinMaxType;

    double toWindowLevel(double _val);
    double fromWindowLevel(double _val);

    WindowLevelMinMaxType getImageWindowMinMax();

    void onImageWindowLevelChanged(double _imageMin, double _imageMax);

    void updateWidgetMinMax(double _imageMin, double _imageMax);
    void updateImageWindowLevel(double _imageMin, double _imageMax);
    void updateTextWindowLevel(double _imageMin, double _imageMax);

    void setWidgetDynamicRange(double min, double max);
    bool getWidgetDoubleValue(QLineEdit* widget, double& val);

private:

    QPointer< QLineEdit >   m_valueTextMin;
    QPointer< QLineEdit >   m_valueTextMax;
    QPointer< QToolButton > m_toggleTFButton;
    QPointer< QToolButton > m_toggleAutoButton;
    QPointer< QToolButton >   m_dynamicRangeSelection;
    QPointer< QMenu >         m_dynamicRangeMenu;
    QPointer< QSignalMapper > m_dynamicRangeSignalMapper;

    QPointer< ::fwGuiQt::widget::QRangeSlider > m_rangeSlider;

    double m_widgetDynamicRangeMin;
    double m_widgetDynamicRangeWidth;
    bool m_autoWindowing;
    bool m_enableSquareTF;

    /// Store previous TF, used in onToggleTF() to restore this TF when switching to the square TF
    ::fwData::TransferFunction::sptr m_previousTF;
};

} // uiImageQt

#endif /*__UIIMAGEQT_WINDOWLEVEL_HPP__*/

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
        <inout key="TFSelections" uid="..."/>
        <config autoWindowing="yes" selectedTFKey="mySelectedTF" useImageGreyLevelTF="yes" />
    </service>
   @endcode
 *
 * @subsection In-Out In-Out
 * - \b image [::fwData::Image]: image on which the windowing will be changed
 * - \b TFSelections [::fwData::Composite]: composite containing the selected transfer function
 *
 * @subsection Configuration Configuration
 *  - \b autoWindowing : if 'yes', image windowing will be automatically compute from image pixel min/max
 *  intensity when this service receive BUFFER event
 *  - \b tfSelection : configure the identifier of the field containing the specific TF selection. By default, it use
 * default selection field.
 *  - \b useImageGreyLevelTF : if 'yes' and if tfSelection is configured, then we use the grey level tf of image
 *
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

    /// Swap of image
    virtual void swapping();

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

    /// Returns the current grey level tf of image
    ::fwData::TransferFunction::sptr getImageGreyLevelTF();

    /// Swap current tf and notify other services
    void swapCurrentTFAndNotify( ::fwData::TransferFunction::sptr newTF );

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
    double m_notifiedImageMin;
    double m_notifiedImageMax;
    bool m_autoWindowing;
    bool m_useImageGreyLevelTF;

    /// Identifier of the field containing the specific selection of TransferFunction
    /// if m_tfSelection is empty => use default TF selection
    std::string m_tfSelection;

};

} // uiImageQt

#endif /*__UIIMAGEQT_WINDOWLEVEL_HPP__*/

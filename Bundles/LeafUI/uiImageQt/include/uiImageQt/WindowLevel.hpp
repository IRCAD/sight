/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _UIIMAGEQT_WINDOWLEVEL_HPP_
#define _UIIMAGEQT_WINDOWLEVEL_HPP_

#include <QObject>
#include <QPointer>

#include <fwTools/Failed.hpp>
#include <fwData/Integer.hpp>
#include <gui/editor/IEditor.hpp>

#include <fwComEd/helper/MedicalImageAdaptor.hpp>

#include "uiImageQt/config.hpp"

class QAction;
class QComboBox;
class QLabel;
class QLineEdit;
class QMenu;
class QSlider;
class QToolButton;
class QSignalMapper;

fwCorePredeclare( (fwGuiQt)(widget)(QRangeSlider) );

namespace uiImage
{

/**
 * @brief   WindowLevel service allows to change the min / max value of windowing.
 * @class   WindowLevel.
 * @author  IRCAD (Research and Development Team).
 * @date    2010-2011.
 *
 * This is represented by
 *  - two sliders to modify the min, max value of windowing
 */
class UIIMAGEQT_CLASS_API WindowLevel : public QObject, public ::fwComEd::helper::MedicalImageAdaptor, public ::gui::editor::IEditor
{
    Q_OBJECT

public :


    fwCoreServiceClassDefinitionsMacro ( (WindowLevel)(::gui::editor::IEditor) ) ;

    /// Constructor. Do nothing.
    UIIMAGEQT_API WindowLevel() throw() ;

    /// Destructor. Do nothing.
    UIIMAGEQT_API virtual ~WindowLevel() throw() ;

    UIIMAGEQT_API void notifyWindowLevelCallback();

protected:

    /**
     * @brief Install the layout.
     */
    virtual void starting() throw(::fwTools::Failed);

    /**
     * @brief Destroy the layout.
     */
    virtual void stopping() throw(::fwTools::Failed);

    /// Management of observations : update editor according to the received message
    virtual void updating( ::boost::shared_ptr< const fwServices::ObjectMsg > _msg ) throw(::fwTools::Failed);

    /// Update editor information from the image
    virtual void updating() throw(::fwTools::Failed);

    /// Swap of image
    virtual void swapping() throw(::fwTools::Failed);

    /**
     * @brief Configure the editor.
     *
     * Example of configuration
     * @verbatim
         <service uid="windowLevel" implementation="::uiImage::WindowLevel" type="::gui::editor::IEditor" autoComChannel="yes">
             <config autoWindowing="yes" selectedTFKey="mySelectedTF" tfPoolFwID="myTFPool" />
         </service>
       @endverbatim
     * With :
     *  - \b autoWindowing : if 'yes', image windowing will be automatically compute from image pixel min/max
     *  intensity when this service receive BUFFER event
     *  - \b tfSelection : configure the identifier of the field containing the specific TF selection. By default, it use default selection field.
     */
    virtual void configuring() throw(fwTools::Failed);

    /// Overrides
    UIIMAGEQT_API virtual void info( std::ostream &_sstream ) ;

    virtual void setEnabled(bool enable);


protected slots :

    void onTextEditingFinished();
    void onToggleTF(bool squareTF);
    void onToggleAutoWL(bool autoWL);

    void onWindowLevelWidgetChanged(double _min, double _max);
    void onDynamicRangeSelectionChanged(QAction *action);

protected:
    typedef ::fwData::TransfertFunction_VERSION_II::TFValuePairType WindowLevelMinMaxType;

    double toWindowLevel(double _val);
    double fromWindowLevel(double _val);

    WindowLevelMinMaxType getImageWindowMinMax();

    void onImageWindowLevelChanged(double _imageMin, double _imageMax);
    void notifyWindowLevel(double _imageMin, double _imageMax);

    void updateWidgetMinMax(double _imageMin, double _imageMax);
    void updateImageWindowLevel(double _imageMin, double _imageMax);
    void updateTextWindowLevel(double _imageMin, double _imageMax);

    void setWidgetDynamicRange(double min, double max);
    bool getWidgetDoubleValue(QLineEdit *widget, double &val);

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
    double m_imageMin;
    double m_imageMax;
    double m_notifiedImageMin;
    double m_notifiedImageMax;
    bool m_isNotifying;
    bool m_autoWindowing;

    /// Identifier of the field containing the specific selection of TransfertFunction
    /// if m_tfSelection is empty => use default TF selection
    std::string m_tfSelection;

};

} // uiImage

#endif /*_UIIMAGEQT_WINDOWLEVEL_HPP_*/



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

#include "uiImageQt/config.hpp"

class QLineEdit;
class QSlider;

fwCorePredeclare( (fwGuiQt)(widget)(QRangeSlider) );

namespace uiImage
{

/**
 * @brief   WindowLevel service allows to change the min / max value of windowing.
 * @class   WindowLevel.
 * @author  IRCAD (Research and Development Team).
 * @date    2010.
 *
 * This is represented by
 *  - two sliders to modify the min, max value of windowing
 */
class UIIMAGEQT_CLASS_API WindowLevel : public QObject, public ::gui::editor::IEditor
{
    Q_OBJECT

public :


    fwCoreServiceClassDefinitionsMacro ( (WindowLevel)(::gui::editor::IEditor) ) ;

    /// Constructor. Do nothing.
    UIIMAGEQT_API WindowLevel() throw() ;

    /// Destructor. Do nothing.
    UIIMAGEQT_API virtual ~WindowLevel() throw() ;

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
         <service uid="windowLevel" implementation="::uiImage::WindowLevel" type="::gui::editor::IEditor" autoComChannel="yes"/>
       @endverbatim
     */
    virtual void configuring() throw(fwTools::Failed);

    /// Overrides
    UIIMAGEQT_API virtual void info( std::ostream &_sstream ) ;

    virtual void setEnabled(bool enable);


protected slots :

    void onMinTextChanged(QString strVal);
    void onMaxTextChanged(QString strVal);

    void onWindowLevelWidgetChanged(double _min, double _max);

protected:
    int toWindowLevel(double _val);
    double fromWindowLevel(int _val);

    void setWidgetWindowMinMax(int _imageMin, int _imageMax);

    void updateWindowLevel(double _min, double _max);

    void onImageWindowLevelChanged(int _imageMin, int _imageMax);
    void notifyWindowLevel(int _imageMin, int _imageMax);
    void updateWindowLevelText(int _imageMin, int _imageMax);

    bool onTextChanged(QLineEdit *widget, QString &strVal, int &val);

private:

    //QPointer< QSlider > m_sliceSelectorMin;
    //QPointer< QSlider > m_sliceSelectorMax;
    QPointer< QLineEdit >   m_valueTextMin;
    QPointer< QLineEdit >   m_valueTextMax;

    QPointer< ::fwGuiQt::widget::QRangeSlider > m_rangeSlider;

    double m_imageDynamicMin;
    double m_imageDynamicWidth;


};

} // uiImage

#endif /*_UIIMAGEQT_WINDOWLEVEL_HPP_*/



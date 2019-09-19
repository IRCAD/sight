/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#pragma once

#include "uiImageQt/config.hpp"

#include <fwData/Integer.hpp>

#include <fwDataTools/helper/TransferFunction.hpp>

#include <fwGui/editor/IEditor.hpp>

#include <fwTools/Failed.hpp>

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
 * - \b tf [::fwData::TransferFunction] (optional): the current TransferFunction. If it is not defined, we use the
 *      image's default transferFunction (CT-GreyLevel).
 *
 * @subsection Configuration Configuration
 * - \b autoWindowing(optional, default no) : if 'yes', image windowing will be automatically compute from image pixel
 * min/max intensity when this service receive BUFFER event
 * - \b enableSquareTF(optional, default: yes) : if 'yes' enable the button to switch between current TF and square TF
 */
class UIIMAGEQT_CLASS_API WindowLevel : public QObject,
                                        public ::fwGui::editor::IEditor
{

Q_OBJECT

public:

    fwCoreServiceMacro(WindowLevel, ::fwGui::editor::IEditor);

    /// Constructor. Do nothing.
    UIIMAGEQT_API WindowLevel() noexcept;

    /// Destructor. Do nothing.
    UIIMAGEQT_API virtual ~WindowLevel() noexcept;

protected:

    /// Install the layout.
    virtual void starting() override;

    ///Destroy the layout.
    virtual void stopping() override;

    /// Update editor information from the image.
    virtual void updating() override;

    /// Select the current tf
    virtual void swapping(const KeyType& key) override;

    /// Parse the xml configuration
    virtual void configuring() override;

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect Image::s_MODIFIED_SIG to this::s_UPDATE_SLOT
     * Connect Image::s_BUFFER_MODIFIED_SIG to this::s_UPDATE_SLOT
     */
    UIIMAGEQT_API virtual KeyConnectionsMap getAutoConnections() const override;

    /// Overrides
    UIIMAGEQT_API virtual void info( std::ostream& _sstream ) override;

    /// Slot: Updates the slider position
    UIIMAGEQT_API virtual void updateTF();

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

    void onImageWindowLevelChanged(double _imageMin, double _imageMax);

    void updateWidgetMinMax(double _imageMin, double _imageMax);

    void updateImageWindowLevel(double _imageMin, double _imageMax);

    void updateTextWindowLevel(double _imageMin, double _imageMax);

    WindowLevelMinMaxType getImageWindowMinMax();

    bool getWidgetDoubleValue(QLineEdit* widget, double& val);

    void setWidgetDynamicRange(double min, double max);

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

    ::fwDataTools::helper::TransferFunction m_helperTF;
};

} // uiImageQt

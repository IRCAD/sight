/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "modules/ui/qt/config.hpp"

#include <core/tools/Failed.hpp>

#include <data/helper/TransferFunction.hpp>
#include <data/Integer.hpp>

#include <ui/base/IEditor.hpp>

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

namespace sight::ui::qt
{

namespace widget
{

class QRangeSlider;

}

}

namespace sight::module::ui::qt::image
{

/**
 * @brief WindowLevel service allows to change the min/max value of windowing.
 *
 * This is represented by two sliders to modify the min and max values of windowing.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
    <service uid="..." type="sight::module::ui::qt::image::WindowLevel" autoConnect="true">
        <inout key="image" uid="..."/>
        <inout key="tf" uid="..." optional="true" />
        <config autoWindowing="yes" enableSquareTF="no" />
    </service>
   @endcode
 *
 * @subsection In-Out In-Out
 * - \b image [sight::data::Image]: image on which the windowing will be changed.
 * - \b tf [sight::data::TransferFunction] (optional): the current TransferFunction. If it is not defined, we use the
 *      image's default transferFunction (CT-GreyLevel).
 *
 * @subsection Configuration Configuration
 * - \b autoWindowing(optional, default="no"): if 'yes', image windowing will be automatically compute from image pixel
 * min/max intensity when this service receive BUFFER event.
 * - \b enableSquareTF(optional, default="yes"): if 'yes', enables the button to switch between current TF and square
 * TF.
 */
class MODULE_UI_QT_CLASS_API WindowLevel final : public QObject,
                                                 public sight::ui::base::IEditor
{
Q_OBJECT

public:

    SIGHT_DECLARE_SERVICE(WindowLevel, sight::ui::base::IEditor);

    /// Initialize signals and slots.
    MODULE_UI_QT_API WindowLevel() noexcept;

    /// Destroys the service.
    MODULE_UI_QT_API virtual ~WindowLevel() noexcept;

protected:

    /// Configures the service.
    void configuring() final;

    /// Installs the layout.
    void starting() final;

    /// Updates editor information from the image.
    void updating() final;

    /// Destroys the layout.
    void stopping() final;

    /// Selects the current tf.
    void swapping(const KeyType&) final;

    /**
     * @brief Proposals to connect service slots to associated object signals.
     * @return A map of each proposed connection.
     *
     * Connect data::Image::s_MODIFIED_SIG to module::ui::qt::image::WindowLevel::s_UPDATE_SLOT
     * Connect Image::s_BUFFER_MODIFIED_SIG to module::ui::qt::image::WindowLevel::s_UPDATE_SLOT
     */
    MODULE_UI_QT_API KeyConnectionsMap getAutoConnections() const final;

    /**
     * @brief Adds informations about this service into the stream.
     * @param _sstream Stream where stores information.
     */
    MODULE_UI_QT_API void info(std::ostream& _sstream) final;

    /// Slot: Updates the slider position
    MODULE_UI_QT_API virtual void updateTF();

protected Q_SLOTS:

    void onTextEditingFinished();

    void onToggleTF(bool squareTF);

    void onToggleAutoWL(bool autoWL);

    void onWindowLevelWidgetChanged(double _min, double _max);

    void onDynamicRangeSelectionChanged(QAction* action);

protected:

    typedef data::TransferFunction::TFValuePairType WindowLevelMinMaxType;

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

    QPointer<QLineEdit> m_valueTextMin;
    QPointer<QLineEdit> m_valueTextMax;
    QPointer<QToolButton> m_toggleTFButton;
    QPointer<QToolButton> m_toggleAutoButton;
    QPointer<QToolButton> m_dynamicRangeSelection;
    QPointer<QMenu> m_dynamicRangeMenu;
    QPointer<QSignalMapper> m_dynamicRangeSignalMapper;

    QPointer<sight::ui::qt::widget::QRangeSlider> m_rangeSlider;

    double m_widgetDynamicRangeMin;
    double m_widgetDynamicRangeWidth;
    bool m_autoWindowing;
    bool m_enableSquareTF;

    /// Store previous TF, used in onToggleTF() to restore this TF when switching to the square TF
    data::TransferFunction::sptr m_previousTF;

    data::helper::TransferFunction m_helperTF;
};

} // uiImageQt

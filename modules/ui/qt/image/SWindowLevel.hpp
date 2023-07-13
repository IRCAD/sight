/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include <data/Image.hpp>
#include <data/Integer.hpp>
#include <data/TransferFunction.hpp>

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

namespace sight::ui::qt::widget
{

class QRangeSlider;

} // namespace sight::ui::qt::widget

namespace sight::module::ui::qt::image
{

/**
 * @brief SWindowLevel service allows to change the min/max value of windowing.
 *
 * This is represented by two sliders to modify the min and max values of windowing.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
    <service uid="..." type="sight::module::ui::qt::image::SWindowLevel" autoConnect="true">
        <inout key="image" uid="..."/>
        <inout key="tf" uid="..." />
        <config autoWindowing="true" enableSquareTF="false" />
    </service>
   @endcode
 *
 * @subsection In-Out In-Out
 * - \b image [sight::data::Image]: image on which the windowing will be changed.
 * - \b tf [sight::data::TransferFunction] (optional): the current TransferFunction. If it is not defined, we use the
 *      image's default transferFunction (CT-GreyLevel).
 *
 * @subsection Configuration Configuration
 * - \b minimal(optional, default="false"): if 'true', only the windowing range slider is shown
 * - \b autoWindowing(optional, default="false"): if 'true', image windowing will be automatically compute from image
 * pixel
 * min/max intensity when this service receive BUFFER event.
 * - \b enableSquareTF(optional, default="true"): if 'true', enables the button to switch between current TF and square
 * TF.
 */
class MODULE_UI_QT_CLASS_API SWindowLevel final : public QObject,
                                                  public sight::ui::base::IEditor
{
Q_OBJECT

public:

    SIGHT_DECLARE_SERVICE(SWindowLevel, sight::ui::base::IEditor);

    /// Initialize signals and slots.
    MODULE_UI_QT_API SWindowLevel() noexcept;

    /// Destroys the service.
    MODULE_UI_QT_API ~SWindowLevel() noexcept override;

protected:

    /// Configures the service.
    void configuring() final;

    /// Installs the layout.
    void starting() final;

    /// Updates editor information from the image.
    void updating() final;

    /// Destroys the layout.
    void stopping() final;

    /**
     * @brief Proposals to connect service slots to associated object signals.
     * @return A map of each proposed connection.
     *
     * Connect data::Image::s_MODIFIED_SIG to module::ui::qt::image::SWindowLevel::IService::slots::s_UPDATE
     * Connect Image::s_BUFFER_MODIFIED_SIG to module::ui::qt::image::SWindowLevel::IService::slots::s_UPDATE
     */
    MODULE_UI_QT_API KeyConnectionsMap getAutoConnections() const final;

    /**
     * @brief Adds informations about this service into the stream.
     * @param _sstream Stream where stores information.
     */
    MODULE_UI_QT_API void info(std::ostream& _sstream) final;

    /// Slot: Updates the slider position
    MODULE_UI_QT_API void updateTF();

protected Q_SLOTS:

    void onTextEditingFinished();

    void onToggleTF(bool squareTF);

    void onToggleAutoWL(bool autoWL);

    void onWindowLevelWidgetChanged(double _min, double _max);

    void onDynamicRangeSelectionChanged(QAction* action);

protected:

    typedef data::TransferFunction::min_max_t WindowLevelMinMaxType;

    double toWindowLevel(double _val) const;

    double fromWindowLevel(double _val);

    void onImageWindowLevelChanged(double _imageMin, double _imageMax);

    void updateWidgetMinMax(double _imageMin, double _imageMax);

    void updateImageWindowLevel(double _imageMin, double _imageMax);

    void updateTextWindowLevel(double _imageMin, double _imageMax);

    WindowLevelMinMaxType getImageWindowMinMax();

    static bool getWidgetDoubleValue(QLineEdit* widget, double& val);

    void setWidgetDynamicRange(double min, double max);

private:

    QPointer<QLineEdit> m_valueTextMin;
    QPointer<QLineEdit> m_valueTextMax;
    QPointer<QToolButton> m_toggleTFButton;
    QPointer<QToolButton> m_toggleAutoButton;
    QPointer<QToolButton> m_dynamicRangeSelection;
    QPointer<QMenu> m_dynamicRangeMenu;

    QPointer<sight::ui::qt::widget::QRangeSlider> m_rangeSlider;

    double m_widgetDynamicRangeMin {-1024};
    double m_widgetDynamicRangeWidth {4000};
    bool m_minimal {false};
    bool m_autoWindowing {false};
    bool m_enableSquareTF {true};

    /// Store previous TF, used in onToggleTF() to restore this TF when switching to the square TF
    data::TransferFunction::sptr m_previousTF;

    static constexpr std::string_view s_IMAGE = "image";
    static constexpr std::string_view s_TF    = "tf";

    data::ptr<data::Image, data::Access::inout> m_image {this, s_IMAGE, true};
    data::ptr<data::TransferFunction, data::Access::inout> m_tf {this, s_TF, true};
};

} // namespace sight::module::ui::qt::image

/************************************************************************
 *
 * Copyright (C) 2017-2021 IRCAD France
 * Copyright (C) 2017-2019 IHU Strasbourg
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

#include <data/Matrix4.hpp>

#include <ui/base/IEditor.hpp>

#include <QObject>
#include <QPointer>

class QSlider;
class QLabel;
class QLineEdit;

namespace sight::module::ui::qt::viz
{

/**
 * @brief   This editor regulates the position and rotation defined in a transformation matrix.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
    <service uid="..." type="sight::module::ui::qt::viz::STransformEditor" autoConnect="false">
        <inout key="matrix" uid="..."/>
        <translation enabled="false" min="-300"/>
        <rotation enabled="true" min="-180" max="180" />
    </service>
   @endcode
 *
 * @subsection In-Out In-Out
 * - \b matrix [sight::data::Matrix4]: matrix modified by the editor
 *
 * @subsection Configuration Configuration
 * - \b enabled (optional): enable/disable rotation/translation edition.
 *      Can be 'true', 'false' or a combination of [xyz] (default: true).
 * - \b min (optional): set the minimum value for translation/rotation (default: translation=-300, rotation=-180 ).
 * - \b max (optional): set the maximum value for translation/rotation (default: translation=+300, rotation=180).
 */
class MODULE_UI_QT_CLASS_API STransformEditor : public QObject,
                                                public sight::ui::base::IEditor
{
Q_OBJECT;

public:

    SIGHT_DECLARE_SERVICE(STransformEditor, sight::ui::base::IEditor);

    /// Constructor. Do nothing.
    MODULE_UI_QT_API STransformEditor() noexcept;

    /// Destructor. Do nothing.
    MODULE_UI_QT_API virtual ~STransformEditor() noexcept;

protected:

    /// This method is used to configure the service parameters:
    MODULE_UI_QT_API void configuring() override;

    ///This method launches the sight::ui::base::IGuiContainer::create method.
    MODULE_UI_QT_API void starting() override;

    ///This method launches the sight::ui::base::IGuiContainer::destroy method.
    MODULE_UI_QT_API void stopping() override;

    /// Updates Slider value
    MODULE_UI_QT_API void updating() override;

    // Connect data::Matrix4::s_MODIFIED_SIG to update slot
    MODULE_UI_QT_API KeyConnectionsMap getAutoConnections() const override;

private Q_SLOTS:

    /// Slot called when slider value changed.
    void onSliderChanged(int value);

    /// Slot called when line edit value changed.
    void onTextChanged();

private:

    /// Update the editor when the matrix changes
    void updateFromMatrix();

    /*
     * @brief This enum defines the transformation matrix entries indexes
     */
    enum SliderIndex
    {
        POSITION_X = 0,
        POSITION_Y,
        POSITION_Z,
        ROTATION_X,
        ROTATION_Y,
        ROTATION_Z,
        MAX_SLIDER_INDEX
    };

    /*
     * @brief This struct regulates a transformation matrix entry in the editor
     */
    struct SliderWidget
    {
        QPointer<QSlider> m_slider;         ///< Slider to change coefficient value.
        QPointer<QLabel> m_labelMin;        ///< Label to show the min value.
        QPointer<QLabel> m_labelMax;        ///< Label to show the max value.
        QPointer<QLabel> m_labelDefinition; ///< Label to show the coefficient description.
        QPointer<QLineEdit> m_sliderValue;  ///< Editor to show the current value of the slider.
    };

    /// Array containing the different structs to regulate the transformation matrix entries.
    SliderWidget m_sliders[MAX_SLIDER_INDEX];

    /// Contains a string identifying which axes [xyz] are displayed for rotation
    std::string m_rotation;

    /// Contains a string identifying which axes [xyz] are displayed for translation
    std::string m_translation;

    /// Range of translation sliders
    int m_translationRange[2];

    /// Range of rotation sliders
    int m_rotationRange[2];

    static const service::key_t s_MATRIX_INOUT;
    data::ptr<data::Matrix4, sight::data::Access::inout> m_matrix {this, s_MATRIX_INOUT, true};
};

} // namespace sight::module::ui::qt::viz

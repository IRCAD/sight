/************************************************************************
 *
 * Copyright (C) 2017-2024 IRCAD France
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

#include <data/matrix4.hpp>

#include <ui/__/editor.hpp>

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
    <service uid="..." type="sight::module::ui::qt::viz::transform_editor" auto_connect="false">
        <inout key="matrix" uid="..."/>
        <translation enabled="false" min="-300"/>
        <rotation enabled="true" min="-180" max="180" />
    </service>
   @endcode
 *
 * @subsection In-Out In-Out
 * - \b matrix [sight::data::matrix4]: matrix modified by the editor
 *
 * @subsection Configuration Configuration
 * - \b enabled (optional): enable/disable rotation/translation edition.
 *      Can be 'true', 'false' or a combination of [xyz] (default: true).
 * - \b min (optional): set the minimum value for translation/rotation (default: translation=-300, rotation=-180 ).
 * - \b max (optional): set the maximum value for translation/rotation (default: translation=+300, rotation=180).
 */
class transform_editor : public QObject,
                         public sight::ui::editor
{
Q_OBJECT;

public:

    SIGHT_DECLARE_SERVICE(transform_editor, sight::ui::editor);

    /// Constructor. Do nothing.
    transform_editor() noexcept;

    /// Destructor. Do nothing.
    ~transform_editor() noexcept override;

protected:

    /// This method is used to configure the service parameters:
    void configuring() override;

    ///This method launches the sight::ui::service::create method.
    void starting() override;

    ///This method launches the sight::ui::service::destroy method.
    void stopping() override;

    /// Updates Slider value
    void updating() override;

    // Connect data::matrix4::MODIFIED_SIG to update slot
    connections_t auto_connections() const override;

private Q_SLOTS:

    /// Slot called when slider value changed.
    void on_slider_changed(int _value);

    /// Slot called when line edit value changed.
    void on_text_changed();

private:

    /// Update the editor when the matrix changes
    void update_from_matrix();

    /*
     * @brief This enum defines the transformation matrix entries indexes
     */
    enum slider_index
    {
        position_x = 0,
        position_y,
        position_z,
        rotation_x,
        rotation_y,
        rotation_z,
        max_slider_index
    };

    /*
     * @brief This struct regulates a transformation matrix entry in the editor
     */
    struct slider_widget
    {
        QPointer<QSlider> m_slider;          ///< Slider to change coefficient value.
        QPointer<QLabel> m_label_min;        ///< Label to show the min value.
        QPointer<QLabel> m_label_max;        ///< Label to show the max value.
        QPointer<QLabel> m_label_definition; ///< Label to show the coefficient description.
        QPointer<QLineEdit> m_slider_value;  ///< Editor to show the current value of the slider.
    };

    /// Array containing the different structs to regulate the transformation matrix entries.
    std::array<slider_widget, max_slider_index> m_sliders;

    /// Contains a string identifying which axes [xyz] are displayed for rotation
    std::string m_rotation;

    /// Contains a string identifying which axes [xyz] are displayed for translation
    std::string m_translation;

    /// Range of translation sliders
    std::array<int, 2> m_translation_range {};

    /// Range of rotation sliders
    std::array<int, 2> m_rotation_range {};

    static constexpr std::string_view MATRIX_INOUT = "matrix";
    data::ptr<data::matrix4, sight::data::access::inout> m_matrix {this, MATRIX_INOUT};
};

} // namespace sight::module::ui::qt::viz

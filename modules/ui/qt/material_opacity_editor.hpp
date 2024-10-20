/************************************************************************
 *
 * Copyright (C) 2021-2024 IRCAD France
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

#include <data/material.hpp>

#include <ui/__/editor.hpp>

#include <QObject>
#include <QPointer>

class QSlider;
class QLabel;

namespace sight::module::ui::qt
{

/**
 * @brief Adds an opacity editor widget (Slider) applied on a data::material.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="sight::module::ui::qt::material_opacity_editor">
       <inout key="material" uid="..." />
       <label>Object's opacity: </label>
   </service>
   @endcode
 *
 *@subsection In-Out In-Out
 * - \b material [sight::data::material]: material object to update.
 *
 *@subsection Configuration Configuration
 * - \b label (optional, default="Material opacity : "): label of the slider.
 */

class material_opacity_editor : public QObject,
                                public sight::ui::editor
{
Q_OBJECT

public:

    SIGHT_DECLARE_SERVICE(material_opacity_editor, sight::ui::editor);

    /// Destroys the service.
    ~material_opacity_editor() noexcept override = default;

private:

    /// Configures the slider's label.
    void configuring() override;

    /// Sets the connections and the UI elements.
    void starting() override;

    /// Sets the opacity slider's value to the material's alpha value.
    void updating() override;

    /// Destroys the connections and cleans the container.
    void stopping() override;

    QPointer<QSlider> m_opacity_slider;
    QPointer<QLabel> m_opacity_value;

    /// Name that appears next to the slider.
    std::string m_label {"Material opacity : "};

    data::ptr<data::material, data::access::inout> m_material {this, "material"};

private Q_SLOTS:

    /**
     * @brief Slot: called when the opacity slider changed.
     * @param _value The new opacity value.
     */
    void on_opacity_slider(int _value);
};

} // namespace sight::module::ui::qt

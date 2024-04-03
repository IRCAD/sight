/************************************************************************
 *
 * Copyright (C) 2018-2024 IRCAD France
 * Copyright (C) 2018-2019 IHU Strasbourg
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

#include <data/reconstruction.hpp>

#include <ui/qml/editor.hpp>

#include <QColor>
#include <QObject>

namespace sight::module::ui::qml::reconstruction
{

/**
 * @brief Display a widget to change the reconstruction material (color and transparency).
 *
 * @section QSignal Qt Signals
 * - \b materialChanged(QColor, int): emitted when the material changed (color, opacity)
 *
 * @section QSlots Qt Slots
 * - \b onOpacitySlider(int): called when the opacity changed, it will update the reconstruction Material
 * - \b onColor(QColor): called when the opacity changed, it will update the reconstruction Material

 *
 * @section QML Qml Configuration
 *
 * @code{.qml}
    organ_material_editor {
        id: organMaterialService

        onStarted: {
            organMaterialEditor.enabled = true
        }
        onStopped: {
            organMaterialEditor.enabled = false
        }

        onMaterialChanged: {
            colorDialog.setColor(color)
            transparencySlider.value = opacity
        }
    }
   @endcode
 *
 * @section Objects Required objects
 *
 * @subsection In-Out In-Out
 * - \b reconstruction [sight::data::reconstruction]: reconstruction containing the material to update.
 */
class organ_material_editor : public sight::ui::qml::editor
{
Q_OBJECT

public:

    SIGHT_DECLARE_SERVICE(organ_material_editor, sight::ui::qml::editor);

    /// Constructor. Do nothing.
    organ_material_editor() noexcept;

    /// Destructor. Do nothing.
    ~organ_material_editor() noexcept override;

Q_SIGNALS:

    void material_changed(QColor _color, int _opacity);

protected:

    /// Call editor::starting
    void starting() override;

    /// Call editor::stopping
    void stopping() override;

    /// Emit a signal to update the Qml ui with the material information
    void updating() override;

    /// Do nothing
    void configuring() override;

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect Reconstruction::MODIFIED_SIG to this::service::slots::UPDATE
     */
    connections_t auto_connections() const override;

protected Q_SLOTS:

    /// Called when the opacity changed, it will update the reconstruction Material
    void on_opacity_slider(int _value);

    /// Called when the color changed, it will update the reconstruction Material
    void on_color(QColor _color);

private:

    /// Notify the material changes
    void material_notification();

    static constexpr std::string_view RECONSTRUCTION_INOUT = "reconstruction";
    data::ptr<data::reconstruction, data::access::inout> m_rec {this, RECONSTRUCTION_INOUT, true};
};

} // namespace sight::module::ui::qml::reconstruction

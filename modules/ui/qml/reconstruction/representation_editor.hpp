/************************************************************************
 *
 * Copyright (C) 2018-2023 IRCAD France
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

#include "modules/ui/qml/config.hpp"

#include <core/com/signal.hpp>
#include <core/com/signals.hpp>

#include <data/material.hpp>
#include <data/reconstruction.hpp>

#include <ui/qml/editor.hpp>

#include <QObject>

namespace sight::module::ui::qml::reconstruction
{

/**
 * @brief Display a widget to change the reconstruction representation (surface, point, edge, ...).
 *
 * @section QSignal Qt Signals
 * - \b materialChanged(int, int, int): emitted when the material changed (representation, shading, option)
 *
 * @section QSlots Qt Slots
 * - \b onChangeRepresentation(int): called when the organ representation changed, it will update the reconstruction
 * Material (1: SURFACE, 2: POINT, 3: WIREFRAME, 4: EDGE)
 * - \b onChangeShading(int): called when the opacity changed, it will update the reconstruction Material (0: AMBIENT,
 * 1: FLAT, 2: PHONG)
 * - \b onShowNormals(int): called when the opacity changed, it will update the reconstruction Material (1: STANDARD,
 * 2: NORMALS, 3: CELLS_NORMALS)
 *
 * @section QML Qml Configuration
 *
 * @code{.qml}
    representation_editor {
        id: representationService

        // @disable-check M16
        onStarted: {
            representationEditor.enabled = true
        }

        // @disable-check M16
        onStopped: {
            representationEditor.enabled = false
        }

        // @disable-check M16
        onMaterialChanged: {
            switch (representationMode) {
                // ...
            }

            switch (shadingMode) {
                // ...
            }

            switch (optionMode) {
                // ...
            }
        }
    }
   @endcode
 *
 * @section Objects Required objects
 * @subsection In-Out In-Out
 * - \b reconstruction [sight::data::reconstruction]: reconstruction that will be updated
 */
class MODULE_UI_QML_CLASS_API representation_editor : public sight::ui::qml::editor
{
Q_OBJECT

public:

    SIGHT_DECLARE_SERVICE(representation_editor, sight::ui::qml::editor);

    /// Constructor. Do nothing.
    MODULE_UI_QML_API representation_editor() noexcept;

    /// Destructor. Do nothing.
    MODULE_UI_QML_API ~representation_editor() noexcept override;

Q_SIGNALS:

    void material_changed(int _representation_mode, int _shading_mode, int _option_mode);

protected:

    /// Call editor::starting
    void starting() override;

    /// Call editor::stopping
    void stopping() override;

    /// Emit a signal to update the Qml ui with the material information
    void updating() override;

    /// Do nothing.
    void configuring() override;

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect Reconstruction::MODIFIED_SIG to this::service::slots::UPDATE
     */
    connections_t auto_connections() const override;

    /// Notify the changes
    void notify_material();

protected Q_SLOTS:

    /// Called when the organ representation changed, it will update the reconstruction Material
    void on_change_representation(int _id);
    /// Called when the organ shading changed, it will update the reconstruction Material
    void on_change_shading(int _id);
    /// Called when the organ normal option changed, it will update the reconstruction Material
    void on_show_normals(int _state);

private:

    data::material::sptr m_material;

    static constexpr std::string_view RECONSTRUCTION_INOUT = "reconstruction";
    data::ptr<data::reconstruction, data::access::inout> m_rec {this, RECONSTRUCTION_INOUT, true};
};

} // namespace sight::module::ui::qml::reconstruction

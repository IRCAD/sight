/************************************************************************
 *
 * Copyright (C) 2018-2021 IRCAD France
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

#include <core/com/Signal.hpp>
#include <core/com/Signals.hpp>

#include <data/Material.hpp>
#include <data/Reconstruction.hpp>

#include <ui/qml/IQmlEditor.hpp>

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
 * 1: FLAT, 2: GOURAUD, 3: PHONG)
 * - \b onShowNormals(int): called when the opacity changed, it will update the reconstruction Material (1: STANDARD,
 * 2: NORMALS, 3: CELLS_NORMALS)
 *
 * @section QML Qml Configuration
 *
 * @code{.qml}
    SRepresentationEditor {
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
 * - \b reconstruction [sight::data::Reconstruction]: reconstruction that will be updated
 */
class MODULE_UI_QML_CLASS_API SRepresentationEditor : public sight::ui::qml::IQmlEditor
{
Q_OBJECT

public:

    SIGHT_DECLARE_SERVICE(SRepresentationEditor, ::sight::ui::qml::IQmlEditor);

    /// Constructor. Do nothing.
    MODULE_UI_QML_API SRepresentationEditor() noexcept;

    /// Destructor. Do nothing.
    MODULE_UI_QML_API virtual ~SRepresentationEditor() noexcept;

Q_SIGNALS:

    void materialChanged(int representationMode, int shadingMode, int optionMode);

protected:

    typedef core::runtime::ConfigurationElement::sptr Configuration;

    /// Call IQmlEditor::starting
    void starting() override;

    /// Call IQmlEditor::stopping
    void stopping() override;

    /// Emit a signal to update the Qml ui with the material information
    void updating() override;

    /// Do nothing.
    void configuring() override;

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect Reconstruction::s_MODIFIED_SIG to this::s_UPDATE_SLOT
     */
    KeyConnectionsMap getAutoConnections() const override;

    /// Notify the changes
    void notifyMaterial();

protected Q_SLOTS:

    /// Called when the organ representation changed, it will update the reconstruction Material
    void onChangeRepresentation(int id);
    /// Called when the organ shading changed, it will update the reconstruction Material
    void onChangeShading(int id);
    /// Called when the organ normal option changed, it will update the reconstruction Material
    void onShowNormals(int state);

private:

    data::Material::sptr m_material;

    static const service::key_t s_RECONSTRUCTION_INOUT;
    data::ptr<data::Reconstruction, data::Access::inout> m_rec {this, s_RECONSTRUCTION_INOUT};
};

} // uiReconstructionQml

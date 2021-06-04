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

#include <ui/qml/IQmlEditor.hpp>

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
    SOrganMaterialEditor {
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
 * - \b reconstruction [sight::data::Reconstruction]: reconstruction containing the material to update.
 */
class MODULE_UI_QML_CLASS_API SOrganMaterialEditor : public sight::ui::qml::IQmlEditor
{
Q_OBJECT

public:

    SIGHT_DECLARE_SERVICE(SOrganMaterialEditor, ::sight::ui::qml::IQmlEditor);

    /// Constructor. Do nothing.
    MODULE_UI_QML_API SOrganMaterialEditor() noexcept;

    /// Destructor. Do nothing.
    MODULE_UI_QML_API virtual ~SOrganMaterialEditor() noexcept;

Q_SIGNALS:

    void materialChanged(QColor color, int opacity);

protected:

    typedef core::runtime::ConfigurationElement::sptr Configuration;

    /// Call IQmlEditor::starting
    void starting() override;

    /// Call IQmlEditor::stopping
    void stopping() override;

    /// Emit a signal to update the Qml ui with the material information
    void updating() override;

    /// Do nothing
    void configuring() override;

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect Reconstruction::s_MODIFIED_SIG to this::s_UPDATE_SLOT
     */
    KeyConnectionsMap getAutoConnections() const override;

protected Q_SLOTS:

    /// Called when the opacity changed, it will update the reconstruction Material
    void onOpacitySlider(int value);

    /// Called when the color changed, it will update the reconstruction Material
    void onColor(QColor color);

private:

    /// Notify the material changes
    void materialNotification();
};

} // uiReconstructionQml

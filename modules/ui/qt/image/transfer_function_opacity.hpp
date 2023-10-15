/************************************************************************
 *
 * Copyright (C) 2023 IRCAD France
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

#include <data/transfer_function.hpp>

#include <ui/__/editor.hpp>

#include <QPointer>

class QSlider;
class QTimer;

namespace sight::module::ui::qt::image
{

/**
 * @brief Provides a slider allowing to scale the opacity of a transfer function or one of its piece.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
    <service uid="..." type="sight::module::ui::qt::image::transfer_function_opacity" auto_connect="true">
        <inout key="tf" uid="..." />
        <config piece="3" />
    </service>
   @endcode
 *
 * @subsection In-Out In-Out
 * - \b tf [sight::data::transfer_function]: the current transfer function.
 *
 * @subsection Configuration Configuration
 * - \b piece(optional, default="-1"): if specified not negative, apply the opacity on the given piece instead of the
 * whole function.
 */
class MODULE_UI_QT_CLASS_API transfer_function_opacity final : public QObject,
                                                               public sight::ui::editor
{
Q_OBJECT

public:

    SIGHT_DECLARE_SERVICE(transfer_function_opacity, sight::ui::editor);

    /// Initialize signals and slots.
    MODULE_UI_QT_API transfer_function_opacity() noexcept = default;

    /// Destroys the service.
    MODULE_UI_QT_API ~transfer_function_opacity() noexcept override = default;

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
     *
     * Connect all transfers functions signals to the update() slot.
     */
    [[nodiscard]] connections_t auto_connections() const final;

protected Q_SLOTS:

    /// Called when the slider moves
    void changeOpacity(int value);

private:

    /// If configured, contains the index of a transfer function piece, otherwise -1
    std::optional<std::size_t> m_piece = {};

    /// Slider widget
    QPointer<QSlider> m_slider;

    /// Previous value, used to scale the opacity when it changes
    int m_previous_value {50};

    static constexpr std::string_view s_TF = "tf";

    data::ptr<data::transfer_function, data::Access::inout> m_tf {this, s_TF, true};
};

} // namespace sight::module::ui::qt::image

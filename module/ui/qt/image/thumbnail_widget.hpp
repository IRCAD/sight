/************************************************************************
 *
 * Copyright (C) 2026 IRCAD France
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

#include <core/com/helper/sig_slot_connection.hpp>

#include <data/image_series.hpp>
#include <data/model_series.hpp>

#include <ui/__/editor.hpp>

#include <QBoxLayout>
#include <QPointer>
#include <QWidget>

#include <cstddef>
#include <string>
#include <string_view>

namespace sight::module::ui::qt::image
{

/**
 * @brief Displays image-series frames or model-series reconstructions as thumbnails.
 *
 * @section XML XML configuration
 * @code{.xml}
   <service uid="..." type="sight::module::ui::qt::image::thumbnail_widget">
       <in key="image_series" uid="..." />
       <config width="..." height="..." orientation="vertical" color="..." />
   </service>

   <service uid="..." type="sight::module::ui::qt::image::thumbnail_widget">
       <in key="model_series" uid="..." />
       <config width="..." height="..." reconstructionId="..." />
   </service>
   @endcode
 *
 * @subsection Input Input
 * Exactly one of the following inputs must be provided:
 * - \b image_series [sight::data::image_series]: image series to display (optional).
 * - \b model_series [sight::data::model_series]: model series whose meshes are displayed (optional).
 *
 * @subsection Configuration Configuration
 * - \b width (optional, default=0): thumbnail width.
 * - \b height (optional, default=0): thumbnail height.
 * - \b orientation (optional, vertical/horizontal, default=vertical): layout orientation.
 * - \b color (optional, default=#64808080): background color, in any format accepted by QColor.
 * - \b max (optional, default=0): maximum number of thumbnails. A value of 0 displays every frame.
 * - \b reconstructionId (optional): internal ID of the only model-series reconstruction to display.
 */
class thumbnail_widget final : public sight::ui::editor
{
public:

    SIGHT_DECLARE_SERVICE(thumbnail_widget, sight::ui::editor);

    ~thumbnail_widget() final = default;

protected:

    void configuring() final;

    void starting() final;

    sight::service::connections_t auto_connections() const final;

    void updating() final;

    void stopping() final;

private:

    /// Creates the base widget data.
    void create_widget();

    /// Implementation of the widget
    QPointer<QWidget> m_thumbnail_widget;

    /// Contains the main layout of the generated UI.
    QPointer<QBoxLayout> m_main_layout {nullptr};

    /// Enables the vertical orientation of the UI.
    bool m_vertical {true};

    /// Sets the width of offscreen renderer.
    std::size_t m_width {0};

    /// Sets the height of offscreen renderer.
    std::size_t m_height {0};

    /// Maximum number of thumbnails to display. Zero means all frames.
    std::size_t m_max_thumbnails {0};

    std::string m_color {"#64808080"};

    /// When set, only the model-series reconstruction with this internal ID is displayed.
    std::string m_reconstruction_id;

    static constexpr std::string_view IMAGE_SERIES_IN = "image_series";
    static constexpr std::string_view MODEL_SERIES_IN = "model_series";

    sight::data::ptr<sight::data::image_series, sight::data::access::in> m_image_series {
        this,
        IMAGE_SERIES_IN,
        true
    };
    sight::data::ptr<sight::data::model_series, sight::data::access::in> m_model_series {
        this,
        MODEL_SERIES_IN,
        true
    };

    /// Connections to reconstruction, mesh and material signals nested in a model series.
    core::com::helper::sig_slot_connection m_model_connections;
};

} // namespace sight::module::ui::qt::image

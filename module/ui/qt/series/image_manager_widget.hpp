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

#include <core/com/signal.hpp>

#include <data/image_series.hpp>
#include <data/model_series.hpp>
#include <data/series_set.hpp>

#include <service/base.hpp>

#include <ui/__/container/widget.hpp>
#include <ui/__/editor.hpp>

#include <QButtonGroup>
#include <QFrame>
#include <QObject>
#include <QPointer>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QWidget>

#include <cstddef>
#include <string>
#include <unordered_map>

namespace sight::module::ui::qt::series
{

/**
 * @brief Displays and manages the image and model series available in a series set.
 *
 * @section Signals Signals
 * - \b image_removed(string): emitted when an image card is closed.
 * - \b model_removed(string): emitted when a reconstruction card is closed.
 * - \b image_selected(string): emitted when an image becomes active.
 * - \b image_visibility_changed(string, bool): emitted when image visibility changes.
 * - \b model_selected(string, bool): emitted when reconstruction visibility changes.
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service type="sight::module::ui::qt::series::image_manager_widget">
        <in key="series_set" uid="..." />
        <in key="model_series" uid="..." />
        <config thumbnail_width="130" thumbnail_height="96" thumbnail_color="#00000000" />
    </service>
   @endcode
 *
 * @subsection Input Input
 * - \b series_set [sight::data::series_set]: image and model series to display.
 * - \b model_series [sight::data::model_series] (optional): model series to display.
 *
 * @subsection Configuration Configuration
 * - \b thumbnail_width (optional, default=128): thumbnail width in pixels.
 * - \b thumbnail_height (optional, default=96): thumbnail height in pixels.
 * - \b thumbnail_color (optional, default="#00000000"): thumbnail background color.
 */
class image_manager_widget final :
    public QObject,
    public sight::ui::editor
{
Q_OBJECT

public:

    /// Stores the user state of a series card.
    struct series_state
    {
        std::string alias;
        bool closed {false};
    };

    struct signals
    {
        /// Emitted when an image card is removed.
        using image_removed_t = core::com::signal<void (std::string)>;
        static inline const signal_key_t IMAGE_REMOVED = "image_removed";

        /// Emitted when a model card is removed.
        using model_removed_t = core::com::signal<void (std::string)>;
        static inline const signal_key_t MODEL_REMOVED = "model_removed";

        /// Emitted when an image becomes active.
        using image_selected_t = core::com::signal<void (std::string)>;
        static inline const signal_key_t IMAGE_SELECTED = "image_selected";

        /// Emitted when image visibility changes.
        using image_visibility_changed_t = core::com::signal<void (std::string, bool)>;
        static inline const signal_key_t IMAGE_VISIBILITY_CHANGED = "image_visibility_changed";

        /// Emitted when model visibility changes.
        using model_selected_t = core::com::signal<void (std::string, bool)>;
        static inline const signal_key_t MODEL_SELECTED = "model_selected";
    };

    SIGHT_DECLARE_SERVICE(image_manager_widget, sight::ui::editor);

    image_manager_widget();

    ~image_manager_widget() final = default;

protected:

    void starting() final;

    void configuring() final;

    connections_t auto_connections() const final;

    void stopping() final;

    void updating() final;

private:

    struct image_card
    {
        QPointer<QFrame> widget;
        sight::service::base::sptr thumbnail_service;
        sight::ui::container::widget::sptr thumbnail_container;
        std::string thumbnail_service_id;
    };

    void update_image_widgets();
    void add_card(
        const std::string& _id,
        const std::string& _label,
        const std::string& _date,
        const sight::data::image_series::sptr& _image_series  = nullptr,
        const sight::data::model_series::csptr& _model_series = nullptr
    );
    void remove_card(const std::string& _id);
    void clear_cards();

    sight::data::ptr<sight::data::series_set, sight::data::access::in> m_series_set {this, "series_set"};
    sight::data::ptr<sight::data::model_series, sight::data::access::in> m_model_series {this, "model_series", true};

    QPointer<QScrollArea> m_scroll_area;
    QPointer<QWidget> m_cards_container;
    QPointer<QVBoxLayout> m_image_layout;
    QPointer<QButtonGroup> m_image_selection_group;

    std::string m_selected_image_id;

    /// Runtime states indexed by the internal Sight ID.
    std::unordered_map<std::string, series_state> m_series_states;

    /// Qt cards indexed by the internal Sight ID.
    std::unordered_map<std::string, image_card> m_image_cards;

    std::size_t m_thumbnail_width {128};
    std::size_t m_thumbnail_height {96};
    std::string m_thumbnail_color {"#00000000"};
};

} // namespace sight::module::ui::qt::series

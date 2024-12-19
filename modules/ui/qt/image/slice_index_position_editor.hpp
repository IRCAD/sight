/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "data/image_series.hpp"

#include <core/tools/failed.hpp>

#include <data/helper/medical_image.hpp>

#include <ui/__/editor.hpp>
#include <ui/qt/slice_selector.hpp>

#include <map>

namespace sight::module::ui::qt::image
{

enum label_option_t
{
    index = 0,
    position
};
enum orientation_t
{
    /// Directions.
    x_axis = 0,
    y_axis,
    z_axis,
    /// Planar definitions.
    sagittal = x_axis,
    frontal  = y_axis,
    axial    = z_axis
};

/**
 * @brief   slice_index_position_editor service allows to change the slice index/position of an image.
 *
 * This is represented by
 *  - a slider to select the slice index/position
 *  - a choice list to select the slice orientation (axial, frontal, sagittal)
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service uid="..." type="sight::module::ui::qt::image::slice_index_position_editor" auto_connect="true">
      <inout key="image" uid="..."/>
      <config orientation="${orientationValue}" label="position" display_axis_selector="true"
 * display_step_buttons="true" />
   </service>
   @endcode
 * @subsection In-Out In-Out
 * - \b image [sight::data::image]: image on which the slice index/position will be changed.
 *
 * @subsection Configuration Configuration
 * - \b orientation : Axis on which the index/position of slice will be changed, must be "axial", "frontal" or
 *"sagittal". ( default = "axial")
 * - \b label : allows to choose between showing the position of the slice or the index of the slice. (default =
 *"index")
 * - \b display_axis_selector : Allows to change the axis.(default = "true")
 * - \b display_step_buttons : Allows to change the slice index/position with step buttons.(default = "false")
 */
class slice_index_position_editor : public sight::ui::editor
{
public:

    SIGHT_DECLARE_SERVICE(slice_index_position_editor, sight::ui::editor);

    /// Constructor. Do nothing.
    slice_index_position_editor() noexcept;

    /// Destructor. Do nothing.
    ~slice_index_position_editor() noexcept override = default;

protected:

    static const service::base::key_t IMAGE_INOUT;

    /// @brief The slice type: axial, frontal, sagittal.
    using axis_t = data::helper::medical_image::axis_t;

    void configuring() override;
    /**
     * @brief Install the layout.
     */
    void starting() override;

    /**
     * @brief Destroy the layout.
     */
    void stopping() override;

    /// Update editor information from the image
    void updating() override;

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect image::MODIFIED_SIG to this::service::slots::UPDATE
     * Connect image::SLICE_INDEX_MODIFIED_SIG to this::UPDATE_SLICE_INDEX_SLOT
     * Connect image::SLICE_TYPE_MODIFIED_SIG to this::UPDATE_SLICE_TYPE_SLOT
     * Connect image::BUFFER_MODIFIED_SIG to this::UPDATE_BUFFER_SLOT
     */
    connections_t auto_connections() const override;

    /// Overrides
    void info(std::ostream& _sstream) override;

    /// Update the editor slider from the image slice index.
    void update_slice_index_from_img(const sight::data::image& _image);

    /// Update the editor slice type choice from the image slice type.
    void update_slice_type_from_img(const axis_t& _type);

    /// This method is called when the slider is move. Notify the slice index is modified.
    void slice_index_notification(int _index);

    /// This method is called when the slice type selected change. Notify the slice type is modified.
    void slice_type_notification(int _type);

    ///update the slider verticals lines.
    void update_slider_fiducial();

    /// This method is called when the label type changes, to destroy the previous type of `qt_container`.
    void destroyEditorContainer();

    /// This method is called when the slice label option changes. It notifies that the slice label option has been
    /// toggled.
    void slice_label_notification();

private:

    /**
     * @name Slots
     * @{
     */

    /// Slot: update image slice index
    void update_slice_index(int _axial, int _frontal, int _sagittal);

    /// Slot: update image slice type
    void update_slice_type(int _from, int _to);

    /// Slot: update image buffer
    void update_buffer();
    /**
     * @}
     */

    sight::ui::qt::slice_selector* m_slice_selector_with_index {};
    data::ptr<data::image, data::access::inout> m_image {this, "image"};

    std::int64_t m_axial_index {-1};
    std::int64_t m_frontal_index {-1};
    std::int64_t m_sagittal_index {-1};

    double m_axial_position {-1};
    double m_sagittal_position {-1};
    double m_frontal_position {-1};

    label_option_t m_label_option {label_option_t::index};
    axis_t m_axis {axis_t::z_axis};

    static std::map<axis_t, std::string> orientation_prefix_map;
    bool m_display_axis_selector {true};

    bool m_display_step_buttons {false};
};

} // namespace sight::module::ui::qt::image

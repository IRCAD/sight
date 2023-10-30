/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "modules/ui/qt/config.hpp"

#include <core/tools/failed.hpp>

#include <data/helper/medical_image.hpp>

#include <ui/__/editor.hpp>
#include <ui/qt/slice_selector.hpp>

namespace sight::module::ui::qt::image
{

/**
 * @brief   slice_index_position_editor service allows to change the slice index of an image.
 *
 * This is represented by
 *  - a slider to select the slice index
 *  - a choice list to select the slice orientation (axial, frontal, sagittal)
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service uid="..." type="sight::module::ui::qt::image::slice_index_position_editor" auto_connect="true">
      <inout key="image" uid="..."/>
      <sliceIndex>${orientationValue}</sliceIndex>
      <displayAxisSelector>true</displayAxisSelector>
   </service>
   @endcode
 * @subsection In-Out In-Out
 * - \b image [sight::data::image]: image on which the slice index will be changed
 *
 * @subsection Configuration Configuration
 * - \b sliceIndex : Axis on which the index will be changed, must be "axial", "frontal" or "sagittal".
 * - \b displayAxisSelector : Allows to change the axis.
 * - \b displayStepButtons : Allows to change the slice index with step buttons.
 */
class MODULE_UI_QT_CLASS_API slice_index_position_editor : public sight::ui::editor
{
public:

    SIGHT_DECLARE_SERVICE(slice_index_position_editor, sight::ui::editor);

    /// Constructor. Do nothing.
    MODULE_UI_QT_API slice_index_position_editor() noexcept;

    /// Destructor. Do nothing.
    MODULE_UI_QT_API ~slice_index_position_editor() noexcept override;

protected:

    static const service::base::key_t IMAGE_INOUT;

    /// @brief The slice type: axial, frontal, sagittal.
    using orientation_t = data::helper::medical_image::orientation_t;

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
     * @brief Configure the editor.
     *
     * Example of configuration
     * @code{.xml}
       <service uid="slider_negato1" type="sight::module::ui::qt::image::slice_index_position_editor"
     * auto_connect="true">
        <sliceIndex>axial</sliceIndex>
       </service>
       @endcode
       \b sliceIndex must be "axial", "frontal" or "sagittal".
     */
    void configuring() override;

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect image::MODIFIED_SIG to this::service::slots::UPDATE
     * Connect image::SLICE_INDEX_MODIFIED_SIG to this::UPDATE_SLICE_INDEX_SLOT
     * Connect image::SLICE_TYPE_MODIFIED_SIG to this::UPDATE_SLICE_TYPE_SLOT
     * Connect image::BUFFER_MODIFIED_SIG to this::UPDATE_BUFFER_SLOT
     */
    MODULE_UI_QT_API connections_t auto_connections() const override;

    /// Overrides
    MODULE_UI_QT_API void info(std::ostream& _sstream) override;

    /// Update the editor slider from the image slice index.
    MODULE_UI_QT_API void update_slice_index_from_img(const sight::data::image& _image);

    /// Update the editor slice type choice from the image slice type.
    MODULE_UI_QT_API void update_slice_type_from_img(const orientation_t& _type);

    /// This method is called when the slider is move. Notify the slice index is modified.
    MODULE_UI_QT_API void slice_index_notification(int _index);

    /// This method is called when the slice type selected change. Notify the slice type is modified.
    MODULE_UI_QT_API void slice_type_notification(int _type);

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

    sight::ui::qt::slice_selector* m_slice_selector_panel {};

    data::ptr<data::image, data::access::inout> m_image {this, "image", true};

    std::int64_t m_axial_index {-1};
    std::int64_t m_frontal_index {-1};
    std::int64_t m_sagittal_index {-1};

    orientation_t m_orientation {orientation_t::z_axis};

    bool m_display_axis_selector {true};

    bool m_display_step_buttons {false};
};

} // namespace sight::module::ui::qt::image

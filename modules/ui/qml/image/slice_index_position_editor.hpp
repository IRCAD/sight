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

#include <data/helper/medical_image.hpp>

#include <ui/qml/editor.hpp>

#include <QObject>

namespace sight::module::ui::qml::image
{

/**
 * @brief   slice_index_position_editor service allows to change the slice index of an image.
 *
 * This is represented by
 *  - a slider to select the slice index
 *  - a choice list to select the slice orientation (axial, frontal, sagittal)
 *
 * @section Slots Slots
 * - \b updateSliceIndex(int, int, int): update image slice index
 * - \b updateSliceType(int, int): update image slice type
 *
 * @section QSignal Qt Signals
 * - \b setSliceRange(int min, int max): emitted to change the range of the slice indices slider
 * - \b setSliceValue(int value): emitted to change the slice index value
 * - \b set_slice_type(int type): emitted to change the slice type (0: sagittal, 1: frontal, 2: axial)
 *
 * @section QSlots Qt Slots
 * - \b onSliceIndex(int): set the current slice index value
 * - \b onSliceType(int): set the current slice type (0: sagittal, 1: frontal, 2: axial)
 *
 * @section QML Qml Configuration
 *
 * @code{.qml}
    slice_index_position_editor {
        id: sliceIndexEditor

        sliceIndex: sliceIndexSelector.sliceIndex

        onSetSliceRange: {
            from = min
            to = max
        }

        onSetSliceValue: {
            slider.value = value
        }

        onSetSliceType: {
            sliceType.currentIndex = type
        }
    }
   @endcode
 * @subsection Configuration Configuration
 * - \b sliceIndex : Axis on which the index will be changed, must be "axial", "frontal" or "sagittal".
 *
 * @section Objects Required objects
 *
 * @subsection In-Out In-Out
 * - \b image [sight::data::image]: image on which the slice index will be changed
 */
class MODULE_UI_QML_CLASS_API slice_index_position_editor : public sight::ui::qml::editor
{
Q_OBJECT
Q_PROPERTY(int sliceIndex READ get_slice_index WRITE set_slice_index)

public:

    SIGHT_DECLARE_SERVICE(slice_index_position_editor, sight::ui::qml::editor);

    /// Constructor. Do nothing.
    MODULE_UI_QML_API slice_index_position_editor() noexcept;

    /// Destructor. Do nothing.
    MODULE_UI_QML_API ~slice_index_position_editor() noexcept override;

    /// To handle orientation of slices.
    using orientation_t = data::helper::medical_image::orientation_t;

Q_SIGNALS:

    void setSliceRange(int _min, int _max);
    void setSliceValue(int _value);
    void set_slice_type(int _type);

public Q_SLOTS:

    /// This method is called when the slider is moved. Notify the slice index is modified.
    MODULE_UI_QML_API void onSliceIndex(int _index);

    /// This method is called when the slice type selected changes. Notify the slice type is modified.
    MODULE_UI_QML_API void onSliceType(int _type);

protected:

    /// Update the information from the image
    MODULE_UI_QML_API void starting() override;

    /// Do nothing
    MODULE_UI_QML_API void stopping() override;

    /// Update editor information from the image
    MODULE_UI_QML_API void updating() override;

    /// Do nothing
    MODULE_UI_QML_API void configuring() override;

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect image::MODIFIED_SIG to this::service::slots::UPDATE
     * Connect image::SLICE_INDEX_MODIFIED_SIG to this::UPDATE_SLICE_INDEX_SLOT
     * Connect image::SLICE_TYPE_MODIFIED_SIG to this::UPDATE_SLICE_TYPE_SLOT
     * Connect image::BUFFER_MODIFIED_SIG to this::UPDATE_BUFFER_SLOT
     */
    MODULE_UI_QML_API connections_t auto_connections() const override;

    /// Update the editor slider from the image slice index.
    MODULE_UI_QML_API void updateSliceIndexFromImg();

    /// Update the editor slice type choice from the image slice type.
    MODULE_UI_QML_API void updateSliceTypeFromImg(const orientation_t& _type);

private:

    /**
     * @name Slots
     * @{
     */
    /// Slot: update image slice index
    void updateSliceIndex(int _axial, int _frontal, int _sagittal);

    /// Slot: update image slice type
    void updateSliceType(int _from, int _to);
    /**
     * @}
     */

    /// Define the slice type, it should only be called before starting from the qml interface
    void set_slice_index(int _slice_index);
    int get_slice_index() const;

    /// image data
    static constexpr std::string_view s_IMAGE_INOUT = "image";
    data::ptr<data::image, data::Access::inout> m_image {this, s_IMAGE_INOUT, true};

    std::int64_t m_axialIndex {-1};
    std::int64_t m_frontalIndex {-1};
    std::int64_t m_sagittalIndex {-1};

    orientation_t m_orientation {orientation_t::Z_AXIS};
};

} // namespace sight::module::ui::qml::image

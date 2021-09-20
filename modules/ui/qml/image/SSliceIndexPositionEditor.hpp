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

#include <data/helper/MedicalImage.hpp>

#include <ui/qml/IQmlEditor.hpp>

#include <QObject>

namespace sight::module::ui::qml::image
{

/**
 * @brief   SSliceIndexPositionEditor service allows to change the slice index of an image.
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
 * - \b setSliceType(int type): emitted to change the slice type (0: sagittal, 1: frontal, 2: axial)
 *
 * @section QSlots Qt Slots
 * - \b onSliceIndex(int): set the current slice index value
 * - \b onSliceType(int): set the current slice type (0: sagittal, 1: frontal, 2: axial)
 *
 * @section QML Qml Configuration
 *
 * @code{.qml}
    SSliceIndexPositionEditor {
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
 * - \b image [sight::data::Image]: image on which the slice index will be changed
 */
class MODULE_UI_QML_CLASS_API SSliceIndexPositionEditor : public sight::ui::qml::IQmlEditor,
                                                          public data::helper::MedicalImage
{
Q_OBJECT
Q_PROPERTY(int sliceIndex READ getSliceIndex WRITE setSliceIndex)

public:

    SIGHT_DECLARE_SERVICE(SSliceIndexPositionEditor, ::sight::ui::qml::IQmlEditor);

    /// Constructor. Do nothing.
    MODULE_UI_QML_API SSliceIndexPositionEditor() noexcept;

    /// Destructor. Do nothing.
    MODULE_UI_QML_API virtual ~SSliceIndexPositionEditor() noexcept;

Q_SIGNALS:

    void setSliceRange(int min, int max);
    void setSliceValue(int value);
    void setSliceType(int type);

public Q_SLOTS:

    /// This method is called when the slider is moved. Notify the slice index is modified.
    MODULE_UI_QML_API void onSliceIndex(int index);

    /// This method is called when the slice type selected changes. Notify the slice type is modified.
    MODULE_UI_QML_API void onSliceType(int type);

protected:

    /// @brief The slice type: axial, frontal, sagittal.
    using data::helper::MedicalImage::Orientation;

    /// Update the infromation from the image
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
     * Connect Image::s_MODIFIED_SIG to this::s_UPDATE_SLOT
     * Connect Image::s_SLICE_INDEX_MODIFIED_SIG to this::s_UPDATE_SLICE_INDEX_SLOT
     * Connect Image::s_SLICE_TYPE_MODIFIED_SIG to this::s_UPDATE_SLICE_TYPE_SLOT
     * Connect Image::s_BUFFER_MODIFIED_SIG to this::s_UPDATE_BUFFER_SLOT
     */
    MODULE_UI_QML_API KeyConnectionsMap getAutoConnections() const override;

    /// Update the editor slider from the image slice index.
    MODULE_UI_QML_API void updateSliceIndexFromImg();

    /// Update the editor slice type choice from the image slice type.
    MODULE_UI_QML_API void updateSliceTypeFromImg(Orientation type);

private:

    /**
     * @name Slots
     * @{
     */
    /// Slot: update image slice index
    void updateSliceIndex(int axial, int frontal, int sagittal);

    /// Slot: update image slice type
    void updateSliceType(int from, int to);
    /**
     * @}
     */

    /// Define the slice type, it should only be called before starting from the qml interface
    void setSliceIndex(int sliceIndex);
    int getSliceIndex() const;

    /// @brief The field IDs for the slice index.
    static const std::string* SLICE_INDEX_FIELDID[3];

    /// Image data
    static constexpr std::string_view s_IMAGE_INOUT = "image";
    data::ptr<data::Image, data::Access::inout> m_image {this, s_IMAGE_INOUT, true};
};

} // uiImageQml

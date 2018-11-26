/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "uiImageQml/config.hpp"

#include <fwDataTools/helper/MedicalImageAdaptor.hpp>

#include <fwQml/IQmlEditor.hpp>

#include <QObject>

namespace uiImageQml
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
 * - \b image [::fwData::Image]: image on which the slice index will be changed
 */
class UIIMAGEQML_CLASS_QT_API SSliceIndexPositionEditor : public ::fwQml::IQmlEditor,
                                                          public ::fwDataTools::helper::MedicalImageAdaptor
{
Q_OBJECT
Q_PROPERTY(int sliceIndex WRITE configureSliceIndex)
public:

    fwCoreServiceClassDefinitionsMacro( (SSliceIndexPositionEditor)(::fwQml::IQmlEditor) )

    /// Constructor. Do nothing.
    UIIMAGEQML_QT_API SSliceIndexPositionEditor() noexcept;

    /// Destructor. Do nothing.
    UIIMAGEQML_QT_API virtual ~SSliceIndexPositionEditor() noexcept;

Q_SIGNALS:
    void setSliceRange(int min, int max);
    void setSliceValue(int value);
    void setSliceType(int type);

public Q_SLOTS:
    /// This method is called when the slider is moved. Notify the slice index is modified.
    UIIMAGEQML_QT_API void onSliceIndex(int index);

    /// This method is called when the slice type selected changes. Notify the slice type is modified.
    UIIMAGEQML_QT_API void onSliceType( int type );

protected:

    /// @brief The slice type: axial, frontal, sagittal.
    using ::fwDataTools::helper::MedicalImageAdaptor::Orientation;

    /// Update the infromation from the image
    UIIMAGEQML_QT_API virtual void starting() override;

    /// Do nothing
    UIIMAGEQML_QT_API virtual void stopping() override;

    /// Update editor information from the image
    UIIMAGEQML_QT_API virtual void updating() override;

    /// Do nothing
    UIIMAGEQML_QT_API virtual void configuring() override;

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect Image::s_MODIFIED_SIG to this::s_UPDATE_SLOT
     * Connect Image::s_SLICE_INDEX_MODIFIED_SIG to this::s_UPDATE_SLICE_INDEX_SLOT
     * Connect Image::s_SLICE_TYPE_MODIFIED_SIG to this::s_UPDATE_SLICE_TYPE_SLOT
     * Connect Image::s_BUFFER_MODIFIED_SIG to this::s_UPDATE_BUFFER_SLOT
     */
    UIIMAGEQML_QT_API virtual KeyConnectionsMap getAutoConnections() const override;

    /// Update the editor slider from the image slice index.
    UIIMAGEQML_QT_API void updateSliceIndexFromImg();

    /// Update the editor slice type choice from the image slice type.
    UIIMAGEQML_QT_API void updateSliceTypeFromImg(Orientation type );

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
    void configureSliceIndex(int sliceIndex);

    /// @brief The field IDs for the slice index.
    static const std::string* SLICE_INDEX_FIELDID[ 3 ];
};

} // uiImageQml

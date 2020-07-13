/************************************************************************
 *
 * Copyright (C) 2018-2020 IRCAD France
 * Copyright (C) 2018-2020 IHU Strasbourg
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

#include "visuOgreAdaptor/config.hpp"
#include "visuOgreAdaptor/SMaterial.hpp"

#include <fwCore/macros.hpp>

#include <fwDataTools/helper/MedicalImage.hpp>

#include <fwRenderOgre/IAdaptor.hpp>
#include <fwRenderOgre/ITransformable.hpp>
#include <fwRenderOgre/Text.hpp>

namespace visuOgreAdaptor
{

/**
 * @brief This adaptor displays landmarks.
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service uid="..." type="::visuOgreAdaptor::SLandmarks">
        <in key="landmarks" uid="..." />
        <in key="image" uid="..." />
        <config layer="default" transform="transformUID" />
    </service>
   @endcode
 *
 * @subsection Input Input:
 * - \b landmarks [::fwData::Landmarks]: landmarks to display.
 * - \b image [::fwData::Image] (optional): if the image is used, each landmark will be displayed only if the
 *      image slice is on it.
 *
 * @subsection Configuration Configuration:
 * - \b layer (mandatory, string): defines landmarks layer.
 * - \b transform (optional, string, default=""): the name of the Ogre transform node where to attach the mesh, as it
 *      was specified
 * - \b fontSource (optional, string, default=DejaVuSans.ttf): TrueType font (*.ttf) source file.
 * - \b fontSize (optional, unsigned int, default=16): font size in points.
 * - \b label (optional, bool, default=true): display label.
 * - \b orientation (optional, axial/frontal/sagittal, default=axial): orientation of the negato.
 */
class VISUOGREADAPTOR_CLASS_API SLandmarks final :
    public ::fwRenderOgre::IAdaptor,
    public ::fwRenderOgre::ITransformable
{

public:

    /// Generates default methods as New, dynamicCast, ...
    fwCoreServiceMacro(SLandmarks, ::fwRenderOgre::IAdaptor)

    /// Creates the adaptor.
    VISUOGREADAPTOR_API SLandmarks() noexcept;

    /// Destroys the service.
    VISUOGREADAPTOR_API ~SLandmarks() noexcept override;

protected:

    /// Configure the adaptor.
    VISUOGREADAPTOR_API void configuring() override;

    /// Creates the material adaptor end create existing landmarls.
    VISUOGREADAPTOR_API void starting() override;

    /**
     * @brief Proposals to connect service slots to associated object signals.
     * @return A map of each proposed connection.
     *
     * Connect ::fwData::Landmarks::s_MODIFIED_SIG of s_LANDMARKS_INPUT to s_UPDATE_SLOT_SLOT
     * Connect ::fwData::Landmarks::s_GROUP_REMOVED_SIG of s_LANDMARKS_INPUT to s_REMOVE_GROUP_SLOT
     * Connect ::fwData::Landmarks::s_GROUP_MODIFIED_SIG of s_LANDMARKS_INPUT to s_MODIFY_GROUP_SLOT
     * Connect ::fwData::Landmarks::s_POINT_ADDED_SIG of s_LANDMARKS_INPUT to s_ADD_POINT_SLOT
     * Connect ::fwData::Landmarks::s_POINT_REMOVED_SIG of s_LANDMARKS_INPUT to s_REMOVE_GROUP_SLOT
     * Connect ::fwData::Landmarks::s_POINT_INSERTED_SIG of s_LANDMARKS_INPUT to s_INSERT_POINT_SLOT
     * Connect ::fwData::Image::s_SLICE_TYPE_MODIFIED_SIG of s_IMAGE_INPUT to s_SLICE_TYPE_SLOT
     * Connect ::fwData::Image::s_SLICE_INDEX_MODIFIED_SIG of s_IMAGE_INPUT to s_SLICE_INDEX_SLOT
     */
    VISUOGREADAPTOR_API ::fwServices::IService::KeyConnectionsMap getAutoConnections() const override;

    /// Deletes landmarks and re-create them.
    VISUOGREADAPTOR_API void updating() override;

    /// Destroys Ogre's resources.
    VISUOGREADAPTOR_API void stopping() override;

private:

    /// Stores data used to display one landmark.
    struct Landmark
    {
        ::Ogre::SceneNode* m_node { nullptr };      /*!< Contains the node of the landmark */
        ::Ogre::ManualObject* m_object { nullptr }; /*!< Contains the manual object that represent the landmark */
        std::string m_groupName { "" };             /*!< Defines the group name of the landmark */
        size_t m_index { 0 };                       /*!< Defines the index of the landmark */
        ::fwRenderOgre::Text* m_label { nullptr };  /*!< Defines the text label of the landmark (can be nullptr) */
    };

    typedef ::fwDataTools::helper::MedicalImage::Orientation OrientationMode;

    /**
     * @brief SLOT: removes an entire group.
     * @param _groupName name of the group to remove.
     */
    void removeGroup(std::string _groupName);

    /**
     * @brief SLOT: removes an entire group and re-create it.
     * @param _groupName name of the group to update.
     */
    void modifyGroup(std::string _groupName);

    /**
     * @brief SLOT: adds the last point of a landmarks group.
     * @param _groupName group name of the point to add.
     */
    void addPoint(std::string _groupName);

    /**
     * @brief SLOT: removes a point.
     * @param _groupName group name of the landmark.
     * @param _index index of the point relative to the group.
     */
    void removePoint(std::string _groupName, size_t _index);

    /**
     * @brief SLOT: inserts a point.
     * @param _groupName group name of the landmark.
     * @param _index index of the point relative to the group.
     */
    void insertPoint(std::string _groupName, size_t _index);

    /// SLOT: initializes image slices index if there is one.
    void initializeImage();

    /**
     * @brief SLOT: updates the image slice type.
     * @param _from origin of the orientation.
     * @param _to destination of the orientation.
     */
    void changeSliceType(int _from, int _to);

    /**
     * @brief SLOT: updates the image slice index to show or hide landmarks.
     * @param _axialIndex new axial slice index.
     * @param _frontalIndex new frontal slice index.
     * @param _sagittalIndex new sagittal slice index.
     */
    void changeSliceIndex(int _axialIndex, int _frontalIndex, int _sagittalIndex);

    /**
     * @brief Hides landmarks that are not on the current image slice index (if one is given).
     * @see hideLandmark().
     */
    void hideLandmarks();

    /**
     * @brief Hides the landmark if it's not on the current image slice index (if one is given).
     * @param _landmark the landmark to hide.
     */
    void hideLandmark(Landmark& _landmark);

    /// Contains the root scene node.
    ::Ogre::SceneNode* m_transNode { nullptr };

    /// Contains the material data.
    ::fwData::Material::sptr m_material { nullptr };

    /// Contains the Ogre material adaptor.
    ::visuOgreAdaptor::SMaterial::sptr m_materialAdaptor { nullptr };

    /// Stores each landmarks points.
    std::vector< Landmark > m_manualObjects;

    /// Enables labels.
    bool m_enableLabels { true };

    /// Defines the label font size in points.
    size_t m_fontSize { 16 };

    /// Defines the TrueType font source file.
    std::string m_fontSource { "DejaVuSans.ttf" };

    /// Defines the image orientation.
    OrientationMode m_orientation { OrientationMode::Z_AXIS };

    /// Stores the current position index for each axis.
    std::array<float, 3> m_currentSlicePos { 0.f, 0.f, 0.f };

};

} // namespace visuOgreAdaptor.

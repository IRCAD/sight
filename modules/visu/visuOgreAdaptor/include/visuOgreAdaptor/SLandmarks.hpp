/************************************************************************
 *
 * Copyright (C) 2018-2021 IRCAD France
 * Copyright (C) 2018-2021 IHU Strasbourg
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

#include <fwData/Landmarks.hpp>

#include <fwDataTools/helper/MedicalImage.hpp>

#include <fwRenderOgre/IAdaptor.hpp>
#include <fwRenderOgre/ITransformable.hpp>
#include <fwRenderOgre/Text.hpp>

#include <fwThread/Timer.hpp>

namespace visuOgreAdaptor
{

/**
 * @brief This adaptor displays landmarks.
 *
 * @section Slots Slots
 * - \b removeGroup(std::string): removes an entire group.
 * - \b modifyGroup(std::string): removes an entire group and re-create it.
 * - \b addPoint(std::string): adds the last point of a landmarks group.
 * - \b removePoint(std::string, size_t): removes a point.
 * - \b insertPoint(std::string, size_t): inserts a point.
 * - \b selectPoint(std::string, size_t) hightlights the selected landmark.
 * - \b deselectPoint(std::string, size_t): resets the hightlighting of the selected landmark.
 * - \b initializeImage(): initializes image slices index if there is one.
 * - \b changeSliceType(int, int): updates the image slice type.
 * - \b changeSliceIndex(int, int, int): updates the image slice index to show or hide landmarks.
 * - \b updateVisibility(bool): shows or hides the landmarks.
 * - \b toggleVisibility(): toggles whether the landmarks are shown or not.
 * - \b show(): shows the landmarks.
 * - \b hide(): hides the landmarks.
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service uid="..." type="::visuOgreAdaptor::SLandmarks">
        <inout key="landmarks" uid="..." />
        <in key="image" uid="..." />
        <config layer="default" transform="transformUID" visible="true" priority="2" />
    </service>
   @endcode
 *
 * @subsection In-Out In-Out:
 * - \b landmarks [::fwData::Landmarks]: landmarks to display.
 *
 * @subsection Input Input:
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
 * - \b visible (optional, default=true): the visibility of the landmarks.
 * - \b interactive (optional, bool, default=true): enables interactions with landmarks.
 * - \b priority (optional, int, default=2): priority of the interactor.
 * - \b queryMask (optional, uint32, default=0xFFFFFFFF): mask used to filter out entities when the distance is auto
 *      snapped.
 * - \b landmarksQueryFlags (optional, uint32, default=0x40000000): mask applied to landmarks.
 */
class VISUOGREADAPTOR_CLASS_API SLandmarks final :
    public ::fwRenderOgre::IAdaptor,
    public ::fwRenderOgre::ITransformable,
    public ::fwRenderOgre::interactor::IInteractor
{

public:

    /// Generates default methods as New, dynamicCast, ...
    fwCoreServiceMacro(SLandmarks, ::fwRenderOgre::IAdaptor)

    /// Creates the adaptor.
    VISUOGREADAPTOR_API SLandmarks() noexcept;

    /// Destroys the adaptor.
    VISUOGREADAPTOR_API ~SLandmarks() noexcept override;

    /**
     * @brief Retrieves the picked landmark and stores the result in m_pickedData.
     * @param _button pressed mouse button.
     * @param _mod keyboard modifiers.
     * @param _x X screen coordinate.
     * @param _y Y screen coordinate.
     */
    VISUOGREADAPTOR_API void buttonPressEvent(MouseButton _button, Modifier _mod, int _x, int _y) override;

    /**
     * @brief Moves a landmark stored in m_pickedData.
     * @param _button pressed mouse button.
     * @param _mod keyboard modifiers.
     * @param _x X screen coordinate.
     * @param _y Y screen coordinate.
     * @param _dx width displacement of the mouse since the last event.
     * @param _dx height displacement of the mouse since the last event.
     */
    VISUOGREADAPTOR_API void mouseMoveEvent(MouseButton _button, Modifier _mod,
                                            int _x, int _y, int _dx, int _dy) override;

    /**
     * @brief Resets m_pickedData.
     * @param _button pressed mouse button.
     * @param _mod keyboard modifiers.
     * @param _x X screen coordinate.
     * @param _y Y screen coordinate.
     */
    VISUOGREADAPTOR_API void buttonReleaseEvent(MouseButton _button, Modifier _mod, int _x, int _y) override;

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
     * Connect ::fwData::Landmarks::s_POINT_SELECTED_SIG of s_LANDMARKS_INPUT to s_SELECT_POINT_SLOT
     * Connect ::fwData::Landmarks::s_POINT_DESELECTED_SIG of s_LANDMARKS_INPUT to s_DESELECT_POINT_SLOT
     * Connect ::fwData::Image::s_SLICE_TYPE_MODIFIED_SIG of s_IMAGE_INPUT to s_SLICE_TYPE_SLOT
     * Connect ::fwData::Image::s_SLICE_INDEX_MODIFIED_SIG of s_IMAGE_INPUT to s_SLICE_INDEX_SLOT
     */
    VISUOGREADAPTOR_API ::fwServices::IService::KeyConnectionsMap getAutoConnections() const override;

    /// Deletes landmarks and re-create them.
    VISUOGREADAPTOR_API void updating() override;

    /// Destroys Ogre's resources.
    VISUOGREADAPTOR_API void stopping() override;

    /**
     * @brief Sets the landmarks visibility.
     * @param _visible the visibility status of the landmarks.
     */
    VISUOGREADAPTOR_API void setVisible(bool _visible) override;

private:

    /// Stores data used to display one landmark.
    struct Landmark
    {
        Landmark(::Ogre::SceneNode* _node,
                 ::Ogre::ManualObject* _object,
                 std::string _groupName,
                 size_t _index,
                 ::fwRenderOgre::Text* _label) :
            m_node(_node),
            m_object(_object),
            m_groupName(_groupName),
            m_index(_index),
            m_label(_label)
        {
        }

        ::Ogre::SceneNode* m_node { nullptr };      /*!< Contains the node of the landmark */
        ::Ogre::ManualObject* m_object { nullptr }; /*!< Contains the manual object that represent the landmark */
        std::string m_groupName { "" };             /*!< Defines the group name of the landmark */
        size_t m_index { 0 };                       /*!< Defines the index of the landmark */
        ::fwRenderOgre::Text* m_label { nullptr };  /*!< Defines the text label of the landmark (can be nullptr) */
    };

    /// Stores data used to hightlight the selected landmark.
    struct SelectedLandmark
    {
        SelectedLandmark(::fwThread::Timer::sptr _timer, std::shared_ptr< Landmark > _landmark) :
            m_timer(_timer),
            m_landmark(_landmark)
        {
        }

        ::fwThread::Timer::sptr m_timer;
        std::shared_ptr< Landmark > m_landmark;
        bool m_show { false };
    };

    typedef ::fwDataTools::helper::MedicalImage::Orientation OrientationMode;

    /**
     * @brief Gets the normalized camera direction vector.
     * @param _cam camera from which to extract the direction vector.
     * @return A vector representing the camera direction
     */
    static ::Ogre::Vector3 getCamDirection(const ::Ogre::Camera* const _cam);

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
     * @brief SLOT: removes a point group and update it.
     * @param _groupName name of the group to update.
     * @param _index index of the point relative to the group.
     */
    void modifyPoint(std::string _groupName, size_t _index);

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
    void insertPoint(std::string _groupName, size_t _index, const ::fwData::Landmarks::csptr& _data = nullptr);

    /**
     * @brief SLOT: hightlights the selected landmark.
     * @param _groupName group name of the landmark.
     * @param _index index of the point relative to the group.
     */
    void selectPoint(std::string _groupName, size_t _index);

    /**
     * @brief SLOT: resets the hightlights the selected landmark.
     * @param _groupName group name of the landmark.
     * @param _index index of the point relative to the group.
     */
    void deselectPoint(std::string _groupName, size_t _index);

    /**
     * @brief Manages the highting of the landmarks at the given index (must be run in a thread).
     * @param _selectedLandmark which landmarks to manage.
     */
    void hightlight(std::shared_ptr<SelectedLandmark> _selectedLandmark);

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
     * @brief Gets the nearest picked position if there is one.
     * @param _x X screen coordinate.
     * @param _y Y screen coordinate.
     * @return The picked world coordinates.
     */
    std::optional< ::Ogre::Vector3 > getNearestPickedPosition(int _x, int _y);

    /**
     * @brief Hides the landmark if it's not on the current image slice index (if one is given).
     * @param _landmark the landmark to hide.
     */
    void hideLandmark(std::shared_ptr< Landmark > _landmark, const ::fwData::Landmarks::csptr& _data = nullptr);

    /// Contains the root scene node.
    ::Ogre::SceneNode* m_transNode { nullptr };

    /// Contains the material data.
    ::fwData::Material::sptr m_material { nullptr };

    /// Contains the Ogre material adaptor.
    ::visuOgreAdaptor::SMaterial::sptr m_materialAdaptor { nullptr };

    /// Stores each landmarks points.
    std::vector< std::shared_ptr< Landmark > > m_manualObjects;

    /// Enables labels.
    bool m_enableLabels { true };

    /// Defines the label font size in points.
    size_t m_fontSize { 16 };

    /// Defines the TrueType font source file.
    std::string m_fontSource { "DejaVuSans.ttf" };

    /// Stores informations about the selected landmark.
    std::list< std::shared_ptr< SelectedLandmark > > m_selectedLandmarks;

    /// Define a mutex to synchronized methods working with selected landmark.
    std::mutex m_selectedMutex;

    /// Defines the image orientation.
    OrientationMode m_orientation { OrientationMode::Z_AXIS };

    /// Stores the current position index for each axis.
    std::array<float, 3> m_currentSlicePos { 0.f, 0.f, 0.f };

    /// Defines whether or not interactions are enabled with distances.
    bool m_interactive { true };

    /// Defines the priority of the interactor.
    int m_priority { 2 };

    /// Defines the current picked data, reset by buttonReleaseEvent(MouseButton, int, int).
    std::shared_ptr< Landmark > m_pickedData { nullptr };

    /// Defines the mask used to filter out entities when the distance is auto snapped.
    std::uint32_t m_queryMask { 0xFFFFFFFF };

    /// Defines the mask used to filter landmarks, it optimizes the ray launched to retrieve the picked distance.
    std::uint32_t m_landmarksQueryFlag { ::Ogre::SceneManager::ENTITY_TYPE_MASK };

};

} // namespace visuOgreAdaptor.

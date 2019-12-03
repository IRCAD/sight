/************************************************************************
 *
 * Copyright (C) 2019 IRCAD France
 * Copyright (C) 2019 IHU Strasbourg
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

#include <fwData/PointList.hpp>

#include <fwRenderOgre/IAdaptor.hpp>
#include <fwRenderOgre/interactor/IInteractor.hpp>
#include <fwRenderOgre/Material.hpp>
#include <fwRenderOgre/Text.hpp>

#include <OgreVector3.h>

#include <memory>
#include <optional>

namespace visuOgreAdaptor
{

/**
 * @brief Displays distances retrieved from an image.
 *
 * @section Slots Slots
 * - \b addDistance(): Add distances contains in the image from the scene manager.
 * - \b removeDistance(): Remove distances contains in the image from the scene manager.
 * - \b updateVisibilityFromField(): Updates the visibility of distances from the field status.
 * - \b updateVisibility(bool): Updates the visibility of distances.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
        <service uid="..." type="::visuOgreAdaptor::SImageMultiDistances" autoConnect="yes" >
            <inout key="image" uid="..." />
            <config layer="default" fontSource="DejaVuSans.ttf" fontSize="32" radius="4.5" priority="2" />
        </service>
   @endcode
 *
 * @subsection In-Out In-Out:
 * - \b image [::fwData::Image]: image containing the distance field.
 *
 * @subsection Configuration Configuration:
 * - \b layer (mandatory) : Defines distance's layer.
 * - \b fontSource (optional, default="DejaVuSans.ttf"): TrueType font (*.ttf) source file.
 * - \b fontSize (optional, default=16): Font size in points.
 * - \b radius (optional, default=4.5): Size of the distances spheres.
 * - \b interactive (optional, true/false, default=true): Enables interactions with distances.
 * - \b priority (optional, default=3): Priority of the interactor.
 * - \b queryMask (optional, uint32, default=0xFFFFFFFF): Mask used to filter out entities when the distance is auto
 * snapped.
 * - \b distanceQueryFlags (optional, uint32, default=0x40000000): Mask apply to distances spheres.
 */
class SImageMultiDistances final : public ::fwRenderOgre::IAdaptor,
                                   public ::fwRenderOgre::interactor::IInteractor
{
public:

    fwCoreServiceMacro(SImageMultiDistances, ::fwRenderOgre::IAdaptor)

    /// Initialize slots.
    VISUOGREADAPTOR_API SImageMultiDistances() noexcept;

    /// Destroys the service.
    VISUOGREADAPTOR_API virtual ~SImageMultiDistances() noexcept override final;

private:

    /// Stores Ogre ressource used to display a distance.
    /// Two spheres each attached to a node, a label to display millimeters,
    /// one line rendered with the depth check and a dashed line rendered without depth check.
    /// The point list is used to update each points when the interactor move a distance sphere,
    /// it's retrieve from the image via a field.
    struct DistanceData
    {
        ::fwData::PointList::sptr m_pointList;
        ::Ogre::SceneNode*  m_node1;
        ::Ogre::ManualObject* m_sphere1;
        ::Ogre::SceneNode* m_node2;
        ::Ogre::ManualObject* m_sphere2;
        ::Ogre::ManualObject* m_line;
        ::Ogre::ManualObject* m_dashedLine;
        ::Ogre::SceneNode* m_labelNode;
        ::fwRenderOgre::Text* m_label;
    };

    /// Stores picking informations.
    struct PickedData
    {
        DistanceData* m_data;
        bool m_first;
    };

    /// Map each distances to there related list ID.
    typedef std::map< ::fwTools::fwID::IDType, DistanceData > DistanceMap;

    /**
     * @brief Generates a color from a distance ID.
     * @param _id ID of the distance.
     * @return The generate color.
     */
    static ::Ogre::ColourValue generateColor(::fwTools::fwID::IDType _id);

    /**
     * @brief Generate a dashed line in a ::Ogre::ManualObject.
     * @param _object Object where generate the dashed line.
     * @param _begin Begin position of the line.
     * @param _end End position of the line.
     * @param _thickness Thickness of dash.
     */
    static void generateDashedLine(::Ogre::ManualObject* const _object,
                                   const ::Ogre::Vector3& _begin,
                                   const ::Ogre::Vector3& _end,
                                   float _thickness);

    /**
     * @brief Get the formated string used to display the length of a distance.
     * @return The formated string.
     */
    static std::string getLength(const ::Ogre::Vector3&, const ::Ogre::Vector3&);

    /**
     * @brief Get the normalized camera direction vector.
     * @return A vector representing the camera direction
     */
    static ::Ogre::Vector3 getCamDirection(const ::Ogre::Camera* const);

    /**
     * @brief Proposals to connect service slots to associated object signals.
     *
     * Connect ::fwData::Image::s_DISTANCE_ADDED_SIG to SImageMultiDistances::s_ADD_DISTANCE_SLOT
     * Connect ::fwData::Image::s_DISTANCE_REMOVED_SIG to SImageMultiDistances::s_REMOVE_DISTANCE_SLOT
     * Connect ::fwData::Image::s_DISTANCE_DISPLAYED_SIG to SImageMultiDistances::s_UPDATE_VISIBILITY_SLOT
     * Connect ::fwData::Image::s_MODIFIED_SIG to SImageMultiDistances::s_UPDATE_SLOT
     */
    virtual KeyConnectionsMap getAutoConnections() const override;

    /// Configures the service.
    virtual void configuring() override final;

    /// Adds the interactor to the layer and create the material.
    virtual void starting() override final;

    /// Updates materials and all distances.
    virtual void updating() override final;

    /// Removes the interactor from the layer and destroys Ogre resources.
    virtual void stopping() override final;

    /// Retrieves distances from the image and add them to the scene.
    void addDistances();

    /// Retrieves distances from the image and remove them from the scene.
    void removeDistances();

    /// Updates distances visibility from the image field.
    void updateVisibilityFromField();

    /**
     * @brief Updates distances visibility.
     * @param _visibility Set to true to show distances.
     */
    void updateVisibility(bool _visibility);

    /**
     * @brief Get the nearest picked position if there is one.
     * @param _x X screen coordinate.
     * @param _y Y screen coordinate.
     * @return The picked world coordinates.
     */
    std::optional< ::Ogre::Vector3 > getNearestPickedPosition(int _x, int _y);

    /**
     * @brief Retrieves the picked distance and store the result in m_pickedData.
     * @param _button Mousse modifier.
     * @param _x X screen coordinate.
     * @param _y Y screen coordinate.
     */
    virtual void buttonPressEvent(MouseButton _button, int _x, int _y) override final;

    /**
     * @brief Moves a distance stores in m_pickedData.
     * @param _x X screen coordinate.
     * @param _y Y screen coordinate.
     */
    virtual void mouseMoveEvent(MouseButton, int _x, int _y, int, int) override final;

    /// Resets m_pickedData.
    virtual void buttonReleaseEvent(MouseButton, int, int) override final;

    /**
     * @brief Creates a distance and add it into m_distances.
     * @param _pl The point list used to create the distance.
     */
    void createDistance(::fwData::PointList::sptr _pl);

    /**
     * @brief Updates a distance.
     * @param _data Distance to update.
     * @param _begin New begin position.
     * @param _end New end position
     */
    void updateDistance(const DistanceData* const _data, ::Ogre::Vector3 _begin, ::Ogre::Vector3 _end);

    /**
     * @brief Destroys a distance from its ID and remove it from m_distances.
     * @param _id ID of the distance.
     */
    void destroyDistance(::fwTools::fwID::IDType _id);

    /// Radius of distances spheres.
    float m_distanceSphereRadius {3.5f};

    /// Visibility of distances.
    bool m_visibility {true};

    /// True type font source file.
    std::string m_fontSource {"DejaVuSans.ttf"};

    /// Font size in points.
    size_t m_fontSize {16};

    /// Sets whether or not interactions are enabled with distances.
    bool m_interactive {true};

    /// Priority of the interactor
    int m_priority {3};

    /// Current picked data, reseted by buttonReleaseEvent(MouseButton, int, int).
    PickedData m_pickedData {nullptr, true};

    /// Mask used to filter out entities when the distance is auto snapped.
    std::uint32_t m_queryMask {0xFFFFFFFF};

    /// Mask used to filter distances, it optimise the ray launched to retrive the picked distance.
    std::uint32_t m_distanceQueryFlag {::Ogre::SceneManager::ENTITY_TYPE_MASK};

    /// Material name with no depth check for spheres.
    std::string m_sphereMaterialName;

    /// Material name with depth check for lines.
    std::string m_lineMaterialName;

    /// Material name with no depth check for dashed lines.
    std::string m_dashedLineMaterialName;

    /// Material with no depth check for spheres.
    ::fwRenderOgre::Material::uptr m_sphereMaterial {nullptr};

    /// Material with depth check for lines.
    ::fwRenderOgre::Material::uptr m_lineMaterial {nullptr};

    /// Material with no depth check for dashed lines.
    ::fwRenderOgre::Material::uptr m_dashedLineMaterial {nullptr};

    /// Stores all generates distances.
    DistanceMap m_distances;

    /** Unused IInteractor API
     *@{
     */
    virtual void focusInEvent() override final;
    virtual void focusOutEvent() override final;
    virtual void wheelEvent(int, int, int) override final;
    virtual void resizeEvent(int, int) override final;
    virtual void keyPressEvent(int) override final;
    virtual void keyReleaseEvent(int) override final;
    /**
     *@}
     */

};
}

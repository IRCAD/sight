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

namespace visuOgreAdaptor
{

/**
 * @brief Displays distances retrieved from an image.
 *
 * @section Slots Slots
 * - \b addDistance(): Add distances contains in the image from the scene manager.
 * - \b removeDistance(): Remove distances contains in the image from the scene manager.
 * - \b updateVisibility(): Updates the visibility of distances.

 * @section XML XML Configuration
 *
 * @code{.xml}
        <service uid="..." type="::visuOgreAdaptor::SImageMultiDistances" autoConnect="yes">
            <inout key="image" uid="..." />
            <config layer="default" fontSource="DejaVuSans.ttf" fontSize="32" radius="4.5" priority="1"/>
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
 * - \b priority (optional, default=1): Priority of the interactor.
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
    /// The mesh is the common data used by each spheres.
    /// The point list is used to update each points when the interactor move a distance sphere,
    /// it's retrieve from the image via a field.
    struct DistanceData
    {
        ::fwData::PointList::sptr m_pointList;
        ::Ogre::MeshPtr m_mesh;
        ::Ogre::SceneNode* m_node1;
        ::Ogre::Entity* m_sphere1;
        ::Ogre::SceneNode* m_node2;
        ::Ogre::Entity* m_sphere2;
        ::Ogre::ManualObject* m_line;
        ::fwRenderOgre::Text* m_label;
        ::Ogre::ManualObject* m_dashedLine;
    };

    /// Stores picking informations.
    struct PickedData
    {
        ::fwTools::fwID::IDType m_id;
        bool m_first;
        float m_distance;
    };

    /// Map each distances to there related list ID.
    typedef std::map<::fwTools::fwID::IDType, DistanceData> DistanceMap;

    /**
     * @brief Get the normalized camera direction vector.
     * @return A vector representing the camera direction
     */
    static ::Ogre::Vector3 getCamDirection(const ::Ogre::Camera* const);

    /**
     * @brief Generate a dashed line in a ::Ogre::ManualObject.
     * @param _object Object where generate the dashed line.
     * @param _begin Begin position of the line.
     * @param _end End position of the line.
     * @param _thickness Thickness of dash.
     */
    static void generateDashedLine(::Ogre::ManualObject* const _object, const ::Ogre::Vector3& _begin,
                                   const ::Ogre::Vector3& _end, float _thickness);

    /**
     * @brief Generates a color from a distance ID.
     * @param _id ID of the distance.
     * @return A generate color.
     */
    static ::Ogre::ColourValue generateColor(::fwTools::fwID::IDType _id);

    /**
     * @brief Get the formated string used to display the length of a distance.
     * @return The formated string.
     */
    static std::string getLength(const ::Ogre::Vector3&, const ::Ogre::Vector3&);

    /// Configures the service.
    void configuring() override final;

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect ::fwData::Image::s_DISTANCE_ADDED_SIG to SImageMultiDistances::s_ADD_DISTANCE_SLOT
     * Connect ::fwData::Image::s_DISTANCE_REMOVED_SIG to SImageMultiDistances::s_REMOVE_DISTANCE_SLOT
     * Connect ::fwData::Image::s_DISTANCE_DISPLAYED_SIG to SImageMultiDistances::s_UPDATE_VISIBILITY_SLOT
     * Connect ::fwData::Image::s_MODIFIED_SIG to SImageMultiDistances::s_UPDATE_SLOT
     */
    virtual KeyConnectionsMap getAutoConnections() const override;

    /// Adds the interactor to the layer and create the material.
    virtual void starting() override final;

    /// Does nothing.
    virtual void updating() override final;

    /// Removes the interactor from the layer and destroys Ogre resources.
    virtual void stopping() override final;

    /// Retrieves distances from the image and add it to the scene.
    void addDistances();

    /// Retrieves distances from the image and remove it to the scene.
    void removeDistances();

    /// Updates distances visibility.
    void updateVisibility();

    /**
     * @brief Moves a distance point stores in m_pickedData.
     * @param _x X screen coordinate.
     * @param _y Y screen coordinate.
     */
    virtual void mouseMoveEvent(MouseButton, int _x, int _y, int, int) override final;

    /// Resets m_pickedData.
    virtual void buttonReleaseEvent(MouseButton, int, int) override final;

    /**
     * @brief Retrieves the picked entity and store the result in m_pickedData.
     * @param _button Mousse modifier.
     * @param _x X screen coordinate.
     * @param _y Y screen coordinate.
     */
    virtual void buttonPressEvent(MouseButton _button, int _x, int _y) override final;

    /**
     * @brief Creates a distance and add it into m_distances.
     * @param _pl The point list used to create the distance.
     */
    void createDistance(::fwData::PointList::sptr _pl);

    /**
     * @brief Updates a distance from its ID.
     * @param _id ID of the distance.
     * @param _begin Begin position.
     * @param _end End position
     */
    void updateDistance(::fwTools::fwID::IDType _id, ::Ogre::Vector3 _begin, ::Ogre::Vector3 _end);

    /**
     * @brief Destroys a distance from its ID and remove it from m_distances.
     * @param _id ID of the distance.
     */
    void destroyDistance(::fwTools::fwID::IDType _id);

    /**
     * @brief Retrieves a ::Ogre::MovableObject from picking coordinates.
     * @param _x X screen coordinate.
     * @param _y Y screen coordinate.
     * @return The picked ::Ogre::MovableObject or nullptr if nothing was found.
     */
    ::Ogre::MovableObject* pickObject(int _x, int _y);

    /// Radius of distances spheres.
    float m_distanceSphereRadius {4.5f};

    /// Visibility of distances.
    bool m_visibility {true};

    /// TrueType font source file.
    std::string m_fontSource { "DejaVuSans.ttf" };

    /// Font size in points.
    size_t m_fontSize {16};

    /// Priority of the interactor
    int m_priority {1};

    /// Material name with depth check.
    std::string m_depthMaterialName;

    /// Material name with no depth check.
    std::string m_noDepthMaterialName;

    /// Material used to display distances
    ::fwRenderOgre::Material::uptr m_depthMaterial {nullptr};

    /// Material used to display the full line of the distance.
    ::fwRenderOgre::Material::uptr m_noDepthMaterial {nullptr};

    /// Current picked data, reseted by buttonReleaseEvent(MouseButton, int, int).
    PickedData m_pickedData;

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

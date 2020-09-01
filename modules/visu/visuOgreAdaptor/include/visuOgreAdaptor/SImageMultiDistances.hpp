/************************************************************************
 *
 * Copyright (C) 2019-2020 IRCAD France
 * Copyright (C) 2019-2020 IHU Strasbourg
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
 * @brief This adaptor displays distances retrieved from an image fields.
 *
 * @section Slots Slots
 * - \b addDistance(): adds distances contained in the image from the scene manager.
 * - \b removeDistance(): removes distances contained in the image from the scene manager.
 * - \b updateVisibilityFromField(): updates the visibility of distances from the field status.
 * - \b updateVisibility(bool): sets whether distances are shown or not.
 * - \b toggleVisibility(): toggles whether distances are shown or not.
 * - \b show(): shows distance.
 * - \b hide(): hides distance.
 *
 * @section XML XML Configuration
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
 * - \b layer (mandatory, string) : defines distance's layer.
 * - \b fontSource (optional, string, default=DejaVuSans.ttf): TrueType font (*.ttf) source file.
 * - \b fontSize (optional, int, default=16): font size in points.
 * - \b radius (optional, float, default=4.5): size of the distances spheres.
 * - \b interactive (optional, bool, default=true): enables interactions with distances.
 * - \b priority (optional, int, default=3): priority of the interactor.
 * - \b queryMask (optional, uint32, default=0xFFFFFFFF): mask used to filter out entities when the distance is auto
 *      snapped.
 * - \b distanceQueryFlags (optional, uint32, default=0x40000000): mask apply to distances spheres.
 */
class SImageMultiDistances final :
    public ::fwRenderOgre::IAdaptor,
    public ::fwRenderOgre::interactor::IInteractor
{

public:

    /// Generates default methods as New, dynamicCast, ...
    fwCoreServiceMacro(SImageMultiDistances, ::fwRenderOgre::IAdaptor)

    /// Initialize slots.
    VISUOGREADAPTOR_API SImageMultiDistances() noexcept;

    /// Destroys the service.
    VISUOGREADAPTOR_API ~SImageMultiDistances() noexcept override;

protected:

    /// Configures the service.
    VISUOGREADAPTOR_API void configuring() override;

    /// Adds the interactor to the layer and creates the material.
    VISUOGREADAPTOR_API void starting() override;

    /**
     * @brief Proposals to connect service slots to associated object signals.
     *
     * Connect ::fwData::Image::s_DISTANCE_ADDED_SIG to s_ADD_DISTANCE_SLOT
     * Connect ::fwData::Image::s_DISTANCE_REMOVED_SIG to s_REMOVE_DISTANCE_SLOT
     * Connect ::fwData::Image::s_DISTANCE_DISPLAYED_SIG to s_UPDATE_VISIBILITY_SLOT
     * Connect ::fwData::Image::s_MODIFIED_SIG to s_UPDATE_SLOT
     */
    VISUOGREADAPTOR_API KeyConnectionsMap getAutoConnections() const override;

    /// Updates materials and all distances.
    VISUOGREADAPTOR_API void updating() override;

    /// Removes the interactor from the layer and destroys Ogre resources.
    VISUOGREADAPTOR_API void stopping() override;

    /**
     * @brief Sets distances visibility.
     * @param _visible the visibility status of distances.
     */
    VISUOGREADAPTOR_API void setVisible(bool _visible) override;

private:

    /// Stores Ogre resources used to display a distance.
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
     * @return The generated color.
     */
    static ::Ogre::ColourValue generateColor(::fwTools::fwID::IDType _id);

    /**
     * @brief Generates a dashed line in a ::Ogre::ManualObject.
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
     * @brief Gets the formated string used to display the length of a distance.
     * @return The formated string.
     */
    static std::string getLength(const ::Ogre::Vector3&, const ::Ogre::Vector3&);

    /**
     * @brief Gets the normalized camera direction vector.
     * @return A vector representing the camera direction
     */
    static ::Ogre::Vector3 getCamDirection(const ::Ogre::Camera* const);

    /// Retrieves distances from the image and adds them to the scene.
    void addDistances();

    /// Retrieves distances from the image and remove them from the scene.
    void removeDistances();

    /// Updates distances visibility from the image field.
    void updateVisibilityFromField();

    /**
     * @brief Gets the nearest picked position if there is one.
     * @param _x X screen coordinate.
     * @param _y Y screen coordinate.
     * @return The picked world coordinates.
     */
    std::optional< ::Ogre::Vector3 > getNearestPickedPosition(int _x, int _y);

    /**
     * @brief Retrieves the picked distance and stores the result in m_pickedData.
     * @param _button Mousse modifier.
     * @param _x X screen coordinate.
     * @param _y Y screen coordinate.
     */
    virtual void buttonPressEvent(MouseButton _button, int _x, int _y) override;

    /**
     * @brief Moves a distance stores in m_pickedData.
     * @param _x X screen coordinate.
     * @param _y Y screen coordinate.
     */
    virtual void mouseMoveEvent(MouseButton, int _x, int _y, int, int) override;

    /// Resets m_pickedData.
    virtual void buttonReleaseEvent(MouseButton, int, int) override;

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

    /// Defines the radius of distances spheres.
    float m_distanceSphereRadius { 3.5f };

    /// Defines the TrueType font source file.
    std::string m_fontSource { "DejaVuSans.ttf" };

    /// Defines the font size in points.
    size_t m_fontSize { 16 };

    /// Defines whether or not interactions are enabled with distances.
    bool m_interactive { true };

    /// Defines the priority of the interactor.
    int m_priority { 2 };

    /// Defines the current picked data, reseted by buttonReleaseEvent(MouseButton, int, int).
    PickedData m_pickedData { nullptr, true };

    /// Defines the mask used to filter out entities when the distance is auto snapped.
    std::uint32_t m_queryMask { 0xFFFFFFFF };

    /// Defines the mask used to filter distances, it optimizes the ray launched to retrive the picked distance.
    std::uint32_t m_distanceQueryFlag { ::Ogre::SceneManager::ENTITY_TYPE_MASK };

    /// Defines the material name with no depth check for spheres.
    std::string m_sphereMaterialName;

    /// Defines the material name with depth check for lines.
    std::string m_lineMaterialName;

    /// Defines the material name with no depth check for dashed lines.
    std::string m_dashedLineMaterialName;

    /// Contains the material with no depth check for spheres.
    ::fwRenderOgre::Material::uptr m_sphereMaterial {nullptr};

    /// Contains the material with depth check for lines.
    ::fwRenderOgre::Material::uptr m_lineMaterial {nullptr};

    /// Contains the material with no depth check for dashed lines.
    ::fwRenderOgre::Material::uptr m_dashedLineMaterial {nullptr};

    /// Stores all generatesd distances.
    DistanceMap m_distances;

};

} // namespace visuOgreAdaptor.

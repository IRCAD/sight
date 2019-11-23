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
#include <fwRenderOgre/interactor/ImageMultiDistancesInteractor.hpp>
#include <fwRenderOgre/Material.hpp>
#include <fwRenderOgre/Text.hpp>

#include <OgreVector3.h>

#include <memory>

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
class SImageMultiDistances final : public ::fwRenderOgre::IAdaptor
{
public:

    fwCoreServiceMacro(SImageMultiDistances, ::fwRenderOgre::IAdaptor)

    /// Initialize slots.
    VISUOGREADAPTOR_API SImageMultiDistances() noexcept;

    /// Destroys the service.
    VISUOGREADAPTOR_API virtual ~SImageMultiDistances() noexcept override final;

private:

    typedef ::fwRenderOgre::interactor::ImageMultiDistancesInteractor::DistanceData DistanceData;
    typedef ::fwRenderOgre::interactor::ImageMultiDistancesInteractor::DistanceMap DistanceMap;

    /**
     * @brief Generates a color from a distance ID.
     * @param _id ID of the distance.
     * @return A generate color.
     */
    static ::Ogre::ColourValue generateColor(::fwTools::fwID::IDType _id);

    /// Configures the service.
    virtual void configuring() override final;

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

    /// Updates materials and all distances.
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
     * @brief Creates a distance and add it into m_distances.
     * @param _pl The point list used to create the distance.
     */
    void createDistance(::fwData::PointList::sptr _pl);

    /**
     * @brief Destroys a distance from its ID and remove it from m_distances.
     * @param _id ID of the distance.
     */
    void destroyDistance(::fwTools::fwID::IDType _id);

    /// Radius of distances spheres.
    float m_distanceSphereRadius {4.5f};

    /// Visibility of distances.
    bool m_visibility {true};

    /// TrueType font source file.
    std::string m_fontSource {"DejaVuSans.ttf"};

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

    /// Stores all generates distances.
    DistanceMap m_distances;

    /// Interactor used to move distances spheres.
    std::shared_ptr< ::fwRenderOgre::interactor::ImageMultiDistancesInteractor > m_interactor {nullptr};

};
}

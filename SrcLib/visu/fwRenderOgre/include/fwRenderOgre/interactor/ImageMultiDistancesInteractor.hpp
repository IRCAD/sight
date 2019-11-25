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

#include "fwRenderOgre/interactor/IInteractor.hpp"
#include <fwRenderOgre/Text.hpp>

#include <fwCore/macros.hpp>

#include <fwData/PointList.hpp>

#include <OGRE/OgreMovableObject.h>
#include <OGRE/OgreSceneNode.h>

namespace fwRenderOgre
{

namespace interactor
{

/**
 * @brief Picks distances and updates it.
 */
class FWRENDEROGRE_CLASS_API ImageMultiDistancesInteractor : public IInteractor
{

public:

    /// Stores Ogre ressource used to display a distance.
    /// Two spheres each attached to a node, a label to display millimeters,
    /// one line rendered with the depth check and a dashed line rendered without depth check.
    /// The point list is used to update each points when the interactor move a distance sphere,
    /// it's retrieve from the image via a field.
    struct DistanceData
    {
        ::fwData::PointList::sptr m_pointList;
        ::Ogre::SceneNode* m_node1;
        ::Ogre::ManualObject* m_sphere1;
        ::Ogre::SceneNode* m_node2;
        ::Ogre::ManualObject* m_sphere2;
        ::Ogre::ManualObject* m_line;
        ::Ogre::ManualObject* m_dashedLine;
        ::Ogre::SceneNode* m_labelNode;
        ::fwRenderOgre::Text* m_label;
    };

    /// Map each distances to there related list ID.
    typedef std::map< ::fwTools::fwID::IDType, DistanceData > DistanceMap;

    /**
     * @brief Generate a dashed line in a ::Ogre::ManualObject.
     * @param _object Object where generate the dashed line.
     * @param _begin Begin position of the line.
     * @param _end End position of the line.
     * @param _thickness Thickness of dash.
     */
    FWRENDEROGRE_API static void generateDashedLine(::Ogre::ManualObject* const _object,
                                                    const ::Ogre::Vector3& _begin,
                                                    const ::Ogre::Vector3& _end,
                                                    float _thickness);

    /**
     * @brief Get the formated string used to display the length of a distance.
     * @return The formated string.
     */
    FWRENDEROGRE_API static std::string getLength(const ::Ogre::Vector3&, const ::Ogre::Vector3&);

    /**
     * @brief Constructor.
     * @param _layer Layer of the interactor.
     * @param _distances Distances shared between the service and this interactor.
     * @param _radius Radius of the spheres used to display distances.
     */
    FWRENDEROGRE_API ImageMultiDistancesInteractor(SPTR(Layer)_layer, DistanceMap& _distances, float _radius) noexcept;

    /// Destructor.
    FWRENDEROGRE_API virtual ~ImageMultiDistancesInteractor() noexcept override final;

private:

    /// Stores picking informations.
    struct PickedData
    {
        DistanceData* m_data;
        bool m_first;
    };

    /**
     * @brief Get the normalized camera direction vector.
     * @return A vector representing the camera direction
     */
    static ::Ogre::Vector3 getCamDirection(const ::Ogre::Camera* const);

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
     * @brief Updates a distance.
     * @param _data Distance to update.
     * @param _begin New begin position.
     * @param _end New end position
     */
    void updateDistance(const DistanceData* const _data, ::Ogre::Vector3 _begin, ::Ogre::Vector3 _end);

    /// Radius of distances spheres.
    float m_distanceSphereRadius {4.5f};

    /// Current picked data, reseted by buttonReleaseEvent(MouseButton, int, int).
    PickedData m_pickedData {nullptr, true};

    /// Distances shared between the service and this interactor.
    DistanceMap& m_distances;

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
} // namespace interactor
} // namespace fwRenderOgre

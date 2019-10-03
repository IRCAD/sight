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
#include "visuOgreAdaptor/SLine.hpp"
#include "visuOgreAdaptor/SPointList.hpp"

#include <fwData/Vector.hpp>

#include <fwDataTools/helper/MedicalImage.hpp>

#include <fwRenderOgre/IAdaptor.hpp>
#include <fwRenderOgre/interactor/IInteractor.hpp>
#include <fwRenderOgre/Text.hpp>

namespace visuOgreAdaptor
{

/**
 * @brief Adaptor to display distance on an image
 *
 * @section Slots Slots
 * - \b removeDistance: Detection when a point is removed from an image
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
        <service uid="multiDistancesAdp" type="::visuOgreAdaptor::SImageMultiDistances" autoConnect="yes">
            <inout key="image" uid="${image}" />
            <config layer="default" fontSource="DejaVuSans.ttf" fontSize="12" />
        </service>
   @endcode
 * @subsection In-Out In-Out
 * - \b image [::fwData::Image]: image containing the distance field.
 * @subsection Configuration Configuration
 * - \b layer(mandatory) : defines distance's layer.
 * - \b fontSource (optional, default="DejaVuSans.ttf"): TrueType font (*.ttf) source file.
 * - \b fontSize (optional, default=16): font size in points.
 */

class SImageMultiDistances : public ::fwRenderOgre::IAdaptor,
                             public ::fwRenderOgre::interactor::IInteractor
{
public:

    fwCoreServiceMacro(SImageMultiDistances, ::fwRenderOgre::IAdaptor);

    /// Constructor
    VISUOGREADAPTOR_API SImageMultiDistances() noexcept;

    /// Destructor
    VISUOGREADAPTOR_API ~SImageMultiDistances() noexcept;

    /// Called when the mouse is moved
    VISUOGREADAPTOR_API virtual void mouseMoveEvent(MouseButton button, int x, int y, int dx, int dy) override;

    /// Called on a wheel event
    VISUOGREADAPTOR_API virtual void wheelEvent(int, int, int) override;

    /// Called when the window is resized
    VISUOGREADAPTOR_API virtual void resizeEvent(int, int) override;

    /// Called when a key is pressed
    VISUOGREADAPTOR_API virtual void keyPressEvent(int) override;

    /// Called when a key is released
    VISUOGREADAPTOR_API virtual void keyReleaseEvent(int) override;

    /// Called when a mouse button is released.
    VISUOGREADAPTOR_API virtual void buttonReleaseEvent(MouseButton button, int x, int y) override;

    /// Called when a mouse button is pressed.
    VISUOGREADAPTOR_API virtual void buttonPressEvent(MouseButton button, int x, int y) override;

    /// Called when the focus is win
    VISUOGREADAPTOR_API virtual void focusInEvent() override;

    /// Called when the focus is lost
    VISUOGREADAPTOR_API virtual void focusOutEvent() override;

protected:

    /// Configure the adaptor
    VISUOGREADAPTOR_API void configuring() override;

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect Image::s_DISTANCE_ADDED_SIG to this::s_UPDATE_SLOT
     * Connect Image::s_DISTANCE_REMOVED_SIG to this::s_REMOVE_DISTANCE_SLOT
     * Connect Image::s_DISTANCE_DISPLAYED_SIG to this::s_SHOW_DISTANCE_SLOT
     */
    VISUOGREADAPTOR_API virtual KeyConnectionsMap getAutoConnections() const override;

    /// Initialize sceneManager, rootSceneNode, layer and update the service
    VISUOGREADAPTOR_API void starting() override;

    /// Update distance displaying and set the spacing to the image coord
    VISUOGREADAPTOR_API void updating() override;

    /// Stop the service
    VISUOGREADAPTOR_API void stopping() override;

private:

    /// Display line of a given pointlist
    void createDistance(float begin[3], float end[3], size_t id, const ::Ogre::ColourValue& color);

    /// Remove the line corresponding to a specific ID
    void deleteDistance(size_t lineID);

    /// Remove lines
    void deleteDistances();

    /// Remove origin in the image coord
    void addCurrentOrigin() const;

    /// Set origin in the image coord
    void removeCurrentOrigin() const;

    /// Create millimeter length label of a specific point. The second argument corresponds to the distance
    void createMillimeterLabel(const float point[3], const Ogre::Real distance, size_t id,
                               const ::Ogre::ColourValue color);

    /// Delete millimeter length label of a specific point.
    void deleteMillimeterLabel(size_t id);

    /// Create ID label of a given point
    void createIdLabel(const float ps1[3], size_t id, const ::Ogre::ColourValue& color);

    /// Destroy label corresponding on a specific id
    void deleteIdLabel(size_t id);

    /**
     * @brief Creates a new line and creates another line to know if you are in front or behind the image.
     * In the case of the second line, the line should only be displayed if you are in front.
     *
     * @param materialAdp corresponds to the material adaptor.
     * @param ps1 corresponds to the coordinates of the first point.
     * @param ps2 corresponds to the coordinates of the second point.
     * @param color corresponds to the color of the line.
     */
    void createLine(const ::visuOgreAdaptor::SMaterial::sptr materialAdp, const float ps1[3],
                    const float ps2[3], size_t id, const ::Ogre::ColourValue& color) const;

    /// Deletes a specific line
    void deleteLine(size_t id) const;

    /**
     * @brief Create a new sphere manual object.
     *
     * @param materialAdp corresponds to the material adaptor.
     * @param name corresponds to the name of the node.
     * @param color corresponds to the color of the sphere.
     */
    ::Ogre::ManualObject* createSphere(const ::visuOgreAdaptor::SMaterial::sptr materialAdp,
                                       const std::string name, const ::Ogre::ColourValue& color) const;

    /// Destroys all resources used by this service
    void deleteAllRessources();

    /// Generate a color from an id
    ::Ogre::ColourValue generateColor(size_t id) const;

    /// Set and return the material adaptor
    ::visuOgreAdaptor::SMaterial::sptr setMaterialAdp();

    /// Get the negato orientation based on the camera direction
    ::fwDataTools::helper::MedicalImage::Orientation getOrientation() const;

    /// Picks the object at screen coordinates (x, y)
    Ogre::MovableObject* pickObject(int x, int y);

    /// Find and return the id of the name
    size_t findLineID(const std::string& name) const;

    /**
     * @brief Check if the mouse click on an existing point..
     *
     * @param x corresponds corresponds to the coordinates X of a point.
     * @param y corresponds to the coordinates Y of a point.
     */
    bool clickPoint(int x, int y);

    /// Scene manager of the scene
    ::Ogre::SceneManager* m_sceneMgr { nullptr };

    /// Root Scene Node
    ::Ogre::SceneNode* m_rootSceneNode { nullptr };

    /// Scene node where point 1 is attached
    ::Ogre::SceneNode* m_sphere1Node { nullptr };

    /// Scene node where point 2 is attached
    ::Ogre::SceneNode* m_sphere2Node { nullptr };

    /// Used to store label of each point
    std::vector< ::fwRenderOgre::Text*> m_labels {};

    /// Used to store the millimeter value for each line
    std::vector< ::fwRenderOgre::Text*> m_millimeterValue {};

    /// Used to store the id value labels points nodes
    std::vector< ::Ogre::SceneNode*> m_labelNodes {};

    /// Used to store the length value labels points nodes
    std::vector< ::Ogre::SceneNode*> m_millimeterNodes {};

    /// Copy of distanceField in case that the signal removed the lign of the image before updating the visual
    ::fwData::Vector::sptr m_distanceField { };

    /// Number of existing lines
    size_t m_distanceNb { 0 };

    /// Id of the point which will move
    size_t m_moveID  { 0 };

    /// Position (begin/back) of the point which will move
    size_t m_isBeginMove { 0 };

    /// Defines whether interaction is possible or not.
    bool m_activeInteraction { false };

    /// Position of the first point
    float m_ps1[3] { 0, 0, 0 };

    /// Position of the second point
    float m_ps2[3] { 0, 0, 0 };

    /// The picker used by this interactor.
    ::fwRenderOgre::picker::IPicker m_picker;

    /// Material Adaptor
    ::visuOgreAdaptor::SMaterial::sptr m_materialAdp {nullptr};

    /// Pointer to the material data
    ::fwData::Material::sptr m_material {nullptr};

    /// TrueType font source file.
    std::string m_fontSource { "DejaVuSans.ttf" };

    /// Font size in points.
    size_t m_fontSize { 16 };

};
}

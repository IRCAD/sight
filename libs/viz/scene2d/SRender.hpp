/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "viz/scene2d/config.hpp"
#include "viz/scene2d/data/Axis.hpp"
#include "viz/scene2d/data/Event.hpp"
#include "viz/scene2d/data/Viewport.hpp"
#include "viz/scene2d/vec2d.hpp"

#include <core/com/helper/SigSlotConnection.hpp>

#include <service/helper/Config.hpp>

#include <viz/base/IRender.hpp>

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QResizeEvent>
#include <Qt>

#include <map>

namespace sight::viz::scene2d
{

// Not declared in the #include because of mutual inclusion.
class Scene2DGraphicsView;

// Not declared in the #include because of mutual inclusion.
class IAdaptor;

/**
 * @brief Qt scene rendering class.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service uid="GENERIC_UID_Scene2D" type="sight::viz::scene2d::SRender" >
       <scene>
           <scene x="-1100" y="-1.1" width="2500" height="1.2" background="#FFFFFF" />
           <axis id="xAxis" origin="0.0" scale="1.0" scaleType="LINEAR" />
           <axis id="yAxis" origin="0.0" scale="-1.0" scaleType="LINEAR" />
           <axis id="axeHistogramY" origin="0.0" scale="-0.000005" scaleType="LINEAR" />

           <adaptor uid="grid" />
       </scene>
   </service>
   @endcode
 *
 * @subsection Configuration Configuration
 *
 * - \<scene x="-1100" y="-1.1" width="2500" height="1.2" /\> : Set the scene coordinates
 *   - \b x: set the x coordinate of the top left scene corner
 *   - \b y: set the y coordinate of the top left scene corner
 *   - \b width: set the width of the scene
 *   - \b height: set the height of the scene
 *   - \b antialiasing (optional, default: false): activate scene antialiasing if attribute's value set to 'true'
 *   - \b aspectRatioMode (optional, default: IgnoreAspectRatio): defines what happens to the aspect ratio
 *     when scaling an rectangle (See https://doc.qt.io/qt-5/qt.html#AspectRatioMode-enum).
 *   - \b background (optional, default: #000000): the background color of the rendering screen.
 *        The color value must be defined as a hexadecimal value (ex : \#ffffff for white).
 * - \<axis id="xAxis" origin="0.0" scale="1.0" scaleType="LINEAR" /\> : Set an axis specifications
 *   - \b id: set the axis id
 *   - \b origin: set the axis origin
 *   - \b scale: set the axis scale
 *   - \b scaleType: set the axis scaleType
 * - \<adaptor uid="grid" /\> : Set an adaptor
 *   - \b uid: set the adaptor uid
 *
 */
class VIZ_SCENE2D_CLASS_API SRender : public viz::base::IRender
{
public:

    SIGHT_DECLARE_SERVICE(SRender, viz::base::IRender);

    /// A string type representing adaptors id.
    typedef std::string AdaptorIDType;

    /// A string type representing objects id.
    typedef std::string ObjectIDType;

    /// A float type representing adaptors zValue.
    typedef float AdaptorZValueType;

    /// Constructor, set coordinates m_sceneStart to (-100, -100), m_sceneWidth to (200, 200) and add new handle events
    ///  ADDED_KEYS, REMOVED_KEYS and CHANGED_KEYS.
    VIZ_SCENE2D_API SRender() noexcept;

    /// Basic destructor, do nothing.
    VIZ_SCENE2D_API ~SRender() noexcept override;

    /// Get the scene.
    VIZ_SCENE2D_API QGraphicsScene* getScene() const;

    /// Get the view.
    VIZ_SCENE2D_API Scene2DGraphicsView* getView() const;

    /// Get the axis.
    VIZ_SCENE2D_API scene2d::data::Axis::sptr getAxis(const std::string& id) const;

    /// If the event hasn't been accepted yet, call the adaptor processInteraction function.
    VIZ_SCENE2D_API void dispatchInteraction(scene2d::data::Event& _event);

    /// Returns true if given coordinates are contained in the view of the scene
    VIZ_SCENE2D_API bool contains(const scene2d::vec2d_t& coord) const;

    /** @brief Compute the viewport coordinates mapped to scene coordinates.
     * @param coord input viewport coordinates
     * @param clip clip the returned coordinates to the size of the scene
     * @return scene coordinates
     */
    VIZ_SCENE2D_API scene2d::vec2d_t mapToScene(const scene2d::vec2d_t& coord, bool clip = false) const;

    /// Returns what happens to scene's aspect ratio on view resize events
    VIZ_SCENE2D_API Qt::AspectRatioMode getAspectRatioMode() const;

    /// Update scene size from items bounding rect, this bounding can be enlarged with ratioPercent parameter
    VIZ_SCENE2D_API void updateSceneSize(float ratioPercent = 0);

protected:

    /// Get configuration options from XML
    VIZ_SCENE2D_API void configuring() override;

    /// Call startContext to set the scene, the viewport and the view,
    /// and add'em to the QtContainer and start the adaptors
    /// contained in the adaptors id vector of the ObjectsID2AdaptorIDVector map.
    VIZ_SCENE2D_API void starting() override;

    /// Do nothing.
    VIZ_SCENE2D_API void updating() override;

    /// Stop all the adaptors attached to the render related composite,
    /// stop all those attached to the objects contained
    /// by the render related composite, clear the maps and call stopContext().
    VIZ_SCENE2D_API void stopping() override;

private:

    /// A ConfigurationElement type representing a configuration.
    typedef SPTR(core::runtime::ConfigurationElement) ConfigurationType;

    /// Create the QtContainer, the scene, the viewport, the view.
    void startContext();

    /// Delete m_scene and m_view;
    void stopContext();

    /// Get the axis configuration specifications
    void configureAxis(ConfigurationType _conf);

    /// Get the scene configuration specifications and set them to m_sceneStart and m_sceneWidth.
    void configureScene(ConfigurationType _conf);

    /// Get the adaptor configuration specifications, push back the id attribute in the m_objectsID2AdaptorIDVector map,
    /// create a SceneAdaptor2D,
    /// set its attributes and push it back in the m_adaptorID2SceneAdaptor2D map.
    void configureAdaptor(ConfigurationType _conf);

    std::map<std::string, scene2d::data::Axis::sptr> m_axisMap;

    /// The render configuration.
    ConfigurationType m_sceneConfiguration;

    /// Coordinates of the scene top left corner.
    scene2d::vec2d_t m_sceneStart;

    /// Width and height of the scene.
    scene2d::vec2d_t m_sceneWidth;

    /// The scene.
    QGraphicsScene* m_scene {nullptr};

    /// The view.
    Scene2DGraphicsView* m_view {nullptr};

    /// If antialiasing is requested (deactivated by default because of a potential lack of performance)
    bool m_antialiasing {false};

    /// Background color of the scene.
    std::string m_background;

    /// How the scene should behave on view resize events
    Qt::AspectRatioMode m_aspectRatioMode {Qt::IgnoreAspectRatio};
};

} // namespace sight::viz::scene2d

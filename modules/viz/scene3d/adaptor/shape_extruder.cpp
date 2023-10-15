/************************************************************************
 *
 * Copyright (C) 2020-2023 IRCAD France
 * Copyright (C) 2020-2021 IHU Strasbourg
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

#include "modules/viz/scene3d/adaptor/shape_extruder.hpp"

#include <core/com/signal.hxx>
#include <core/com/slots.hxx>

#include <data/mesh.hpp>
#include <data/reconstruction.hpp>
#include <data/tools/color.hpp>

#include <geometry/data/mesh.hpp>

#include <viz/scene3d/ogre.hpp>

#include <OgreCamera.h>

namespace sight::module::viz::scene3d::adaptor
{

static const core::com::slots::key_t s_ENABLE_TOOL_SLOT       = "enableTool";
static const core::com::slots::key_t s_DELETE_LAST_MESH_SLOT  = "deleteLastMesh";
static const core::com::slots::key_t s_CANCEL_LAST_CLICK_SLOT = "cancelLastClick";
static const core::com::slots::key_t s_RESET_SLOT             = "reset";
static const core::com::slots::key_t s_VALIDATE_SLOT          = "validate";

static const core::com::slots::key_t TOOL_DISABLED_SIG = "toolDisabled";

shape_extruder::Triangle2D::Triangle2D(
    const Ogre::Vector2& _a,
    const Ogre::Vector2& _b,
    const Ogre::Vector2& _c
) :
    a(_a),
    b(_b),
    c(_c),
    id(s_id++)
{
    // Matrix to rotate of 90 degree over the Z axis.
    const Ogre::Matrix3 quarterRotation {0, -1, 0, 1, 0, 0, 0, 0, 1};

    // Compute the circumscribed circle of the triangle.
    Ogre::Vector2 firstBisectorPos;
    Ogre::Vector2 firstBisectorDir;

    if(a.y != b.y)
    {
        firstBisectorPos = (a + b) / 2.F;
        const Ogre::Vector2 firstEdgeDir = (a - b).normalisedCopy();
        firstBisectorDir = (quarterRotation * Ogre::Vector3(firstEdgeDir.x, firstEdgeDir.y, 0)).xy();
    }
    else
    {
        firstBisectorPos = (c + b) / 2.F;
        const Ogre::Vector2 firstEdgeDir = (c - b).normalisedCopy();
        firstBisectorDir = (quarterRotation * Ogre::Vector3(firstEdgeDir.x, firstEdgeDir.y, 0)).xy();
    }

    Ogre::Vector2 secondBisectorPos;
    Ogre::Vector2 secondBisectorDir;

    if(a.y != c.y)
    {
        secondBisectorPos = (a + c) / 2.F;
        Ogre::Vector2 secondEdgeDir = (a - c).normalisedCopy();
        secondBisectorDir =
            (quarterRotation * Ogre::Vector3(secondEdgeDir.x, secondEdgeDir.y, 0)).xy();
    }
    else
    {
        secondBisectorPos = (b + c) / 2.F;
        Ogre::Vector2 secondEdgeDir = (b - c).normalisedCopy();
        secondBisectorDir =
            (quarterRotation * Ogre::Vector3(secondEdgeDir.x, secondEdgeDir.y, 0)).xy();
    }

    const float aPrim = firstBisectorDir.y / firstBisectorDir.x;
    const float aSec  = secondBisectorDir.y / secondBisectorDir.x;

    const float bPrim = firstBisectorPos.y - aPrim * firstBisectorPos.x;
    const float bSec  = secondBisectorPos.y - aSec * secondBisectorPos.x;

    const float intersectX = (bSec - bPrim) / (aPrim - aSec);
    const float intersectY = aPrim * intersectX + bPrim;

    center = Ogre::Vector2(intersectX, intersectY);
    radius = a.distance(center);

    const Ogre::Vector2 baryDir(((a + b) / 2.F) - c);
    barycentre = c + 2.F / 3.F * baryDir;
}

//------------------------------------------------------------------------------

bool shape_extruder::Edge::intersect(Edge _edge) const
{
    const Ogre::Vector2 p = a;
    const Ogre::Vector2 r = (b - a);

    const Ogre::Vector2 q = _edge.a;
    const Ogre::Vector2 s = (_edge.b - _edge.a);

    const Ogre::Vector2 qp = (q - p);
    const float qpXs       = (qp.x * s.y - qp.y * s.x);
    const float qpXr       = (qp.x * r.y - qp.y * r.x);
    const float rXs        = (r.x * s.y - r.y * s.x);
    const float t          = qpXs / rXs;
    const float u          = qpXr / rXs;

    return rXs != 0 && t >= 0 && t < 1 && u >= 0 && u < 1;
}

//------------------------------------------------------------------------------

Ogre::Vector3 shape_extruder::getCamDirection(const Ogre::Camera* const _cam)
{
    const Ogre::Matrix4 view = _cam->getViewMatrix();
    Ogre::Vector3 direction(view[2][0], view[2][1], view[2][2]);
    direction.normalise();
    return -direction;
}

//-----------------------------------------------------------------------------

shape_extruder::shape_extruder() noexcept :
    service::notifier(m_signals)
{
    new_slot(s_ENABLE_TOOL_SLOT, &shape_extruder::enableTool, this);
    new_slot(s_DELETE_LAST_MESH_SLOT, &shape_extruder::deleteLastMesh, this);
    new_slot(s_CANCEL_LAST_CLICK_SLOT, &shape_extruder::cancelLastClick, this);
    new_slot(s_RESET_SLOT, &shape_extruder::reset, this);
    new_slot(s_VALIDATE_SLOT, &shape_extruder::validate, this);

    m_toolDisabledSig = this->new_signal<core::com::signal<void()> >(TOOL_DISABLED_SIG);
}

//-----------------------------------------------------------------------------

void shape_extruder::configuring()
{
    this->configureParams();

    const config_t config = this->get_config();

    static const std::string s_PRIORITY_CONFIG         = s_CONFIG + "priority";
    static const std::string s_EXTRUDE_CONFIG          = s_CONFIG + "extrude";
    static const std::string s_LINE_COLOR_CONFIG       = s_CONFIG + "lineColor";
    static const std::string s_EDGE_COLOR_CONFIG       = s_CONFIG + "edgeColor";
    static const std::string s_DOUBLE_CLICK_VALIDATION = s_CONFIG + "validation_on_double_click";

    m_priority                = config.get<int>(s_PRIORITY_CONFIG, m_priority);
    m_extrude                 = config.get<bool>(s_EXTRUDE_CONFIG, m_extrude);
    m_validationByDoubleClick = config.get<bool>(s_DOUBLE_CLICK_VALIDATION, m_validationByDoubleClick);

    const auto divideBy255 = [](auto&& PH1, auto&& ...)
                             {
                                 return std::divides<>()(std::forward<decltype(PH1)>(PH1), 255.F);
                             };

    const auto hexaLineColor = config.get<std::string>(s_LINE_COLOR_CONFIG, "#FFFFFF");
    std::array<std::uint8_t, 4> lineColor {};
    data::tools::color::hexaStringToRGBA(hexaLineColor, lineColor);
    std::transform(lineColor.begin(), lineColor.end(), m_lineColor.ptr(), divideBy255);

    const auto hexaEdgeColor = config.get<std::string>(s_EDGE_COLOR_CONFIG, "#FFFFFF");
    std::array<std::uint8_t, 4> edgeColor {};
    data::tools::color::hexaStringToRGBA(hexaEdgeColor, edgeColor);
    std::transform(edgeColor.begin(), edgeColor.end(), m_edgeColor.ptr(), divideBy255);
}

//-----------------------------------------------------------------------------

void shape_extruder::starting()
{
    this->adaptor::initialize();

    this->getRenderService()->makeCurrent();

    // Add the interactor to the layer.
    const sight::viz::scene3d::Layer::sptr layer = this->getLayer();

    const sight::viz::scene3d::interactor::base::sptr interactor =
        std::dynamic_pointer_cast<sight::viz::scene3d::interactor::base>(this->get_sptr());
    layer->addInteractor(interactor, m_priority);

    // Create entities.
    Ogre::SceneManager* const sceneMng = this->getSceneManager();

    m_lassoNode = sceneMng->getRootSceneNode()->createChildSceneNode(this->get_id() + "_lassoNode");

    m_lasso         = sceneMng->createManualObject(this->get_id() + "_lasso");
    m_lastLassoLine = sceneMng->createManualObject(this->get_id() + "_lastLassoLine");

    m_lassoNode->attachObject(m_lasso);
    m_lassoNode->attachObject(m_lastLassoLine);

    // Create the material.
    m_material = std::make_shared<data::material>();

    m_materialAdaptor = this->registerService<module::viz::scene3d::adaptor::material>(
        "sight::module::viz::scene3d::adaptor::material"
    );
    m_materialAdaptor->set_inout(m_material, module::viz::scene3d::adaptor::material::s_MATERIAL_INOUT, true);
    m_materialAdaptor->configure(
        this->get_id() + m_materialAdaptor->get_id(),
        this->get_id() + m_materialAdaptor->get_id(),
        this->getRenderService(),
        m_layerID,
        "ambient"
    );
    m_materialAdaptor->start();
    m_materialAdaptor->getMaterialFw()->setHasVertexColor(true);
    m_materialAdaptor->update();
}

//-----------------------------------------------------------------------------

void shape_extruder::updating()
{
}

//-----------------------------------------------------------------------------

void shape_extruder::stopping()
{
    this->getRenderService()->makeCurrent();

    // Destroy the material.
    this->unregisterServices();
    m_material.reset();

    // Destroy entities.
    Ogre::SceneManager* const sceneMng = this->getSceneManager();

    sceneMng->destroyManualObject(m_lastLassoLine);
    sceneMng->destroyManualObject(m_lasso);

    sceneMng->destroySceneNode(m_lassoNode);

    // Remove the interactor from the layer.
    const sight::viz::scene3d::Layer::sptr layer = this->getLayer();

    const sight::viz::scene3d::interactor::base::sptr interactor =
        std::dynamic_pointer_cast<sight::viz::scene3d::interactor::base>(this->get_sptr());
    layer->removeInteractor(interactor);
}

//-----------------------------------------------------------------------------

void shape_extruder::enableTool(bool _enable)
{
    this->getRenderService()->makeCurrent();

    m_toolEnableState = _enable;

    // Stop the lasso interaction.
    m_interactionEnableState = false;

    // Clear entities.
    m_lassoToolPositions.clear();
    m_lassoNearPositions.clear();
    m_lassoFarPositions.clear();
    m_lassoEdgePositions.clear();
    m_lasso->clear();
    m_lastLassoLine->clear();

    // Send a render request.
    this->requestRender();
}

//-----------------------------------------------------------------------------

void shape_extruder::deleteLastMesh()
{
    // Get the reconstruction list.
    const auto extrudedMeshes = m_extrudedMeshes.lock();

    data::model_series::ReconstructionVectorType reconstructions = extrudedMeshes->getReconstructionDB();

    if(!reconstructions.empty())
    {
        reconstructions.pop_back();
        extrudedMeshes->setReconstructionDB(reconstructions);

        // Send notification
        this->notifier::info("Last extrusion deleted.");

        // Send the signal.
        auto sig = extrudedMeshes->signal<data::model_series::ReconstructionsRemovedSignalType>(
            data::model_series::RECONSTRUCTIONS_REMOVED_SIG
        );
        sig->async_emit(data::model_series::ReconstructionVectorType {reconstructions});
    }
    else
    {
        this->notifier::failure("No extrusion to delete.");
    }
}

//------------------------------------------------------------------------------

void shape_extruder::cancelLastClick()
{
    modifyLasso(Action::REMOVE);
}

//------------------------------------------------------------------------------

void shape_extruder::reset()
{
    // Get the reconstruction list.
    const auto extrudedMeshes = m_extrudedMeshes.lock();

    data::model_series::ReconstructionVectorType reconstructions = extrudedMeshes->getReconstructionDB();

    if(!reconstructions.empty())
    {
        reconstructions.clear();
        extrudedMeshes->setReconstructionDB(reconstructions);

        // Send the signal.
        auto sig = extrudedMeshes->signal<data::model_series::ModifiedSignalType>(data::model_series::MODIFIED_SIG);
        sig->async_emit();
    }
}

//-----------------------------------------------------------------------------

std::tuple<Ogre::Vector3, Ogre::Vector3, Ogre::Vector3> shape_extruder::getNearFarRayPositions(
    int _x,
    int _y
) const
{
    // Compute the ray.
    sight::viz::scene3d::Layer::sptr layer = this->getLayer();

    const Ogre::Camera* const camera = layer->getDefaultCamera();
    const Ogre::Viewport* const vp   = camera->getViewport();

    const float vpX = static_cast<float>(_x - vp->getActualLeft()) / static_cast<float>(vp->getActualWidth());
    const float vpY = static_cast<float>(_y - vp->getActualTop()) / static_cast<float>(vp->getActualHeight());

    Ogre::Ray ray = camera->getCameraToViewportRay(vpX, vpY);

    // Compute the intersection between the ray and the far working plane.
    std::pair<bool, Ogre::Real> farHit = Ogre::Math::intersects(ray, m_lassoFarPlane);
    SIGHT_ASSERT("The ray must hit the plane", farHit.first);
    const Ogre::Vector3 farPosition = ray.getPoint(farHit.second);

    // Launch the ray on the near plane after since the ray must be in front or behind it due to the frustum curve.
    std::pair<bool, Ogre::Real> nearHit = Ogre::Math::intersects(ray, m_lassoNearPlane);
    if(!nearHit.first)
    {
        ray.setDirection(-ray.getDirection());
        nearHit = Ogre::Math::intersects(ray, m_lassoNearPlane);
    }

    SIGHT_ASSERT("The ray must hit the plane", nearHit.first);
    const Ogre::Vector3 nearPosition = ray.getPoint(nearHit.second);

    // Launch the ray on the tool plane.
    std::pair<bool, Ogre::Real> toolHit = Ogre::Math::intersects(ray, m_lassoToolPlane);
    if(!toolHit.first)
    {
        ray.setDirection(-ray.getDirection());
        toolHit = Ogre::Math::intersects(ray, m_lassoToolPlane);
    }

    SIGHT_ASSERT("The ray must hit the plane", toolHit.first);
    const Ogre::Vector3 toolPosition = ray.getPoint(toolHit.second);

    return {toolPosition, nearPosition, farPosition};
}

//------------------------------------------------------------------------------

void shape_extruder::modifyLasso(Action _action, int _x, int _y)
{
    if(m_toolEnableState)
    {
        this->getRenderService()->makeCurrent();

        const sight::viz::scene3d::Layer::sptr layer = this->getLayer();

        // Start the lasso interaction.
        if(!m_interactionEnableState)
        {
            m_interactionEnableState = true;

            // Compute the plane where the tool will work.
            // This plane allows to generate all points of the lasso on the same plane to simplify further algorithms.
            const Ogre::Camera* const camera = layer->getDefaultCamera();
            const Ogre::Vector3 direction    = this->getCamDirection(camera);

            // Compute the near plane and the far plane.
            const Ogre::Vector3 camPos = camera->getDerivedPosition();
            m_lassoToolPlane = Ogre::Plane(direction, camPos + direction);
            m_lassoNearPlane = Ogre::Plane(direction, camPos + direction * camera->getNearClipDistance());
            m_lassoFarPlane  = Ogre::Plane(direction, camPos + direction * camera->getFarClipDistance());
        }

        m_leftButtonMoveState = false;

        // Cancel others interactions.
        layer->cancelFurtherInteraction();

        // Get the clicked point in the world space.
        const auto toolNearFarPos = this->getNearFarRayPositions(_x, _y);

        // Check the interactions.
        if(_action == Action::ADD)
        {
            // Check if the point can be added.
            bool near = false;
            for(const Ogre::Vector3 pos : m_lassoEdgePositions)
            {
                if((std::get<0>(toolNearFarPos) - pos).length() < m_lassoEdgeSize)
                {
                    near = true;
                    break;
                }
            }

            // Add the clicked point.
            if(!near)
            {
                m_lassoToolPositions.push_back(std::get<0>(toolNearFarPos));
                m_lassoNearPositions.push_back(std::get<1>(toolNearFarPos));
                m_lassoFarPositions.push_back(std::get<2>(toolNearFarPos));
                m_lassoEdgePositions.push_back(std::get<0>(toolNearFarPos));
            }
            else
            {
                return;
            }
        }
        else if(_action == Action::REMOVE)
        {
            // Remove the last clicked point.
            if(!m_lassoToolPositions.empty())
            {
                m_lassoEdgePositions.pop_back();
                do
                {
                    m_lassoToolPositions.pop_back();
                    m_lassoNearPositions.pop_back();
                    m_lassoFarPositions.pop_back();
                }
                while(!m_lassoToolPositions.empty() && m_lassoToolPositions.back() != m_lassoEdgePositions.back());
            }

            // Clear the last line if it's empty.
            if(m_lassoToolPositions.empty())
            {
                m_interactionEnableState = false;
                m_lastLassoLine->clear();
                m_lasso->clear();
                this->requestRender();
                return;
            }
        }

        // Draw the lasso.
        this->drawLasso();

        // Draw the last lasso line.
        m_lastLassoLine->clear();

        SIGHT_ASSERT("Lasso positions must have at east one point", !m_lassoToolPositions.empty());

        m_lastLassoLine->begin(
            m_materialAdaptor->getMaterialName(),
            Ogre::RenderOperation::OT_LINE_STRIP,
            sight::viz::scene3d::RESOURCE_GROUP
        );

        m_lastLassoLine->colour(m_lineColor);
        m_lastLassoLine->position(m_lassoToolPositions.back());
        if(_x != -1 && _y != -1)
        {
            m_lastLassoLine->position(std::get<0>(toolNearFarPos));
        }

        m_lastLassoLine->end();

        // Send a render request.
        this->requestRender();
    }
}

//------------------------------------------------------------------------------

void shape_extruder::validate()
{
    this->getRenderService()->makeCurrent();

    // When coming from touch, mouseReleaseEvent is not always called.
    if(m_leftButtonMoveState)
    {
        // Add a new point to the lasso edge list.
        m_lassoEdgePositions.push_back(m_lassoToolPositions.back());
        this->drawLasso();

        // Cancel the left button move state.
        m_leftButtonMoveState = false;
    }

    this->triangulatePoints();

    this->enableTool(false);
    m_toolDisabledSig->async_emit();

    // Send a render request.
    this->requestRender();
}

//-----------------------------------------------------------------------------

void shape_extruder::wheelEvent(Modifier /*_mods*/, double /*_angleDelta*/, int /*_x*/, int /*_y*/)
{
    if(m_interactionEnableState)
    {
        // Don't change the zoom when the tool interaction is enabled.
        const sight::viz::scene3d::Layer::sptr layer = this->getLayer();
        layer->cancelFurtherInteraction();
    }
}

//-----------------------------------------------------------------------------

void shape_extruder::buttonPressEvent(MouseButton _button, Modifier /*_mods*/, int _x, int _y)
{
    if(_button == MouseButton::LEFT)
    {
        modifyLasso(Action::ADD, _x, _y);
    }
    else if(_button == MouseButton::RIGHT)
    {
        modifyLasso(Action::REMOVE, _x, _y);
    }
}

//-----------------------------------------------------------------------------

void shape_extruder::buttonDoublePressEvent(MouseButton _button, Modifier /*_mods*/, int /*_x*/, int /*_y*/)
{
    if(m_validationByDoubleClick && m_interactionEnableState && _button == MouseButton::LEFT)
    {
        // Cancel others interactions.
        const sight::viz::scene3d::Layer::sptr layer = this->getLayer();
        layer->cancelFurtherInteraction();

        this->validate();
    }
}

//-----------------------------------------------------------------------------

void shape_extruder::mouseMoveEvent(MouseButton _button, Modifier /*_mods*/, int _x, int _y, int /*_dx*/, int /*_dy*/)
{
    if(!m_interactionEnableState)
    {
        return;
    }

    getRenderService()->makeCurrent();

    // Cancel others interactions.
    getLayer()->cancelFurtherInteraction();

    // Get the clicked point in the world space.
    const auto& [toolPosition, nearPosition, farPosition] = getNearFarRayPositions(_x, _y);

    // Check if the mouse is still on the last point.
    // This should not happen but it's better to check, since adding the same points twice will break everything.
    if(!m_lassoToolPositions.empty() && m_lassoToolPositions.back() == toolPosition
       && !m_lassoNearPositions.empty() && m_lassoNearPositions.back() == nearPosition
       && !m_lassoFarPositions.empty() && m_lassoFarPositions.back() == farPosition)
    {
        return;
    }

    if(_button == MouseButton::LEFT)
    {
        // Add a new position and draws the lasso.
        m_lassoToolPositions.push_back(toolPosition);
        m_lassoNearPositions.push_back(nearPosition);
        m_lassoFarPositions.push_back(farPosition);
        drawLasso();

        // Enable the left button move state.
        m_leftButtonMoveState = true;
    }

    // Draw the last lasso line.
    SIGHT_ASSERT("Lasso positions must have at east one point", !m_lassoToolPositions.empty());

    m_lastLassoLine->beginUpdate(0);

    m_lastLassoLine->colour(m_lineColor);
    m_lastLassoLine->position(m_lassoToolPositions.back());
    m_lastLassoLine->position(toolPosition);

    m_lastLassoLine->end();

    // Send a render request.
    requestRender();
}

//-----------------------------------------------------------------------------

void shape_extruder::buttonReleaseEvent(MouseButton /*_button*/, Modifier /*_mods*/, int /*_x*/, int /*_y*/)
{
    if(m_interactionEnableState && m_leftButtonMoveState)
    {
        this->getRenderService()->makeCurrent();

        // Cancel others interactions.
        const sight::viz::scene3d::Layer::sptr layer = this->getLayer();
        layer->cancelFurtherInteraction();

        // Add a new point to the lasso edge list.
        m_lassoEdgePositions.push_back(m_lassoToolPositions.back());
        this->drawLasso();

        // Cancel the left button move state.
        m_leftButtonMoveState = false;

        // Send a render request.
        this->requestRender();
    }
}

//-----------------------------------------------------------------------------

void shape_extruder::drawLasso()
{
    // Clear the previous lasso.
    m_lasso->clear();

    // Draw the lasso line.
    m_lasso->begin(
        m_materialAdaptor->getMaterialName(),
        Ogre::RenderOperation::OT_LINE_STRIP,
        sight::viz::scene3d::RESOURCE_GROUP
    );
    m_lasso->colour(m_lineColor);
    for(const Ogre::Vector3 pos : m_lassoToolPositions)
    {
        m_lasso->position(pos);
    }

    m_lasso->end();

    // Draw the spheres at the edge of each line.
    const unsigned int sample = 16;
    const auto deltaRing      = static_cast<float>(Ogre::Math::PI / sample);
    const float deltaSeg      = 2 * static_cast<float>(Ogre::Math::PI / sample);

    for(const Ogre::Vector3 pos : m_lassoEdgePositions)
    {
        // Begin a new section.
        m_lasso->begin(
            m_materialAdaptor->getMaterialName(),
            Ogre::RenderOperation::OT_TRIANGLE_LIST,
            sight::viz::scene3d::RESOURCE_GROUP
        );
        m_lasso->colour(m_edgeColor);

        Ogre::uint32 index = 0;
        for(unsigned ring = 0 ; ring <= sample ; ++ring)
        {
            const float r0 = m_lassoEdgeSize * std::sin(static_cast<float>(ring) * deltaRing);
            const float y0 = m_lassoEdgeSize * std::cos(static_cast<float>(ring) * deltaRing);

            for(unsigned seg = 0 ; seg <= sample ; ++seg)
            {
                const float x0 = r0 * std::sin(static_cast<float>(seg) * deltaSeg);
                const float z0 = r0 * std::cos(static_cast<float>(seg) * deltaSeg);
                Ogre::Vector3 point(x0, y0, z0);

                m_lasso->position(pos + point);

                if(ring != sample)
                {
                    m_lasso->index(index + sample + 1);
                    m_lasso->index(index);
                    m_lasso->index(index + sample);
                    m_lasso->index(index + sample + 1);
                    m_lasso->index(index + 1);
                    m_lasso->index(index);
                    ++index;
                }
            }
        }

        m_lasso->end();
    }
}

//-----------------------------------------------------------------------------

void shape_extruder::triangulatePoints() const
{
    SIGHT_ASSERT(
        "Lasso near and far positions must have the same size",
        m_lassoNearPositions.size() == m_lassoFarPositions.size()
    );

    // Generate triangles from the point list with the constrained Browyer-Watson algorithm.
    std::vector<Triangle3D> triangulation;
    if(m_lassoNearPositions.size() > 3)
    {
        this->generateDelaunayTriangulation(m_lassoNearPositions, triangulation);
        if(m_extrude)
        {
            this->generateDelaunayTriangulation(m_lassoFarPositions, triangulation);
        }
    }
    else if(m_lassoNearPositions.size() == 3)
    {
        triangulation.emplace_back(m_lassoNearPositions[0], m_lassoNearPositions[1], m_lassoNearPositions[2]);
        if(m_extrude)
        {
            triangulation.emplace_back(m_lassoFarPositions[0], m_lassoFarPositions[1], m_lassoFarPositions[2]);
        }
    }

    // Create triangles that links the two points lists.
    if(m_extrude)
    {
        const std::size_t size = m_lassoNearPositions.size();
        for(std::size_t index = 0 ; index < size ; ++index)
        {
            const bool isLast    = index == size - 1;
            const std::size_t i0 = index;
            const std::size_t i1 = isLast ? 0 : index + 1;

            const Triangle3D triA(m_lassoNearPositions[i0], m_lassoFarPositions[i0], m_lassoFarPositions[i1]);
            const Triangle3D triB(m_lassoNearPositions[i0], m_lassoFarPositions[i1], m_lassoNearPositions[i1]);

            triangulation.push_back(triA);
            triangulation.push_back(triB);
        }
    }

    this->generateExtrudedMesh(triangulation);
}

//-----------------------------------------------------------------------------

void shape_extruder::generateExtrudedMesh(const std::vector<Triangle3D>& _triangulation) const
{
    // Creates the mesh from a list a 3D triangles.
    const data::mesh::sptr mesh = std::make_shared<data::mesh>();
    {
        const auto lock = mesh->dump_lock();

        // 3 points per triangle and one cell per triangle.
        mesh->resize(
            static_cast<data::mesh::size_t>(_triangulation.size() * 3),
            static_cast<data::mesh::size_t>(_triangulation.size()),
            data::mesh::CellType::TRIANGLE,
            data::mesh::Attributes::POINT_NORMALS
        );

        // Fill points.
        {
            auto it = mesh->begin<data::iterator::point::xyz>();

            for(const Triangle3D& triangle : _triangulation)
            {
                it->x = triangle.a.x;
                it->y = triangle.a.y;
                it->z = triangle.a.z;
                ++it;
                it->x = triangle.b.x;
                it->y = triangle.b.y;
                it->z = triangle.b.z;
                ++it;
                it->x = triangle.c.x;
                it->y = triangle.c.y;
                it->z = triangle.c.z;
                ++it;
            }
        }

        // Fill cell coordinates.
        {
            auto it = mesh->begin<data::iterator::cell::triangle>();
            for(data::mesh::size_t index = 0 ; index < _triangulation.size() * 3 ; index += 3)
            {
                it->pt[0] = index;
                it->pt[1] = index + 1;
                it->pt[2] = index + 2;

                ++it;
            }
        }

        // Generate normals.
        geometry::data::mesh::generatePointNormals(mesh);
    }

    // Get the reconstruction list.
    const auto extrudedMeshes = m_extrudedMeshes.lock();

    data::model_series::ReconstructionVectorType reconstructions = extrudedMeshes->getReconstructionDB();

    // Creates the reconstruction.
    const data::reconstruction::sptr reconstruction = std::make_shared<data::reconstruction>();
    reconstruction->setMesh(mesh);
    reconstruction->setOrganName("ExtrudedMesh_" + std::to_string(reconstructions.size()));

    // Pushs the reconstruction.
    reconstructions.push_back(reconstruction);
    extrudedMeshes->setReconstructionDB(reconstructions);

    // Send the signal.
    auto sig = extrudedMeshes->signal<data::model_series::ReconstructionsAddedSignalType>(
        data::model_series::RECONSTRUCTIONS_ADDED_SIG
    );
    sig->async_emit(data::model_series::ReconstructionVectorType {reconstruction});
}

//------------------------------------------------------------------------------

void shape_extruder::generateDelaunayTriangulation(
    const std::vector<Ogre::Vector3>& _points,
    std::vector<Triangle3D>& _wordTriangulation
) const
{
    // Retrieve the camera.
    const sight::viz::scene3d::Layer::sptr layer = this->getLayer();
    const Ogre::Camera* const camera             = layer->getDefaultCamera();
    const Ogre::Matrix4 viewMatrix               = camera->getViewMatrix();

    // Transform all point from world space to view space to get them in a 2D plane with a constant Z value.
    std::vector<Ogre::Vector2> points;
    for(const Ogre::Vector3 point : _points)
    {
        const Ogre::Vector2 viewPoint = (viewMatrix * Ogre::Vector4(point, 1.F)).xy();
        points.push_back(viewPoint);
    }

    // Get the depth of the 2D plane.
    const float depth = (viewMatrix * Ogre::Vector4(_points[0], 1.F)).z;

    // Compute the bounding box of points.
    const float min = std::numeric_limits<float>::lowest();
    const float max = std::numeric_limits<float>::max();
    Ogre::Vector2 minBound(max, max);
    Ogre::Vector2 maxBound(min, min);

    for(const Ogre::Vector2 point : points)
    {
        minBound.x = std::min(point.x, minBound.x);
        maxBound.x = std::max(point.x, maxBound.x);
        minBound.y = std::min(point.y, minBound.y);
        maxBound.y = std::max(point.y, maxBound.y);
    }

    // Increase the bounding box to avoid points to be shared with some of the triangle edges.
    minBound -= 1;
    maxBound += 1;

    // Compute a triangle large enough to contains all points.
    const Ogre::Vector2 bottomLeft = minBound;
    const Ogre::Vector2 bottomRight(minBound.x + (maxBound.x - minBound.x) * 2.F, minBound.y);
    const Ogre::Vector2 topLeft(minBound.x, minBound.y + (maxBound.y - minBound.y) * 2.F);
    const Triangle2D superTriangle(bottomLeft, bottomRight, topLeft);

    // Store triangles.
    std::vector<Triangle2D> triangulation {superTriangle};

    // Triangulate points with the Bowyer-Watson algorithm.
    for(const Ogre::Vector2 sommet : points)
    {
        sight::module::viz::scene3d::adaptor::shape_extruder::addDelaunayPoint(triangulation, sommet);
    }

    // Some input segment are missing from the triangulation, we insert them.
    // Add missing segments while new constraints are added to the previous iteration.
    std::vector<Ogre::Vector2> oldPoints = points;
    std::vector<Ogre::Vector2> newPoints = points;
    const int maxIteration               = 3;
    int count                            = 0;
    do
    {
        oldPoints = newPoints;
        newPoints.clear();
        for(std::int64_t index = 0 ; index < static_cast<std::int64_t>(oldPoints.size()) ; ++index)
        {
            const std::size_t previousIndex = index - 1
                                              < 0 ? oldPoints.size() - 1 : static_cast<std::size_t>(index - 1);
            const Edge edge(oldPoints[previousIndex], oldPoints[static_cast<std::size_t>(index)]);
            std::list<Ogre::Vector2> constraints = this->addConstraints(triangulation, edge);

            newPoints.push_back(oldPoints[previousIndex]);
            newPoints.insert(newPoints.end(), constraints.begin(), constraints.end());
        }
    }
    while(count++ < maxIteration && oldPoints.size() != newPoints.size());

    // If triangles contains a vertex from original super-triangle, remove them.
    auto endTriangulation = triangulation.end();
    for(auto it = triangulation.begin() ; it != endTriangulation ; )
    {
        const bool edgeA = it->a == superTriangle.a || it->a == superTriangle.b || it->a == superTriangle.c;
        const bool edgeB = it->b == superTriangle.a || it->b == superTriangle.b || it->b == superTriangle.c;
        const bool edgeC = it->c == superTriangle.a || it->c == superTriangle.b || it->c == superTriangle.c;

        if(edgeA || edgeB || edgeC)
        {
            it               = triangulation.erase(it);
            endTriangulation = triangulation.end();
        }
        else
        {
            ++it;
        }
    }

    // Remove each triangle that aren't in the shape.
    for(auto it = triangulation.begin() ; it != endTriangulation ; )
    {
        const Edge ray(it->barycentre, superTriangle.a);

        bool inside = false;
        for(std::int64_t i = 0 ; i < static_cast<std::int64_t>(points.size()) ; ++i)
        {
            const std::size_t previousI = i - 1 < 0 ? points.size() - 1 : static_cast<std::size_t>(i - 1);
            const Edge edge(points[previousI], points[static_cast<std::size_t>(i)]);

            if(ray.intersect(edge))
            {
                inside = !inside;
            }
        }

        if(!inside)
        {
            it               = triangulation.erase(it);
            endTriangulation = triangulation.end();
        }
        else
        {
            ++it;
        }
    }

    // Gets back triangle coordinates to the world space.
    for(const Triangle2D& triangle : triangulation)
    {
        Ogre::Vector3 a = (viewMatrix.inverse() * Ogre::Vector4(triangle.a.x, triangle.a.y, depth, 1.F)).xyz();
        Ogre::Vector3 b = (viewMatrix.inverse() * Ogre::Vector4(triangle.b.x, triangle.b.y, depth, 1.F)).xyz();
        Ogre::Vector3 c = (viewMatrix.inverse() * Ogre::Vector4(triangle.c.x, triangle.c.y, depth, 1.F)).xyz();

        _wordTriangulation.emplace_back(a, b, c);
    }
}

//-----------------------------------------------------------------------------

void shape_extruder::addDelaunayPoint(std::vector<Triangle2D>& _triangulation, const Ogre::Vector2& _sommet)
{
    // first find all the triangles that are no longer valid due to the insertion.
    std::list<Triangle2D> badTriangles;
    for(const Triangle2D& triangle : _triangulation)
    {
        // If the current sommet is in the circumscribed circle of the current triangle.
        if(triangle.center.distance(_sommet) <= triangle.radius)
        {
            badTriangles.push_back(triangle);
        }
    }

    // find the boundary of the polygonal hole.
    std::list<Edge> polygon;
    for(const Triangle2D& triangle : badTriangles)
    {
        const Edge edgeA(triangle.a, triangle.b);
        const Edge edgeB(triangle.a, triangle.c);
        const Edge edgeC(triangle.b, triangle.c);

        bool allowA = true;
        bool allowB = true;
        bool allowC = true;

        // If the edge is not shared by any other triangles in badTriangles.
        for(const Triangle2D& shared : badTriangles)
        {
            if(shared.id != triangle.id)
            {
                Edge sharedA(shared.a, shared.b);
                Edge sharedB(shared.a, shared.c);
                Edge sharedC(shared.b, shared.c);
                if(edgeA == sharedA || edgeA == sharedB || edgeA == sharedC)
                {
                    allowA = false;
                }

                if(edgeB == sharedA || edgeB == sharedB || edgeB == sharedC)
                {
                    allowB = false;
                }

                if(edgeC == sharedA || edgeC == sharedB || edgeC == sharedC)
                {
                    allowC = false;
                }
            }
        }

        // Add edges of the triangle to the candidate polygon.
        if(allowA)
        {
            polygon.push_back(edgeA);
        }

        if(allowB)
        {
            polygon.push_back(edgeB);
        }

        if(allowC)
        {
            polygon.push_back(edgeC);
        }

        _triangulation.erase(
            std::find_if(
                _triangulation.begin(),
                _triangulation.end(),
                [&](const Triangle2D& _tri) -> bool
            {
                return _tri.id == triangle.id;
            })
        );
    }

    // Create news triangles by using the current sommet and the polygon.
    for(const Edge& edge : polygon)
    {
        // Add a new triangle to the triangulation.
        const Triangle2D tri(edge.a, edge.b, _sommet);
        _triangulation.push_back(tri);
    }
}

//------------------------------------------------------------------------------

std::list<Ogre::Vector2> shape_extruder::addConstraints(
    std::vector<Triangle2D>& _triangulation,
    const Edge& _edge,
    int _depth
) const
{
    if(_depth >= 5)
    {
        return {};
    }

    bool found = false;
    for(const Triangle2D& triangle : _triangulation)
    {
        const Edge edgeA(triangle.a, triangle.b);
        const Edge edgeB(triangle.a, triangle.c);
        const Edge edgeC(triangle.b, triangle.c);

        if(_edge == edgeA || _edge == edgeB || _edge == edgeC)
        {
            found = true;
            break;
        }
    }

    std::list<Ogre::Vector2> addedPoints;
    if(!found)
    {
        const Ogre::Vector2 midPoint = (_edge.a + _edge.b) / 2.F;
        this->addDelaunayPoint(_triangulation, midPoint);

        const int depth                     = _depth + 1;
        std::list<Ogre::Vector2> frontAdded = this->addConstraints(_triangulation, Edge(_edge.a, midPoint), depth);
        std::list<Ogre::Vector2> backAdded  = this->addConstraints(_triangulation, Edge(midPoint, _edge.b), depth);

        addedPoints = frontAdded;
        addedPoints.push_back(midPoint);
        addedPoints.insert(addedPoints.end(), backAdded.begin(), backAdded.end());
    }

    return addedPoints;
}

} // namespace sight::module::viz::scene3d::adaptor.

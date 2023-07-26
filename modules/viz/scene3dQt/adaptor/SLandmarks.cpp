/************************************************************************
 *
 * Copyright (C) 2018-2023 IRCAD France
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

#include "SLandmarks.hpp"

#include "core/thread/Worker.hpp"

#include "viz/scene3d/IMaterialAdaptor.hpp"

#include <core/com/Slots.hxx>
#include <core/runtime/path.hpp>

#include <ui/base/Cursor.hpp>

#include <viz/scene3d/helper/ManualObject.hpp>
#include <viz/scene3d/helper/Scene.hpp>

#include <modules/viz/scene3dQt/WindowInteractor.hpp>

#include <QHBoxLayout>

namespace sight::module::viz::scene3dQt::adaptor
{

static constexpr Ogre::Real s_SELECTED_SCALE = 1.35F;
static constexpr Ogre::Real s_DEFAULT_SCALE  = 1.F;

//------------------------------------------------------------------------------

static std::array<float, 3> getCurrentSlicePos(const data::Image& image)
{
    namespace Helper = sight::data::helper::MedicalImage;

    const auto axialIndex = std::max(
        std::int64_t(0),
        Helper::getSliceIndex(image, Helper::orientation_t::AXIAL).value_or(0)
    );

    const auto frontalIndex = std::max(
        std::int64_t(0),
        Helper::getSliceIndex(image, Helper::orientation_t::FRONTAL).value_or(0)
    );

    const auto sagittalIndex = std::max(
        std::int64_t(0),
        Helper::getSliceIndex(image, Helper::orientation_t::SAGITTAL).value_or(0)
    );

    const auto& imgSpacing = image.getSpacing();
    const auto& imgOrigin  = image.getOrigin();

    return {
        static_cast<float>(sagittalIndex) * static_cast<float>(imgSpacing[0])
        + static_cast<float>(imgOrigin[0]),
        static_cast<float>(frontalIndex) * static_cast<float>(imgSpacing[1])
        + static_cast<float>(imgOrigin[1]),
        static_cast<float>(axialIndex) * static_cast<float>(imgSpacing[2])
        + static_cast<float>(imgOrigin[2])
    };
}

//------------------------------------------------------------------------------

Ogre::Vector3 SLandmarks::getCamDirection(const Ogre::Camera* const _cam)
{
    const Ogre::Matrix4 view = _cam->getViewMatrix();
    Ogre::Vector3 direction(view[2][0], view[2][1], view[2][2]);
    direction.normalise();
    return -direction;
}

//-----------------------------------------------------------------------------

SLandmarks::SLandmarks() noexcept
{
    newSlot(Slots::REMOVE_ALL, &SLandmarks::removeAll, this);
    newSlot(Slots::REMOVE_GROUP, &SLandmarks::removeGroup, this);
    newSlot(Slots::MODIFY_GROUP, &SLandmarks::modifyGroup, this);
    newSlot(Slots::MODIFY_POINT, &SLandmarks::modifyPoint, this);
    newSlot(Slots::ADD_POINT, &SLandmarks::addPoint, this);
    newSlot(Slots::REMOVE_POINT, &SLandmarks::removePoint, this);
    newSlot(Slots::INSERT_POINT, &SLandmarks::insertPoint, this);
    newSlot(Slots::SELECT_POINT, &SLandmarks::selectPoint, this);
    newSlot(Slots::DESELECT_POINT, &SLandmarks::deselectPoint, this);
    newSlot(Slots::SLICE_TYPE, &SLandmarks::changeSliceType, this);
    newSlot(Slots::SLICE_INDEX, &SLandmarks::changeSliceIndex, this);
    newSlot(Slots::RENAME_GROUP, &SLandmarks::renameGroup, this);
    newSlot(Slots::SET_CURRENT_GROUP, &SLandmarks::setCurrentGroup, this);
    newSlot(Slots::REMOVE_LANDMARKS, &SLandmarks::removeLandmarks, this);
    newSlot(Slots::CREATE_LANDMARK, &SLandmarks::createLandmark, this);
    newSlot(Slots::CONFIGURE_LANDMARKS, &SLandmarks::configureLandmarks, this);
    newSlot(Slots::ENABLE_EDIT_MODE, &SLandmarks::enableEditMode, this);
    newSlot(Slots::DISABLE_EDIT_MODE, &SLandmarks::disableEditMode, this);
    newSlot(Slots::TOGGLE_EDIT_MODE, [this]{m_editMode ? disableEditMode() : enableEditMode();});
    newSlot(Slots::CHANGE_EDIT_MODE, [this](bool editMode){editMode ? enableEditMode() : disableEditMode();});
}

//-----------------------------------------------------------------------------

void SLandmarks::configuring()
{
    configureParams();

    const auto& config = getConfiguration();

    setTransformId(
        config.get<std::string>(
            sight::viz::scene3d::ITransformable::s_TRANSFORM_CONFIG,
            getID() + "_transform"
        )
    );

    static const std::string s_FONT_SIZE_CONFIG       = s_CONFIG + "fontSize";
    static const std::string s_LABEL_CONFIG           = s_CONFIG + "label";
    static const std::string s_ORIENTATION_CONFIG     = s_CONFIG + "orientation";
    static const std::string s_LANDMARKS_FLAGS_CONFIG = s_CONFIG + "landmarksQueryFlags";
    static const std::string s_INTERACTIVE_CONFIG     = s_CONFIG + "interactive";
    static const std::string s_PRIORITY_CONFIG        = s_CONFIG + "priority";
    static const std::string s_QUERY_MASK_CONFIG      = s_CONFIG + "queryMask";
    static const std::string s_VIEW_DISTANCE          = s_CONFIG + "viewDistance";
    static const std::string s_INITIAL_GROUP          = s_CONFIG + "initialGroup";
    static const std::string s_INITIAL_COLOR          = s_CONFIG + "initialColor";
    static const std::string s_INITIAL_SIZE           = s_CONFIG + "initialSize";
    static const std::string s_INITIAL_SHAPE          = s_CONFIG + "initialShape";
    static const std::string s_ALLOW_RENAME           = s_CONFIG + "allowRename";

    m_fontSize     = config.get<std::size_t>(s_FONT_SIZE_CONFIG, m_fontSize);
    m_enableLabels = config.get<bool>(s_LABEL_CONFIG, m_enableLabels);
    m_interactive  = config.get<bool>(s_INTERACTIVE_CONFIG, m_interactive);
    m_priority     = config.get<int>(s_PRIORITY_CONFIG, m_priority);

    const std::string orientation = config.get<std::string>(s_ORIENTATION_CONFIG, "axial");
    if(orientation == "axial")
    {
        m_orientation = OrientationMode::Z_AXIS;
    }
    else if(orientation == "frontal")
    {
        m_orientation = OrientationMode::Y_AXIS;
    }
    else if(orientation == "sagittal")
    {
        m_orientation = OrientationMode::X_AXIS;
    }
    else
    {
        SIGHT_ERROR("Unknown orientation, allow values are `axial`, `frontal` and `sagittal`");
    }

    std::string hexaMask = config.get<std::string>(s_QUERY_MASK_CONFIG, "");
    if(!hexaMask.empty())
    {
        SIGHT_ASSERT(
            "Hexadecimal values (" + s_QUERY_MASK_CONFIG + ") should start with '0x'. Given value : " + hexaMask,
            hexaMask.length() > 2 && hexaMask.substr(0, 2) == "0x"
        );
        m_queryMask = static_cast<std::uint32_t>(std::stoul(hexaMask, nullptr, 16));
    }

    hexaMask = config.get<std::string>(s_LANDMARKS_FLAGS_CONFIG, "");
    if(!hexaMask.empty())
    {
        SIGHT_ASSERT(
            "Hexadecimal values (" + s_LANDMARKS_FLAGS_CONFIG + ") should start with '0x'. Given value : " + hexaMask,
            hexaMask.length() > 2 && hexaMask.substr(0, 2) == "0x"
        );
        m_landmarksQueryFlag = static_cast<std::uint32_t>(std::stoul(hexaMask, nullptr, 16));
    }

    // View distance
    const std::string& view_distance = config.get<std::string>(s_VIEW_DISTANCE, "slicesInRange");
    if(view_distance == "slicesInRange")
    {
        m_viewDistance = ViewDistance::SLICES_IN_RANGE;
    }
    else if(view_distance == "currentSlice")
    {
        m_viewDistance = ViewDistance::CURRENT_SLICE;
    }
    else if(view_distance == "allSlices")
    {
        m_viewDistance = ViewDistance::ALL_SLICES;
    }
    else
    {
        SIGHT_ERROR("Unknown view distance, allowed values are `slicesInRange`, `currentSlice` and `allSlices`");
    }

    // Initial group
    m_currentGroup = config.get<std::string>(s_INITIAL_GROUP, m_currentGroup);

    // Initial color
    auto color = sight::data::Color::New();
    color->setRGBA(config.get<std::string>(s_INITIAL_COLOR, "#FFFF00FF"));
    m_currentColor = {color->red(), color->green(), color->blue(), color->alpha()};

    // Initial size
    m_currentSize = config.get<float>(s_INITIAL_SIZE, m_currentSize);

    // Initial shape
    const std::string& initialShape = config.get<std::string>(s_INITIAL_SHAPE, "sphere");
    if(initialShape == "sphere")
    {
        m_currentShape = sight::data::Landmarks::Shape::SPHERE;
    }
    else if(initialShape == "cube")
    {
        m_currentShape = sight::data::Landmarks::Shape::CUBE;
    }
    else
    {
        SIGHT_ERROR("Unknown shape, allowed values are `sphere` and `cube`");
    }

    m_renamingAllowed = config.get<bool>(s_ALLOW_RENAME, m_enableLabels);
    SIGHT_ASSERT(
        "Renaming labels is allowed yet the labels are disabled, this is forbidden.",
        m_enableLabels || !m_renamingAllowed
    );
}

//-----------------------------------------------------------------------------

void SLandmarks::starting()
{
    initialize();

    getRenderService()->makeCurrent();

    auto* rootSceneNode = getSceneManager()->getRootSceneNode();
    m_transNode = getOrCreateTransformNode(rootSceneNode);

    m_material = data::Material::New();
    m_material->setDiffuse(data::Color::New(1.F, 1.F, 1.F, 1.F));

    // Register the material adaptor.
    m_materialAdaptor = this->registerService<sight::viz::scene3d::IMaterialAdaptor>(
        "sight::module::viz::scene3d::adaptor::SMaterial"
    );
    m_materialAdaptor->setInOut(m_material, sight::viz::scene3d::IMaterialAdaptor::s_MATERIAL_INOUT, true);
    m_materialAdaptor->configure(
        this->getID() + m_materialAdaptor->getID(),
        this->getID() + m_materialAdaptor->getID(),
        this->getRenderService(),
        m_layerID
    );
    m_materialAdaptor->start();

    m_materialAdaptor->getMaterialFw()->setHasVertexColor(true);
    m_materialAdaptor->update();

    if(m_interactive)
    {
        auto interactor = std::dynamic_pointer_cast<sight::viz::scene3d::interactor::IInteractor>(getSptr());
        getLayer()->addInteractor(interactor, m_priority);
    }

    // Draw landmarks.
    updating();

    auto interactor    = getLayer()->getRenderService()->getInteractorManager();
    auto qtInteractor  = WindowInteractor::dynamicCast(interactor);
    auto* parentWidget = qtInteractor->getQtWidget();
    m_contextualMenu = new QWidget(parentWidget);
    m_contextualMenu->setStyleSheet(".QWidget {background-color: none;}");
    m_contextualMenu->hide();
    auto* layout = new QHBoxLayout;
    QIcon trashBinIcon(QString::fromStdString(
                           core::runtime::getModuleResourceFilePath(
                               "sight::module::ui::flaticons/RedTrashBin.svg"
                           ).string()
    ));
    auto* binButton             = new QPushButton(trashBinIcon, "");
    const std::string serviceID = getID().substr(getID().find_last_of('_') + 1);
    binButton->setObjectName(QString::fromStdString(serviceID) + "/binButton");
    binButton->setCursor(Qt::ArrowCursor);
    binButton->adjustSize();
    if(binButton->width() < 40)
    {
        binButton->setMinimumWidth(40);
    }

    if(binButton->height() < 40)
    {
        binButton->setMinimumHeight(40);
    }

    binButton->adjustSize();
    binButton->setIconSize(binButton->size());
    layout->addWidget(binButton);
    if(m_renamingAllowed)
    {
        QIcon penIcon(QString::fromStdString(
                          core::runtime::getModuleResourceFilePath(
                              "sight::module::ui::flaticons/YellowPen.svg"
                          ).string()
        ));
        auto* penButton = new QPushButton(penIcon, "");
        penButton->setObjectName(QString::fromStdString(serviceID) + "/penButton");
        penButton->setCursor(Qt::ArrowCursor);
        penButton->adjustSize();
        if(penButton->width() < 40)
        {
            penButton->setMinimumWidth(40);
        }

        if(penButton->height() < 40)
        {
            penButton->setMinimumHeight(40);
        }

        penButton->adjustSize();
        penButton->setIconSize(penButton->size());
        layout->addWidget(penButton);
    }

    m_contextualMenu->setLayout(layout);
    m_contextualMenu->adjustSize();
    m_contextualMenu->raise();
}

//-----------------------------------------------------------------------------

service::IService::KeyConnectionsMap SLandmarks::getAutoConnections() const
{
    service::IService::KeyConnectionsMap connections;

    connections.push(s_TRANSFORM_CONFIG, data::Matrix4::s_MODIFIED_SIG, IService::slots::s_UPDATE);

    connections.push(s_LANDMARKS_INOUT, data::Landmarks::s_MODIFIED_SIG, IService::slots::s_UPDATE);
    connections.push(s_LANDMARKS_INOUT, data::Landmarks::s_GROUP_REMOVED_SIG, Slots::REMOVE_GROUP);
    connections.push(s_LANDMARKS_INOUT, data::Landmarks::s_GROUP_MODIFIED_SIG, Slots::MODIFY_GROUP);
    connections.push(s_LANDMARKS_INOUT, data::Landmarks::s_GROUP_RENAMED_SIG, Slots::RENAME_GROUP);
    connections.push(s_LANDMARKS_INOUT, data::Landmarks::s_POINT_MODIFIED_SIG, Slots::MODIFY_POINT);
    connections.push(s_LANDMARKS_INOUT, data::Landmarks::s_POINT_ADDED_SIG, Slots::ADD_POINT);
    connections.push(s_LANDMARKS_INOUT, data::Landmarks::s_POINT_REMOVED_SIG, Slots::REMOVE_POINT);
    connections.push(s_LANDMARKS_INOUT, data::Landmarks::s_POINT_INSERTED_SIG, Slots::INSERT_POINT);
    connections.push(s_LANDMARKS_INOUT, data::Landmarks::s_POINT_SELECTED_SIG, Slots::SELECT_POINT);
    connections.push(s_LANDMARKS_INOUT, data::Landmarks::s_POINT_DESELECTED_SIG, Slots::DESELECT_POINT);
    connections.push(s_IMAGE_INPUT, data::Image::s_SLICE_TYPE_MODIFIED_SIG, Slots::SLICE_TYPE);
    connections.push(s_IMAGE_INPUT, data::Image::s_SLICE_INDEX_MODIFIED_SIG, Slots::SLICE_INDEX);

    return connections;
}

//-----------------------------------------------------------------------------

void SLandmarks::updating()
{
    // Delete all groups.
    removeAllManualObjects();

    const auto& landmarks = m_landmarks.const_lock();

    // Create all point.
    for(const auto& groupName : landmarks->getGroupNames())
    {
        const auto& group = landmarks->getGroup(groupName);
        for(std::size_t index = 0 ; index < group.m_points.size() ; ++index)
        {
            this->insertMyPoint(groupName, index, landmarks.get_shared());
        }
    }
}

//-----------------------------------------------------------------------------

void SLandmarks::stopping()
{
    if(m_interactive)
    {
        auto interactor = std::dynamic_pointer_cast<sight::viz::scene3d::interactor::IInteractor>(getSptr());
        getLayer()->removeInteractor(interactor);
    }

    // Stop all threads.
    for(const auto& selectedLandmark : m_selectedLandmarks)
    {
        selectedLandmark->m_timer->stop();
    }

    removeAllManualObjects();

    // Unregister the material adaptor.
    unregisterServices();

    m_eventFilter = nullptr;
}

//------------------------------------------------------------------------------

void SLandmarks::removeAllManualObjects()
{
    // Make the context as current.
    getRenderService()->makeCurrent();

    auto* sceneMgr = getSceneManager();

    // Find object where name match _groupName and delete Ogre's resources.
    for(auto it = m_manualObjects.begin() ; it != m_manualObjects.end() ; )
    {
        const auto& landmark = *it;
        const auto& name     = landmark->m_groupName;

        deselectPoint(name, landmark->m_index);

        if(m_enableLabels)
        {
            landmark->m_label->detachFromNode();
        }

        m_transNode->removeAndDestroyChild(landmark->m_node);
        sceneMgr->destroyManualObject(landmark->m_object);

        it = m_manualObjects.erase(it);
    }

    // Request the rendering.
    requestRender();
}

//------------------------------------------------------------------------------

void SLandmarks::removeAll()
{
    m_contextualMenu->hide();

    auto landmarks = m_landmarks.lock();
    for(const std::string& grp : landmarks->getGroupNames())
    {
        landmarks->removeGroup(grp);
        landmarks->signal<data::Landmarks::GroupRemovedSignalType>(data::Landmarks::s_GROUP_REMOVED_SIG)->asyncEmit(grp);
    }
}

//------------------------------------------------------------------------------

void SLandmarks::removeGroup(std::string _groupName)
{
    // Make the context as current.
    getRenderService()->makeCurrent();

    auto* sceneMgr = getSceneManager();

    // Find object where name match _groupName and delete Ogre's resources.
    for(auto objectIt = m_manualObjects.begin() ; objectIt != m_manualObjects.end() ; )
    {
        const std::string& name = (*objectIt)->m_groupName;
        if(name.find(_groupName) != std::string::npos)
        {
            // Stop the thread if it already run since we are deleting data.
            deselectPoint(_groupName, (*objectIt)->m_index);

            if(m_enableLabels)
            {
                (*objectIt)->m_label->detachFromNode();
                (*objectIt)->m_label->setVisible(false);
            }

            m_transNode->removeAndDestroyChild((*objectIt)->m_node);
            sceneMgr->destroyManualObject((*objectIt)->m_object);

            objectIt = m_manualObjects.erase(objectIt);
        }
        else
        {
            ++objectIt;
        }
    }

    // Request the rendering.
    requestRender();
}

//------------------------------------------------------------------------------

void SLandmarks::modifyGroup(std::string _groupName)
{
    // Make the context as current.
    getRenderService()->makeCurrent();

    // Get all selected point.
    std::vector<std::size_t> indexes;
    for(const std::shared_ptr<SelectedLandmark>& landmark : m_selectedLandmarks)
    {
        indexes.push_back(landmark->m_landmark->m_index);
    }

    // Remove the group.
    removeGroup(_groupName);

    // Get landmarks.
    const auto& landmarks = m_landmarks.const_lock();

    // Retrieve group.
    const auto& group = landmarks->getGroup(_groupName);

    std::size_t groupSize = group.m_points.size();

    // Re-create the group.
    for(std::size_t index = 0 ; index < groupSize ; ++index)
    {
        this->insertMyPoint(_groupName, index, landmarks.get_shared());
    }

    // Re-run selected landmark threads
    for(std::size_t index : indexes)
    {
        this->selectPoint(_groupName, index);
    }
}

//------------------------------------------------------------------------------

void SLandmarks::renameGroup(std::string _oldGroupName, std::string _newGroupName)
{
    for(const std::shared_ptr<Landmark>& landmark : m_manualObjects)
    {
        if(landmark->m_groupName == _oldGroupName)
        {
            landmark->m_groupName = _newGroupName;
            if(m_enableLabels)
            {
                landmark->m_label->setText(_newGroupName + '_' + std::to_string(landmark->m_index));
            }
        }
    }

    if(m_currentGroup == _oldGroupName)
    {
        m_currentGroup = _newGroupName;
    }
}

//------------------------------------------------------------------------------

void SLandmarks::setCurrentGroup(std::string _newCurrentGroupName)
{
    m_currentGroup = _newCurrentGroupName;
}

//------------------------------------------------------------------------------

void SLandmarks::modifyPoint(std::string _groupName, std::size_t _index)
{
    const auto& landmarks = m_landmarks.const_lock();
    const auto& point     = landmarks->getPoint(_groupName, _index);

    for(auto& m_manualObject : m_manualObjects)
    {
        const auto& name = m_manualObject->m_groupName;
        if(name.find(_groupName) != std::string::npos && m_manualObject->m_index == _index)
        {
            const Ogre::Vector3 position(static_cast<float>(point[0]),
                                         static_cast<float>(point[1]),
                                         static_cast<float>(point[2]));
            m_manualObject->m_node->setPosition(position);
            updateLandmarkVisibility(*m_manualObject, *landmarks);
            break;
        }
    }

    getRenderService()->requestRender();
}

//------------------------------------------------------------------------------

void SLandmarks::addPoint(std::string _groupName)
{
    // Make the context as current.
    getRenderService()->makeCurrent();

    // Get landmarks.
    const auto landmarks = m_landmarks.const_lock();

    // Retrieve group.
    const auto& group = landmarks->getGroup(_groupName);
    SIGHT_ASSERT(
        "They must have at least one point in the group `" << _groupName << "`",
        !group.m_points.empty()
    );

    // Get the last index.
    std::size_t index = group.m_points.size() - 1;

    // Add the new point.
    this->insertMyPoint(_groupName, index, landmarks.get_shared());
}

//------------------------------------------------------------------------------

void SLandmarks::removePoint(std::string _groupName, std::size_t _index)
{
    // Make the context as current.
    this->getRenderService()->makeCurrent();

    Ogre::SceneManager* sceneMgr = this->getSceneManager();

    // Find object where name match _groupName and the index, and delete Ogre's resources.
    for(auto objectIt = m_manualObjects.begin() ; objectIt != m_manualObjects.end() ; ++objectIt)
    {
        const std::string& name = (*objectIt)->m_groupName;
        if(name.find(_groupName) != std::string::npos && (*objectIt)->m_index == _index)
        {
            // Stop the thread if it already run since we are deleting data.
            this->deselectPoint(_groupName, _index);

            if(m_enableLabels)
            {
                (*objectIt)->m_label->detachFromNode();
                (*objectIt)->m_label->setVisible(false);
            }

            m_transNode->removeAndDestroyChild((*objectIt)->m_node);
            sceneMgr->destroyManualObject((*objectIt)->m_object);
            objectIt = m_manualObjects.erase(objectIt);
            break;
        }
    }

    // Re-compute index of landmarks in the same group.
    for(auto& m_manualObject : m_manualObjects)
    {
        const std::string& name = m_manualObject->m_groupName;
        if(name.find(_groupName) != std::string::npos && m_manualObject->m_index > _index)
        {
            m_manualObject->m_index -= 1;
            if(m_enableLabels)
            {
                m_manualObject->m_label->setText(_groupName + "_" + std::to_string(m_manualObject->m_index));
            }
        }
    }

    // Request the rendering.
    this->requestRender();
}

//------------------------------------------------------------------------------

void SLandmarks::insertPoint(std::string _groupName, std::size_t _index)
{
    // Make the context as current
    getRenderService()->makeCurrent();

    const auto landmarks = m_landmarks.const_lock();
    insertMyPoint(_groupName, _index, landmarks.get_shared());
}

//------------------------------------------------------------------------------

std::shared_ptr<SLandmarks::Landmark> SLandmarks::insertMyPoint(
    std::string _groupName,
    std::size_t _index,
    const data::Landmarks::csptr& _landmarks
)
{
    // Retrieve group.
    const data::Landmarks::LandmarksGroup& group = _landmarks->getGroup(_groupName);

    // Create the point name.
    const std::string pointName = _groupName + "_" + std::to_string(_index);

    // Create the manual object.
    const Ogre::ColourValue color =
        Ogre::ColourValue(group.m_color[0], group.m_color[1], group.m_color[2], group.m_color[3]);

    Ogre::SceneManager* sceneMgr = this->getSceneManager();
    Ogre::ManualObject* object   = sceneMgr->createManualObject(this->getID() + "_" + pointName + "_object");
    switch(group.m_shape)
    {
        case data::Landmarks::Shape::SPHERE:
            sight::viz::scene3d::helper::ManualObject::createSphere(
                object,
                m_materialAdaptor->getMaterialName(),
                color,
                group.m_size * 0.5F
            );
            break;

        case data::Landmarks::Shape::CUBE:
            sight::viz::scene3d::helper::ManualObject::createCube(
                object,
                m_materialAdaptor->getMaterialName(),
                color,
                group.m_size
            );
            break;
    }

    object->setQueryFlags(m_landmarksQueryFlag);

    Ogre::SceneNode* node = m_transNode->createChildSceneNode(this->getID() + "_" + pointName + "_node");

    // Set the point to the right position.
    const data::Landmarks::PointType& point = _landmarks->getPoint(_groupName, _index);
    node->setPosition(Ogre::Real(point[0]), Ogre::Real(point[1]), Ogre::Real(point[2]));

    // Attach data.
    node->attachObject(object);

    // Create the label.
    sight::viz::scene3d::IText::sptr text;
    if(m_enableLabels)
    {
        // Create the label.
        text = sight::viz::scene3d::IText::New(this->getLayer());
        text->setFontSize(m_fontSize);
        text->setText(pointName);
        text->setTextColor(color);
        text->setVisible(group.m_visibility && m_isVisible);
        text->setTextAlignment("center", "");

        // Attach data.
        text->attachToNode(node, this->getLayer()->getDefaultCamera());
    }

    // Store the created data.
    auto newLandmark = std::make_shared<Landmark>(node, object, _groupName, _index, text);
    m_manualObjects.push_back(newLandmark);

    if(m_enableLabels)
    {
        // The landmark should be in a weak ptr, else there is a circular reference (the slot refers to the landmark via
        // newLandmark, and the landmark refers to the slot via m_slots).
        auto slotTextEdited = core::com::newSlot(
            [this, weakLandmark = std::weak_ptr(newLandmark)](std::string editedText)
            {
                auto landmark             = weakLandmark.lock();
                std::string newGroupName  = editedText;
                std::string indexAsString = std::to_string(landmark->m_index);
                // Remove the index suffix if present
                if(newGroupName.ends_with('_' + indexAsString))
                {
                    newGroupName = newGroupName.substr(0, newGroupName.size() - (indexAsString.size() + 1));
                }

                std::string oldGroupName = landmark->m_groupName;
                m_landmarks.lock()->renameGroup(oldGroupName, newGroupName);
                renameGroup(oldGroupName, newGroupName);
                auto signal =
                    m_landmarks.lock()->signal<data::Landmarks::GroupRenamedSignalType>(
                        data::Landmarks::s_GROUP_RENAMED_SIG
                    );
                signal->asyncEmit(oldGroupName, newGroupName);
            });
        slotTextEdited->setWorker(core::thread::getDefaultWorker());
        newLandmark->m_label->signal(sight::viz::scene3d::IText::s_TEXT_EDITED_SIGNAL)->connect(slotTextEdited);
        newLandmark->m_slots.push_back(slotTextEdited);

        auto slotEditingFinish = core::com::newSlot(
            [label = std::weak_ptr(newLandmark->m_label)]
            {
                label.lock()->setEditMode(false);
            });
        slotEditingFinish->setWorker(core::thread::getDefaultWorker());
        newLandmark->m_label->signal(sight::viz::scene3d::IText::s_EDITING_FINISHED_SIGNAL)->connect(slotEditingFinish);
        newLandmark->m_slots.push_back(slotEditingFinish);
    }

    updateLandmarkVisibility(*m_manualObjects.back(), *_landmarks);

    // Request the rendering.
    this->requestRender();

    return newLandmark;
}

//------------------------------------------------------------------------------

void SLandmarks::selectPoint(std::string _groupName, std::size_t _index)
{
    // Make the context as current.
    this->getRenderService()->makeCurrent();

    for(auto& m_manualObject : m_manualObjects)
    {
        const std::string& name = m_manualObject->m_groupName;
        if(name.find(_groupName) != std::string::npos && m_manualObject->m_index == _index)
        {
            const auto it = std::find_if(
                m_selectedLandmarks.begin(),
                m_selectedLandmarks.end(),
                [&](std::shared_ptr<SelectedLandmark> _landmark)
                {
                    return _landmark->m_landmark->m_groupName == _groupName && _landmark->m_landmark->m_index == _index;
                });

            if(it == m_selectedLandmarks.end())
            {
                // This method must be synchronized with deselectPoint(std::string, std::size_t).
                std::lock_guard guard(m_selectedMutex);

                // Create thread data.
                std::shared_ptr<SelectedLandmark> selectedLandmark =
                    std::make_shared<SelectedLandmark>(this->worker()->createTimer(), m_manualObject);
                m_selectedLandmarks.push_back(selectedLandmark);

                // Run a thread that change the selected point.
                core::thread::Timer::TimeDurationType duration = std::chrono::milliseconds(500);
                selectedLandmark->m_timer->setFunction(
                    [this, selectedLandmark](auto&& ...)
                    {
                        hightlight(selectedLandmark);
                    });
                selectedLandmark->m_timer->setDuration(duration);
                selectedLandmark->m_timer->start();
            }

            break;
        }
    }
}

//------------------------------------------------------------------------------

void SLandmarks::deselectPoint(std::string _groupName, std::size_t _index)
{
    // Make the context as current.
    getRenderService()->makeCurrent();

    // This method must be synchronized with selectPoint(std::string, std::size_t).
    std::lock_guard guard(m_selectedMutex);

    // Find the thread and stop it.
    for(auto landmarkIt = m_selectedLandmarks.begin() ; landmarkIt != m_selectedLandmarks.end() ; ++landmarkIt)
    {
        if((*landmarkIt)->m_landmark->m_groupName == _groupName && (*landmarkIt)->m_landmark->m_index == _index)
        {
            // Stop the timer.
            (*landmarkIt)->m_timer->stop();
            (*landmarkIt)->m_landmark->m_object->setVisible(true);
            updateLandmarkVisibility((*landmarkIt)->m_landmark);

            // Request the rendering.
            requestRender();

            m_selectedLandmarks.erase(landmarkIt);
            break;
        }
    }
}

//------------------------------------------------------------------------------

void SLandmarks::hightlight(std::shared_ptr<SelectedLandmark> _selectedLandmark)
{
    // Make the context as current.
    this->getRenderService()->makeCurrent();

    // Hightlight the selected landmark.
    this->updateLandmarkVisibility(_selectedLandmark->m_landmark);
    if(_selectedLandmark->m_landmark->m_object->isVisible())
    {
        if(_selectedLandmark->m_show)
        {
            _selectedLandmark->m_landmark->m_object->setVisible(true);
        }
        else
        {
            _selectedLandmark->m_landmark->m_object->setVisible(false);
        }

        this->requestRender();
    }

    _selectedLandmark->m_show = !_selectedLandmark->m_show;

    // Request the rendering.
    this->requestRender();
}

//------------------------------------------------------------------------------

void SLandmarks::changeSliceType(int _from, int _to)
{
    // Make the context as current.
    this->getRenderService()->makeCurrent();

    const auto toOrientation   = static_cast<OrientationMode>(_to);
    const auto fromOrientation = static_cast<OrientationMode>(_from);

    const auto newOrientation = m_orientation == toOrientation ? fromOrientation
                                                               : m_orientation
                                == fromOrientation ? toOrientation : m_orientation;

    m_orientation = newOrientation;

    this->updateLandmarksVisibility();

    // Request the rendering.
    this->requestRender();
}

//------------------------------------------------------------------------------

void SLandmarks::changeSliceIndex(int /*_axialIndex*/, int /*_frontalIndex*/, int /*_sagittalIndex*/)
{
    // Make the context as current.
    this->getRenderService()->makeCurrent();

    const auto imageLock = m_image.lock();

    if(imageLock)
    {
        this->getRenderService()->makeCurrent();

        this->updateLandmarksVisibility();

        // Request the rendering.
        this->requestRender();
    }
}

//------------------------------------------------------------------------------

void SLandmarks::removeLandmarks()
{
    auto landmarks = m_landmarks.lock();

    if(!landmarks)
    {
        // No need to continue if there is no landmarks
        return;
    }

    const auto& image = m_image.const_lock();
    SIGHT_THROW_IF("Data is null.", !image);

    const auto slice_index = sight::data::helper::MedicalImage::getSliceIndex(
        *image,
        m_orientation
    );

    if(!slice_index)
    {
        // No slice selected, early return
        return;
    }

    bool hasDeleted = false;

    for(const auto& name : landmarks->getGroupNames())
    {
        auto& group = landmarks->getGroup(name);

        for(auto it = group.m_points.begin() ; it < group.m_points.end() ; )
        {
            if(isLandmarkVisible(*it, group.m_size))
            {
                it         = group.m_points.erase(it);
                hasDeleted = true;
            }
            else
            {
                ++it;
            }
        }
    }

    if(hasDeleted)
    {
        const auto& sig = landmarks->signal<sight::data::Landmarks::ModifiedSignalType>(
            sight::data::Landmarks::s_MODIFIED_SIG
        );
        sig->asyncEmit();
    }
}

//------------------------------------------------------------------------------

void SLandmarks::configureLandmarks(
    std::optional<std::string> group,
    std::optional<sight::data::Landmarks::ColorType> color,
    std::optional<sight::data::Landmarks::SizeType> size,
    std::optional<sight::data::Landmarks::Shape> shape,
    std::optional<int> groupMax,
    std::optional<int> visibleMax,
    std::optional<int> totalMax
)
{
    if(group)
    {
        m_currentGroup = *group;
    }

    if(color)
    {
        m_currentColor = *color;
    }

    if(size)
    {
        m_currentSize = *size;
    }

    if(shape)
    {
        m_currentShape = *shape;
    }

    if(groupMax)
    {
        if(*groupMax >= 0)
        {
            m_groupMax[m_currentGroup] = std::size_t(*groupMax);
        }
        else
        {
            m_groupMax.erase(m_currentGroup);
        }
    }

    if(visibleMax)
    {
        if(*visibleMax >= 0)
        {
            m_visibleMax = *visibleMax;
        }
        else
        {
            m_visibleMax = std::nullopt;
        }
    }

    if(totalMax)
    {
        if(*totalMax >= 0)
        {
            m_totalMax = *totalMax;
        }
        else
        {
            m_totalMax = std::nullopt;
        }
    }
}

//------------------------------------------------------------------------------

void SLandmarks::enableEditMode()
{
    if(m_editMode)
    {
        return;
    }

    m_editMode = true;
    setCursor(Qt::CrossCursor);

    if(m_eventFilter == nullptr)
    {
        auto interactor    = getLayer()->getRenderService()->getInteractorManager();
        auto qtInteractor  = WindowInteractor::dynamicCast(interactor);
        auto* parentWidget = qtInteractor->getQtWidget();
        m_eventFilter = std::make_unique<DeleteContextualMenuWhenFocusOut>(this);
        parentWidget->installEventFilter(m_eventFilter.get());
    }

    m_editModeChanged->asyncEmit(true);
}

//------------------------------------------------------------------------------

void SLandmarks::disableEditMode()
{
    if(!m_editMode)
    {
        return;
    }

    m_editMode = false;
    auto interactor    = getLayer()->getRenderService()->getInteractorManager();
    auto qtInteractor  = WindowInteractor::dynamicCast(interactor);
    auto* parentWidget = qtInteractor->getQtWidget();
    parentWidget->unsetCursor();

    m_contextualMenu->hide();

    if(m_enableLabels && m_renamingAllowed)
    {
        std::ranges::for_each(m_manualObjects, [](std::shared_ptr<Landmark> l){l->m_label->setEditMode(false);});
    }

    m_editModeChanged->asyncEmit(false);
}

//------------------------------------------------------------------------------

void SLandmarks::createLandmark(sight::data::Landmarks::PointType point)
{
    createAndPickLandmark(point, false);
}

//------------------------------------------------------------------------------

void SLandmarks::createAndPickLandmark(const sight::data::Landmarks::PointType& point, bool pick)
{
    if(isMaxLandmarksReached())
    {
        // No need to continue if the maximum number of landmarks is reached
        SIGHT_DEBUG("Maximum number of landmarks reached.");
        return;
    }

    auto landmarks = m_landmarks.lock();

    // If the group does not exist, we create it.
    if(!landmarks->hasGroup(m_currentGroup))
    {
        landmarks->addGroup(m_currentGroup, m_currentColor, m_currentSize, m_currentShape);
        const auto& sig = landmarks->signal<sight::data::Landmarks::GroupAddedSignalType>(
            sight::data::Landmarks::s_GROUP_ADDED_SIG
        );

        sig->asyncEmit(m_currentGroup);
    }

    landmarks->addPoint(m_currentGroup, point);

    // Get the last index.
    const auto& group = landmarks->getGroup(m_currentGroup);

    // Add the new point.
    const auto& new_landmark = insertMyPoint(m_currentGroup, group.m_points.size() - 1, landmarks.get_shared());

    if(pick)
    {
        m_pickedData = new_landmark;
        m_pickedData->m_node->setScale(s_SELECTED_SCALE, s_SELECTED_SCALE, s_SELECTED_SCALE);
    }
    else
    {
        m_pickedData = nullptr;
    }

    // Block the signal to avoid being called back.
    const auto& sig = landmarks->signal<sight::data::Landmarks::PointAddedSignalType>(
        sight::data::Landmarks::s_POINT_ADDED_SIG
    );

    sight::core::com::Connection::Blocker blocker(sig->getConnection(slot(Slots::ADD_POINT)));
    sig->asyncEmit(m_currentGroup);
}

//------------------------------------------------------------------------------

bool SLandmarks::isMaxLandmarksReached()
{
    if(!m_totalMax && !m_visibleMax && m_groupMax.empty())
    {
        // Early return if we must not count anything
        return false;
    }

    // Count landmarks.
    const auto& landmarks = m_landmarks.const_lock();

    if(m_groupMax.contains(m_currentGroup)
       && (m_groupMax[m_currentGroup] == 0
           || (landmarks->hasGroup(m_currentGroup)
               && landmarks->getGroup(m_currentGroup).m_points.size() >= m_groupMax[m_currentGroup])))
    {
        return true;
    }

    if(m_totalMax || m_visibleMax)
    {
        const auto& names = landmarks->getGroupNames();

        std::size_t max        = 0;
        std::size_t maxVisible = 0;

        for(const auto& name : names)
        {
            const auto& group = landmarks->getGroup(name);

            if(m_totalMax)
            {
                max += group.m_points.size();

                if(max >= *m_totalMax)
                {
                    return true;
                }
            }

            if(m_visibleMax)
            {
                for(const auto& point : group.m_points)
                {
                    if(isLandmarkVisible(point, group.m_size))
                    {
                        ++maxVisible;

                        if(maxVisible >= *m_visibleMax)
                        {
                            return true;
                        }
                    }
                }
            }
        }
    }

    return false;
}

//------------------------------------------------------------------------------

void SLandmarks::updateLandmarksVisibility()
{
    // Make the context as current.
    getRenderService()->makeCurrent();

    const auto imageLock = m_image.lock();

    // Hide landmarks only if there is an image.
    if(imageLock)
    {
        for(const auto& landmark : m_manualObjects)
        {
            updateLandmarkVisibility(landmark);
        }
    }
}

//------------------------------------------------------------------------------

void SLandmarks::updateLandmarkVisibility(std::shared_ptr<Landmark> _landmark)
{
    // Make the context as current.
    getRenderService()->makeCurrent();

    const auto& landmarks = m_landmarks.const_lock();
    updateLandmarkVisibility(*_landmark, *landmarks);
}

//------------------------------------------------------------------------------

void SLandmarks::updateLandmarkVisibility(Landmark& landmark, const data::Landmarks& landmarks)
{
    const bool must_show =
        [&]
        {
            // Retrieve group
            if(!landmarks.hasGroup(landmark.m_groupName))
            {
                return false;
            }

            const auto& group = landmarks.getGroup(landmark.m_groupName);

            if(!group.m_visibility)
            {
                return false;
            }

            if(const auto& image = m_image.const_lock(); image)
            {
                const auto& position = landmark.m_node->getPosition();
                return isLandmarkVisible({position[0], position[1], position[2]}, group.m_size);
            }

            return true;
        }();

    // Show or hide the landmark.
    landmark.m_object->setVisible(must_show);
}

//------------------------------------------------------------------------------

bool SLandmarks::isLandmarkVisible(
    const data::Landmarks::PointType& point,
    data::Landmarks::SizeType group_size
) const
{
    if(const auto& image = m_image.const_lock(); image)
    {
        const auto position       = point[m_orientation];
        const auto slice_position = getCurrentSlicePos(*image)[m_orientation];
        const auto spacing        = image->getSpacing()[m_orientation];

        switch(m_viewDistance)
        {
            case ViewDistance::SLICES_IN_RANGE:
            {
                // Check if the position is the same than slice position
                const auto group_half_size = group_size * 0.5;
                const auto max_size        = std::max(group_half_size, spacing);

                return core::tools::is_greater(position, (slice_position - group_size))
                       && core::tools::is_less(position, (slice_position + max_size));
            }

            case ViewDistance::CURRENT_SLICE:
            {
                // Check if the position is the same than slice position
                const auto roundedPosition      = std::round(position / spacing);
                const auto roundedSlicePosition = std::round(slice_position / spacing);
                return core::tools::is_equal(roundedPosition, roundedSlicePosition);
            }

            default:
                break;
        }
    }

    return true;
}

//------------------------------------------------------------------------------

void SLandmarks::setVisible(bool _visible)
{
    const auto landmarks = m_landmarks.const_lock();
    for(const auto& landmark : m_manualObjects)
    {
        const auto& group = landmarks->getGroup(landmark->m_groupName);
        landmark->m_object->setVisible(_visible && group.m_visibility);
        if(m_enableLabels)
        {
            landmark->m_label->setVisible(_visible && group.m_visibility);
        }
    }

    requestRender();
}

//------------------------------------------------------------------------------

std::optional<Ogre::Vector3> SLandmarks::getNearestPickedPosition(int _x, int _y)
{
    Ogre::SceneManager* sm = this->getSceneManager();
    const auto result      = sight::viz::scene3d::Utils::pickObject(_x, _y, m_queryMask, *sm);

    if(result.has_value())
    {
        const auto* const camera = sm->getCamera(sight::viz::scene3d::Layer::s_DEFAULT_CAMERA_NAME);
        const auto* const vp     = camera->getViewport();

        // Screen to viewport space conversion.
        const float vpX = static_cast<float>(_x - vp->getActualLeft()) / static_cast<float>(vp->getActualWidth());
        const float vpY = static_cast<float>(_y - vp->getActualTop()) / static_cast<float>(vp->getActualHeight());

        const Ogre::Ray ray = camera->getCameraToViewportRay(vpX, vpY);

        Ogre::Vector3 normal = -ray.getDirection();
        normal.normalise();

        return result->second + normal * 0.01F;
    }

    return std::nullopt;
}

//------------------------------------------------------------------------------

void SLandmarks::buttonPressEvent(MouseButton _button, Modifier /*_mods*/, int _x, int _y)
{
    m_contextualMenu->hide();

    if(_button != LEFT)
    {
        // We only manage left click.
        return;
    }

    if(!m_editMode)
    {
        // If the edit mode is off, the landmarks are read-only.
        return;
    }

    std::shared_ptr<Landmark> pickedData = tryPick(_x, _y);
    if(pickedData != nullptr)
    {
        setCursor(Qt::ClosedHandCursor);
        m_pickedData = pickedData;
        m_pickedData->m_node->setScale(s_SELECTED_SCALE, s_SELECTED_SCALE, s_SELECTED_SCALE);
        m_mustShowContextualMenu = true;
    }
    else
    {
        // If nothing is picked, we will create a new landmark.
        if(auto newPos = this->getNearestPickedPosition(_x, _y); newPos)
        {
            setCursor(Qt::ClosedHandCursor);
            createAndPickLandmark({(*newPos)[0], (*newPos)[1], (*newPos)[2]});
            m_mustShowContextualMenu = false;
        }
    }
}

//------------------------------------------------------------------------------

void SLandmarks::mouseMoveEvent(MouseButton /*_button*/, Modifier /*_mods*/, int _x, int _y, int /*_dx*/, int /*_dy*/)
{
    if(m_pickedData != nullptr)
    {
        m_mustShowContextualMenu = false;

        setCursor(Qt::ClosedHandCursor);

        // Discard the current landmark to launch the ray over the scene without picking this one.
        m_pickedData->m_object->setQueryFlags(0x0);

        const auto layer              = this->getLayer();
        const Ogre::Camera* const cam = layer->getDefaultCamera();
        SIGHT_ASSERT("No camera found", cam);

        bool moveInCameraPlane = true;

        Ogre::Vector3 newPos;
        if(cam->getProjectionType() == Ogre::ProjectionType::PT_PERSPECTIVE)
        {
            // If something is picked, we will snap the landmark to it
            std::optional<Ogre::Vector3> pickedPos = this->getNearestPickedPosition(_x, _y);
            if(pickedPos.has_value())
            {
                newPos            = pickedPos.value();
                moveInCameraPlane = false;
            }
        }

        // Else we move the distance along a plane.
        if(moveInCameraPlane)
        {
            const auto* const vp = cam->getViewport();

            const float vpX = static_cast<float>(_x - vp->getActualLeft()) / static_cast<float>(vp->getActualWidth());
            const float vpY = static_cast<float>(_y - vp->getActualTop()) / static_cast<float>(vp->getActualHeight());

            const Ogre::Ray ray           = cam->getCameraToViewportRay(vpX, vpY);
            const Ogre::Vector3 direction = this->getCamDirection(cam);

            const Ogre::Vector3 position = m_pickedData->m_node->getPosition();

            const Ogre::Plane plane(direction, position);

            const std::pair<bool, Ogre::Real> hit = Ogre::Math::intersects(ray, plane);

            if(!hit.first)
            {
                SIGHT_ERROR("The ray must intersect the plane")
                return;
            }

            newPos = ray.getPoint(hit.second);
        }

        // Reset the query flag.
        m_pickedData->m_object->setQueryFlags(m_landmarksQueryFlag);

        // Update the data, the autoconnection will call modifyPoint.
        auto landmarks                    = m_landmarks.lock();
        data::Landmarks::PointType& point = landmarks->getPoint(m_pickedData->m_groupName, m_pickedData->m_index);
        point[0] = newPos[0];
        point[1] = newPos[1];
        point[2] = newPos[2];

        const auto& sig = landmarks->signal<data::Landmarks::PointModifiedSigType>(
            data::Landmarks::s_POINT_MODIFIED_SIG
        );
        sig->asyncEmit(m_pickedData->m_groupName, m_pickedData->m_index);

        this->requestRender();
    }
    else if(m_editMode)
    {
        if(tryPick(_x, _y) != nullptr)
        {
            setCursor(Qt::OpenHandCursor);
        }
        else
        {
            setCursor(Qt::CrossCursor);
        }
    }
}

//------------------------------------------------------------------------------

void SLandmarks::buttonReleaseEvent(MouseButton _button, Modifier /*_mods*/, int /*_x*/, int /*_y*/)
{
    if(m_pickedData != nullptr)
    {
        m_pickedData->m_node->setScale(s_DEFAULT_SCALE, s_DEFAULT_SCALE, s_DEFAULT_SCALE);

        setCursor(Qt::OpenHandCursor);

        // If we are in edit mode, we show the bin button to remove it.
        if(m_editMode && m_mustShowContextualMenu)
        {
            std::pair<Ogre::Vector2, Ogre::Vector2> screenPos = sight::viz::scene3d::helper::Scene::computeBoundingRect(
                *getLayer()->getDefaultCamera(),
                m_pickedData->m_node
            );
            const auto ratio   = m_contextualMenu->devicePixelRatioF();
            auto interactor    = getLayer()->getRenderService()->getInteractorManager();
            auto qtInteractor  = WindowInteractor::dynamicCast(interactor);
            auto* parentWidget = qtInteractor->getQtWidget();
            const int x        = std::clamp(
                int(((screenPos.first.x + screenPos.second.x) / 2) / ratio),
                0,
                parentWidget->width() - m_contextualMenu->width()
            );

            int y = int((screenPos.first.y / ratio) - m_contextualMenu->height());
            if(y < 0)
            {
                // If there isn't enough place upward the landmark, place the menu downward.
                y = int(screenPos.second.y / ratio);
            }

            m_contextualMenu->move(x, y);
            m_contextualMenu->raise();
            const QString serviceID = QString::fromStdString(getID().substr(getID().find_last_of('_') + 1));
            auto* binButton         = m_contextualMenu->findChild<QPushButton*>(serviceID + "/binButton");
            SIGHT_ASSERT("The contextual menu should have the bin button", binButton);
            binButton->disconnect();
            QObject::connect(
                binButton,
                &QPushButton::clicked,
                [this, groupName = m_pickedData->m_groupName, index = m_pickedData->m_index]
                {
                    m_contextualMenu->hide();
                    m_landmarks.lock()->removePoint(groupName, index);
                    removePoint(groupName, index);
                    const auto& sig = m_landmarks.lock()->signal<sight::data::Landmarks::PointRemovedSignalType>(
                        sight::data::Landmarks::s_POINT_REMOVED_SIG
                    );

                    // Block the signal to avoid being called back.
                    sight::core::com::Connection::Blocker blocker(
                        sig->getConnection(slot(Slots::REMOVE_POINT))
                    );

                    sig->asyncEmit(groupName, index);
                });
            if(m_renamingAllowed)
            {
                auto* penButton = m_contextualMenu->findChild<QPushButton*>(serviceID + "/penButton");
                SIGHT_ASSERT("The contextual menu should have the pen button", penButton);
                penButton->disconnect();
                QObject::connect(
                    penButton,
                    &QPushButton::clicked,
                    [this, label = std::weak_ptr(m_pickedData->m_label)]
                    {
                        m_contextualMenu->hide();
                        for(const std::shared_ptr<Landmark>& landmark : m_manualObjects)
                        {
                            landmark->m_label->setEditMode(false);
                        }

                        label.lock()->setEditMode(true);
                    });
            }

            m_contextualMenu->show();
        }

        m_pickedData = nullptr;

        this->getLayer()->requestRender();
    }
    else if(_button == RIGHT)
    {
        disableEditMode();
    }
}

//------------------------------------------------------------------------------

void SLandmarks::buttonDoublePressEvent(MouseButton /*_button*/, Modifier /*_mods*/, int _x, int _y)
{
    m_contextualMenu->hide();

    std::shared_ptr<Landmark> pickedData = tryPick(_x, _y);

    if(pickedData != nullptr)
    {
        getLayer()->cancelFurtherInteraction();

        // Check if something is picked to update the position of the distance.
        const auto pickedPos = getNearestPickedPosition(_x, _y);
        if(pickedPos.has_value())
        {
            // Update the data, the autoconnection will call modifyPoint.
            const auto& landmarks = m_landmarks.const_lock();
            const auto& point     = landmarks->getPoint(
                pickedData->m_groupName,
                pickedData->m_index
            );

            // Send signal with world coordinates of the landmarks
            m_send_world_coord->asyncEmit(
                point[0],
                point[1],
                point[2]
            );
        }
    }
}

//------------------------------------------------------------------------------

void SLandmarks::wheelEvent(Modifier /*_mods*/, double /*_angleDelta*/, int /*_x*/, int /*_y*/)
{
    m_contextualMenu->hide();
}

//------------------------------------------------------------------------------

void SLandmarks::keyPressEvent(int _key, Modifier /*_mods*/, int /*_mouseX*/, int /*_mouseY*/)
{
    if(m_editMode && _key == Qt::Key_Escape)
    {
        disableEditMode();
    }
}

//------------------------------------------------------------------------------

std::shared_ptr<SLandmarks::Landmark> SLandmarks::tryPick(int _x, int _y) const
{
    const auto layer = this->getLayer();

    Ogre::SceneManager* const sceneMgr = layer->getSceneManager();

    const Ogre::Camera* const cam = layer->getDefaultCamera();
    const auto* const vp          = cam->getViewport();

    const float vpX = static_cast<float>(_x - vp->getActualLeft()) / static_cast<float>(vp->getActualWidth());
    const float vpY = static_cast<float>(_y - vp->getActualTop()) / static_cast<float>(vp->getActualHeight());

    const Ogre::Ray ray = cam->getCameraToViewportRay(vpX, vpY);

    const auto raySceneQuery = std::unique_ptr<Ogre::RaySceneQuery>(
        sceneMgr->createRayQuery(ray, m_landmarksQueryFlag)
    );

    raySceneQuery->setSortByDistance(true);
    const auto& queryResult = raySceneQuery->execute();

    if(queryResult.empty())
    {
        // Nothing found.
        return nullptr;
    }

    // Find the ogre landmark and apply a scale.
    bool found = false;

    // The plane is always intercepted, if a landmark is found
    for(std::size_t index = 0 ; !found && index < queryResult.size() ; ++index)
    {
        // The landmark should be the nearest object found.
        const auto* const object = queryResult[index].movable;

        auto landmarks = m_landmarks.lock();

        for(const auto& landmark : m_manualObjects)
        {
            if(landmark->m_object == object)
            {
                try
                {
                    if(const auto& group = landmarks->getGroup(landmark->m_groupName);
                       group.m_visibility && isLandmarkVisible(group.m_points[landmark->m_index], group.m_size))
                    {
                        return landmark;
                    }
                }
                catch(...)
                {
                    SIGHT_ERROR("Landmark group '" + landmark->m_groupName + "' not found.");

                    // No point found, we can't select it.
                    continue;
                }
            }
        }
    }

    return nullptr;
}

//------------------------------------------------------------------------------

void SLandmarks::setCursor(QCursor cursor)
{
    auto interactor    = getLayer()->getRenderService()->getInteractorManager();
    auto qtInteractor  = WindowInteractor::dynamicCast(interactor);
    auto* parentWidget = qtInteractor->getQtWidget();
    parentWidget->setCursor(cursor);
}

SLandmarks::DeleteContextualMenuWhenFocusOut::DeleteContextualMenuWhenFocusOut(SLandmarks* sLandmarks) :
    m_sLandmarks(sLandmarks)
{
}

//------------------------------------------------------------------------------

bool SLandmarks::DeleteContextualMenuWhenFocusOut::eventFilter(QObject* /*o*/, QEvent* e)
{
    if(m_sLandmarks->m_contextualMenu != nullptr
       && !m_sLandmarks->m_contextualMenu->findChild<QPushButton*>()->hasFocus()
       && (e->type() == QEvent::FocusOut || e->type() == QEvent::Resize))
    {
        m_sLandmarks->m_contextualMenu->hide();
    }

    return false;
}

} // namespace sight::module::viz::scene3dQt::adaptor.

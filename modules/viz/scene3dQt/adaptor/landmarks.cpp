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

#include "data/landmarks.hpp"

#include "landmarks.hpp"

#include "core/thread/worker.hpp"

#include "data/dicom/Attribute.hpp"
#include "data/fiducials_series.hpp"
#include "data/image_series.hpp"

#include "viz/scene3d/IMaterialAdaptor.hpp"

#include <core/com/slots.hxx>
#include <core/runtime/path.hpp>
#include <core/tools/uuid.hpp>

#include <ui/__/cursor.hpp>

#include <viz/scene3d/helper/ManualObject.hpp>
#include <viz/scene3d/helper/Scene.hpp>

#include <modules/viz/scene3dQt/window_interactor.hpp>

#include <QHBoxLayout>

namespace sight::module::viz::scene3dQt::adaptor
{

static constexpr Ogre::Real s_SELECTED_SCALE = 1.35F;
static constexpr Ogre::Real s_DEFAULT_SCALE  = 1.F;

namespace
{

//------------------------------------------------------------------------------

[[nodiscard]] std::optional<data::fiducials_series::FiducialSet> getFiducialSet(
    const std::string& _groupName,
    data::fiducials_series::csptr fiducialsSeries
)
{
    SIGHT_ASSERT("Fiducials series mustn't be null", fiducialsSeries != nullptr);
    std::optional<std::pair<data::fiducials_series::FiducialSet, std::size_t> > res =
        fiducialsSeries->getFiducialSetAndIndex(_groupName);
    return res.has_value() ? std::optional(res->first) : std::optional<data::fiducials_series::FiducialSet>();
}

//------------------------------------------------------------------------------

[[nodiscard]] std::vector<data::fiducials_series::Fiducial*> getPointFiducialsPtr(
    data::fiducials_series::FiducialSet& fiducialSet
)
{
    std::vector<data::fiducials_series::Fiducial*> pointFiducials;
    std::ranges::for_each(
        fiducialSet.fiducialSequence,
        [&pointFiducials](data::fiducials_series::Fiducial& fiducial)
            {
                if(fiducial.shapeType == data::fiducials_series::Shape::POINT)
                {
                    pointFiducials.push_back(&fiducial);
                }
            });
    return pointFiducials;
}

//------------------------------------------------------------------------------

[[nodiscard]] std::vector<std::string> getGroupNames(const LandmarksOrImageSeriesConstLock& lock)
{
    if(lock.landmarks != nullptr)
    {
        return lock.landmarks->getGroupNames();
    }

    if(lock.imageSeries != nullptr)
    {
        return lock.imageSeries->getFiducials()->getPointFiducialsGroupNames();
    }

    SIGHT_ASSERT("Either 'landmarks' or 'fiducialsSeries' must be configured as inout", false);
    return {};
}

//------------------------------------------------------------------------------

[[nodiscard]] std::optional<std::size_t> getNumberOfPointsInGroup(
    const std::string& groupName,
    const LandmarksOrImageSeriesConstLock& lock
)
{
    if(lock.landmarks != nullptr)
    {
        return lock.landmarks->getGroup(groupName).m_points.size();
    }

    if(lock.imageSeries != nullptr)
    {
        return lock.imageSeries->getFiducials()->getNumberOfPointsInGroup(groupName);
    }

    SIGHT_ASSERT("Either 'landmarks' or 'fiducialsSeries' must be configured as inout", false);
    return {};
}

//------------------------------------------------------------------------------

void setPoint(
    data::fiducials_series::FiducialSet& fiducialSet,
    data::fiducials_series::Fiducial& fiducial,
    const data::landmarks::PointType& point,
    data::image_series::sptr imageSeries
)
{
    SIGHT_ASSERT("Only point fiducials are supported", fiducial.shapeType == data::fiducials_series::Shape::POINT);
    if(!fiducialSet.frameOfReferenceUID.has_value())
    {
        std::string frameOfReferenceUID = imageSeries->getStringValue(
            data::dicom::attribute::Keyword::FrameOfReferenceUID
        );
        if(frameOfReferenceUID.empty())
        {
            // Generate a frame of reference UID if the image doesn't have one. It is supposed to be mandatory according
            // to the DICOM standard anyway.
            frameOfReferenceUID = core::tools::UUID::generate();
            imageSeries->setStringValue(data::dicom::attribute::Keyword::FrameOfReferenceUID, frameOfReferenceUID);
        }

        fiducialSet.frameOfReferenceUID = frameOfReferenceUID;
    }

    fiducial.contourData = {data::fiducials_series::Point3 {.x = point[0], .y = point[1], .z = point[2]}};
    // If both ContourData and GraphicCoordinatesDataSequence are set, they must be synchronized, but I'm too lazy
    // to do that, so I simply get rid of GraphicCoordinatesDataSequence.
    fiducial.graphicCoordinatesDataSequence = std::nullopt;
}

//------------------------------------------------------------------------------

[[nodiscard]] std::optional<std::array<double, 3> > getPoint(
    data::fiducials_series::Fiducial fiducial,
    data::fiducials_series::FiducialSet fiducialSet
)
{
    if(fiducialSet.frameOfReferenceUID.has_value() && !fiducial.contourData.empty())
    {
        data::fiducials_series::Point3 point = fiducial.contourData[0];
        return {{point.x, point.y, point.z}};
    }

    // Position with Graphic Coordinates Data Sequence isn't supported
    return std::nullopt;
}

//------------------------------------------------------------------------------

[[nodiscard]] std::optional<std::array<double, 3> > getPoint(
    const std::string& _groupName,
    std::size_t _index,
    data::fiducials_series::csptr fiducialsSeries
)
{
    std::optional<data::fiducials_series::FiducialSet> fiducialSet = getFiducialSet(_groupName, fiducialsSeries);
    if(!fiducialSet.has_value())
    {
        return std::nullopt;
    }

    std::vector<data::fiducials_series::Fiducial> pointFiducials =
        data::fiducials_series::getPointFiducials(*fiducialSet);
    if(pointFiducials.size() <= _index)
    {
        return std::nullopt;
    }

    return getPoint(pointFiducials[_index], *fiducialSet);
}

//------------------------------------------------------------------------------

[[nodiscard]] std::optional<std::array<double, 3> > getPoint(
    const std::string& _groupName,
    std::size_t _index,
    const LandmarksOrImageSeriesConstLock& lock
)
{
    if(lock.landmarks != nullptr)
    {
        return lock.landmarks->getPoint(_groupName, _index);
    }

    if(lock.imageSeries != nullptr)
    {
        return getPoint(_groupName, _index, lock.imageSeries->getFiducials());
    }

    SIGHT_ASSERT("Either 'landmarks' or 'imageSeries' must be configured as inout", false);
    return {};
}

//------------------------------------------------------------------------------

[[nodiscard]] std::optional<data::landmarks::LandmarksGroup> getGroup(
    const std::string& _groupName,
    const LandmarksOrImageSeriesConstPtr& lock
)
{
    if(lock.landmarks != nullptr)
    {
        if(!lock.landmarks->hasGroup(_groupName))
        {
            return std::nullopt;
        }

        return lock.landmarks->getGroup(_groupName);
    }

    if(lock.imageSeries != nullptr)
    {
        return lock.imageSeries->getFiducials()->getGroup(_groupName);
    }

    SIGHT_ASSERT("Either 'landmarks' or 'imageSeries' must be configured as inout", false);
    return {};
}

//------------------------------------------------------------------------------

[[nodiscard]] bool hasGroup(const std::string& _groupName, const LandmarksOrImageSeriesConstLock& lock)
{
    if(lock.landmarks != nullptr)
    {
        return lock.landmarks->hasGroup(_groupName);
    }

    if(lock.imageSeries != nullptr)
    {
        return getFiducialSet(_groupName, lock.imageSeries->getFiducials()).has_value();
    }

    SIGHT_ASSERT("Either 'landmarks' or 'imageSeries' must be configured as inout", false);
    return {};
}

//------------------------------------------------------------------------------

std::array<float, 3> getCurrentSlicePos(const data::image& image)
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

} // namespace

//------------------------------------------------------------------------------

Ogre::Vector3 landmarks::getCamDirection(const Ogre::Camera* const _cam)
{
    const Ogre::Matrix4 view = _cam->getViewMatrix();
    Ogre::Vector3 direction(view[2][0], view[2][1], view[2][2]);
    direction.normalise();
    return -direction;
}

//-----------------------------------------------------------------------------

landmarks::landmarks() noexcept
{
    new_slot(Slots::REMOVE_ALL, &landmarks::removeAll, this);
    new_slot(Slots::REMOVE_GROUP, &landmarks::removeGroup, this);
    new_slot(Slots::MODIFY_GROUP, &landmarks::modifyGroup, this);
    new_slot(Slots::MODIFY_POINT, &landmarks::modifyPoint, this);
    new_slot(Slots::ADD_POINT, &landmarks::addPoint, this);
    new_slot(Slots::REMOVE_POINT, &landmarks::removePoint, this);
    new_slot(Slots::INSERT_POINT, &landmarks::insertPoint, this);
    new_slot(Slots::SELECT_POINT, &landmarks::selectPoint, this);
    new_slot(Slots::DESELECT_POINT, &landmarks::deselectPoint, this);
    new_slot(Slots::SLICE_TYPE, &landmarks::changeSliceType, this);
    new_slot(Slots::SLICE_INDEX, &landmarks::changeSliceIndex, this);
    new_slot(Slots::RENAME_GROUP, &landmarks::renameGroup, this);
    new_slot(Slots::SET_CURRENT_GROUP, &landmarks::setCurrentGroup, this);
    new_slot(Slots::REMOVE_LANDMARKS, &landmarks::removeLandmarks, this);
    new_slot(Slots::CREATE_LANDMARK, &landmarks::createLandmark, this);
    new_slot(Slots::CONFIGURE_LANDMARKS, &landmarks::configureLandmarks, this);
    new_slot(Slots::ENABLE_EDIT_MODE, &landmarks::enableEditMode, this);
    new_slot(Slots::DISABLE_EDIT_MODE, &landmarks::disableEditMode, this);
    new_slot(
        Slots::TOGGLE_EDIT_MODE,
        [this]
        {
            (m_editMode& EditMode::EDIT) == EditMode::EDIT ? disableEditMode() : enableEditMode();
        });
    new_slot(Slots::CHANGE_EDIT_MODE, [this](bool editMode){editMode ? enableEditMode() : disableEditMode();});
    new_slot(Slots::ENABLE_MOVE_MODE, &landmarks::enableMoveMode, this);
    new_slot(Slots::DISABLE_MOVE_MODE, &landmarks::disableMoveMode, this);
    new_slot(
        Slots::TOGGLE_MOVE_MODE,
        [this]
        {
            (m_editMode& EditMode::MOVE) == EditMode::MOVE ? disableMoveMode() : enableMoveMode();
        });
    new_slot(Slots::CHANGE_MOVE_MODE, [this](bool editMode){editMode ? enableMoveMode() : disableMoveMode();});
}

//-----------------------------------------------------------------------------

void landmarks::configuring()
{
    configureParams();

    const auto& config = get_config();

    setTransformId(
        config.get<std::string>(
            sight::viz::scene3d::transformable::s_TRANSFORM_CONFIG,
            get_id() + "_transform"
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
    static const std::string s_MODIFY                 = s_CONFIG + "modify";

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
    auto color = std::make_shared<sight::data::color>();
    color->setRGBA(config.get<std::string>(s_INITIAL_COLOR, "#FFFF00FF"));
    m_currentColor = {color->red(), color->green(), color->blue(), color->alpha()};

    // Initial size
    m_currentSize = config.get<float>(s_INITIAL_SIZE, m_currentSize);

    // Initial shape
    const std::string& initialShape = config.get<std::string>(s_INITIAL_SHAPE, "sphere");
    if(initialShape == "sphere")
    {
        m_currentShape = sight::data::landmarks::Shape::SPHERE;
    }
    else if(initialShape == "cube")
    {
        m_currentShape = sight::data::landmarks::Shape::CUBE;
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

    std::string modify = config.get(s_MODIFY, "all");
    SIGHT_ERROR_IF(
        "'modify' config has wrong value '" << modify << "', the only authorized values are 'all' and 'group'",
        modify != "all" && modify != "group"
    );
    m_canOnlyModifyCurrent = (modify == "group");

    SIGHT_ASSERT(
        "Only one of 'landmarks' or 'imageSeries' must be present as inout",
        (m_landmarks.const_lock() != nullptr) + (m_imageSeries.const_lock() != nullptr) == 1
    );
    SIGHT_ASSERT(
        "Either 'image' or 'imageSeries' must be present as parameter",
        (m_image.const_lock() != nullptr) + (m_imageSeries.const_lock() != nullptr) <= 1
    );
}

//-----------------------------------------------------------------------------

void landmarks::starting()
{
    initialize();

    getRenderService()->makeCurrent();

    auto* rootSceneNode = getSceneManager()->getRootSceneNode();
    m_transNode = getOrCreateTransformNode(rootSceneNode);

    m_material = std::make_shared<data::material>();
    m_material->setDiffuse(std::make_shared<data::color>(1.F, 1.F, 1.F, 1.F));

    // Register the material adaptor.
    m_materialAdaptor = this->registerService<sight::viz::scene3d::IMaterialAdaptor>(
        "sight::module::viz::scene3d::adaptor::material"
    );
    m_materialAdaptor->set_inout(m_material, sight::viz::scene3d::IMaterialAdaptor::s_MATERIAL_INOUT, true);
    m_materialAdaptor->configure(
        this->get_id() + m_materialAdaptor->get_id(),
        this->get_id() + m_materialAdaptor->get_id(),
        this->getRenderService(),
        m_layerID
    );
    m_materialAdaptor->start();

    m_materialAdaptor->getMaterialFw()->setHasVertexColor(true);
    m_materialAdaptor->update();

    if(m_interactive)
    {
        auto interactor = std::dynamic_pointer_cast<sight::viz::scene3d::interactor::base>(get_sptr());
        getLayer()->addInteractor(interactor, m_priority);
    }

    // Draw landmarks.
    updating();

    auto interactor    = getLayer()->getRenderService()->getInteractorManager();
    auto qtInteractor  = std::dynamic_pointer_cast<window_interactor>(interactor);
    auto* parentWidget = qtInteractor->getQtWidget();
    m_contextualMenu = new QWidget(parentWidget);
    m_contextualMenu->setStyleSheet(".QWidget {background-color: none;}");
    m_contextualMenu->hide();
    auto* layout = new QHBoxLayout;
    QIcon trashBinIcon(QString::fromStdString(
                           core::runtime::get_module_resource_file_path(
                               "sight::module::ui::flaticons/RedTrashBin.svg"
                           ).string()
    ));
    auto* binButton             = new QPushButton(trashBinIcon, "");
    const std::string serviceID = get_id().substr(get_id().find_last_of('_') + 1);
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
                          core::runtime::get_module_resource_file_path(
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

    if(auto imageSeries = m_imageSeries.lock())
    {
        imageSeries->getFiducials()->setGroupNamesForPointFiducials();
    }
}

//-----------------------------------------------------------------------------

service::connections_t landmarks::auto_connections() const
{
    service::connections_t connections;

    connections.push(s_TRANSFORM_CONFIG, data::matrix4::MODIFIED_SIG, service::slots::UPDATE);

    connections.push(s_LANDMARKS_INOUT, data::landmarks::MODIFIED_SIG, service::slots::UPDATE);
    connections.push(s_LANDMARKS_INOUT, data::landmarks::GROUP_REMOVED_SIG, Slots::REMOVE_GROUP);
    connections.push(s_LANDMARKS_INOUT, data::landmarks::GROUP_MODIFIED_SIG, Slots::MODIFY_GROUP);
    connections.push(s_LANDMARKS_INOUT, data::landmarks::GROUP_RENAMED_SIG, Slots::RENAME_GROUP);
    connections.push(s_LANDMARKS_INOUT, data::landmarks::POINT_MODIFIED_SIG, Slots::MODIFY_POINT);
    connections.push(s_LANDMARKS_INOUT, data::landmarks::POINT_ADDED_SIG, Slots::ADD_POINT);
    connections.push(s_LANDMARKS_INOUT, data::landmarks::POINT_REMOVED_SIG, Slots::REMOVE_POINT);
    connections.push(s_LANDMARKS_INOUT, data::landmarks::POINT_INSERTED_SIG, Slots::INSERT_POINT);
    connections.push(s_LANDMARKS_INOUT, data::landmarks::POINT_SELECTED_SIG, Slots::SELECT_POINT);
    connections.push(s_LANDMARKS_INOUT, data::landmarks::POINT_DESELECTED_SIG, Slots::DESELECT_POINT);

    connections.push(s_IMAGE_SERIES_INOUT, data::image_series::MODIFIED_SIG, service::slots::UPDATE);
    connections.push(s_IMAGE_SERIES_INOUT, data::has_fiducials::signals::GROUP_REMOVED, Slots::REMOVE_GROUP);
    connections.push(s_IMAGE_SERIES_INOUT, data::has_fiducials::signals::GROUP_MODIFIED, Slots::MODIFY_GROUP);
    connections.push(s_IMAGE_SERIES_INOUT, data::has_fiducials::signals::GROUP_RENAMED, Slots::RENAME_GROUP);
    connections.push(s_IMAGE_SERIES_INOUT, data::has_fiducials::signals::POINT_MODIFIED, Slots::MODIFY_POINT);
    connections.push(s_IMAGE_SERIES_INOUT, data::has_fiducials::signals::POINT_ADDED, Slots::ADD_POINT);
    connections.push(s_IMAGE_SERIES_INOUT, data::has_fiducials::signals::POINT_REMOVED, Slots::REMOVE_POINT);
    connections.push(s_IMAGE_SERIES_INOUT, data::has_fiducials::signals::POINT_INSERTED, Slots::INSERT_POINT);
    connections.push(s_IMAGE_SERIES_INOUT, data::has_fiducials::signals::POINT_SELECTED, Slots::SELECT_POINT);
    connections.push(s_IMAGE_SERIES_INOUT, data::has_fiducials::signals::POINT_DESELECTED, Slots::DESELECT_POINT);

    connections.push(s_IMAGE_INPUT, data::image::SLICE_TYPE_MODIFIED_SIG, Slots::SLICE_TYPE);
    connections.push(s_IMAGE_INPUT, data::image::SLICE_INDEX_MODIFIED_SIG, Slots::SLICE_INDEX);

    connections.push(s_IMAGE_SERIES_INOUT, data::image::SLICE_TYPE_MODIFIED_SIG, Slots::SLICE_TYPE);
    connections.push(s_IMAGE_SERIES_INOUT, data::image::SLICE_INDEX_MODIFIED_SIG, Slots::SLICE_INDEX);

    return connections;
}

//-----------------------------------------------------------------------------

void landmarks::updating()
{
    // Delete all groups.
    removeAllManualObjects();

    LandmarksOrImageSeriesConstLock lock = constLockLandmarks();

    // Create all point.
    for(const auto& groupName : getGroupNames(lock))
    {
        std::optional<std::size_t> nbOfPointsInGroup = getNumberOfPointsInGroup(groupName, lock);
        for(std::size_t index = 0 ; index < nbOfPointsInGroup ; ++index)
        {
            this->createManualObject(groupName, index, lock);
        }
    }
}

//-----------------------------------------------------------------------------

void landmarks::stopping()
{
    if(m_interactive)
    {
        auto interactor = std::dynamic_pointer_cast<sight::viz::scene3d::interactor::base>(get_sptr());
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

void landmarks::removeAllManualObjects()
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

void landmarks::removeAll()
{
    m_contextualMenu->hide();

    LandmarksOrImageSeriesLock lfLock = lockLandmarks();
    if(lfLock.landmarks != nullptr)
    {
        for(const std::string& grp : lfLock.landmarks->getGroupNames())
        {
            lfLock.landmarks->removeGroup(grp);
            lfLock.landmarks->signal<data::landmarks::GroupRemovedSignalType>(data::landmarks::GROUP_REMOVED_SIG)->
            async_emit(grp);
        }
    }
    else if(lfLock.imageSeries != nullptr)
    {
        std::vector<std::string> deletedGroupNames;
        std::vector<data::fiducials_series::FiducialSet> fiducialSets =
            lfLock.imageSeries->getFiducials()->getFiducialSets();
        for(auto itFiducialSet = fiducialSets.begin() ; itFiducialSet != fiducialSets.end() ; )
        {
            if(!itFiducialSet->groupName.has_value())
            {
                ++itFiducialSet;
                continue;
            }

            bool hasDeleted = false;
            for(auto itFiducial = itFiducialSet->fiducialSequence.begin() ;
                itFiducial != itFiducialSet->fiducialSequence.end() ; )
            {
                if(itFiducial->shapeType == data::fiducials_series::Shape::POINT)
                {
                    itFiducial = itFiducialSet->fiducialSequence.erase(itFiducial);
                    hasDeleted = true;
                }
                else
                {
                    ++itFiducial;
                }
            }

            if(hasDeleted)
            {
                deletedGroupNames.push_back(*itFiducialSet->groupName);
            }

            if(itFiducialSet->fiducialSequence.empty())
            {
                itFiducialSet = fiducialSets.erase(itFiducialSet);
            }
            else
            {
                ++itFiducialSet;
            }
        }

        lfLock.imageSeries->getFiducials()->setFiducialSets(fiducialSets);
        for(const std::string& group : deletedGroupNames)
        {
            lfLock.imageSeries->signal<data::has_fiducials::signals::GroupRemoved>(
                data::has_fiducials::signals::GROUP_REMOVED
            )->async_emit(group);
        }
    }
}

//------------------------------------------------------------------------------

void landmarks::removeGroup(std::string _groupName)
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

void landmarks::modifyGroup(std::string _groupName)
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
    LandmarksOrImageSeriesConstLock lock = constLockLandmarks();

    std::optional<std::size_t> groupSize = getNumberOfPointsInGroup(_groupName, lock);
    if(!groupSize.has_value())
    {
        return;
    }

    // Re-create the group.
    for(std::size_t index = 0 ; index < groupSize ; ++index)
    {
        createManualObject(_groupName, index, lock);
    }

    // Re-run selected landmark threads
    for(std::size_t index : indexes)
    {
        this->selectPoint(_groupName, index);
    }
}

//------------------------------------------------------------------------------

void landmarks::renameGroup(std::string _oldGroupName, std::string _newGroupName)
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

void landmarks::setCurrentGroup(std::string _newCurrentGroupName)
{
    m_currentGroup = _newCurrentGroupName;
}

//------------------------------------------------------------------------------

void landmarks::modifyPoint(std::string _groupName, std::size_t _index)
{
    std::optional<std::array<double, 3> > maybePoint = getPoint(_groupName, _index, constLockLandmarks());

    if(!maybePoint.has_value())
    {
        return;
    }

    std::array<double, 3> point = *maybePoint;

    for(auto& m_manualObject : m_manualObjects)
    {
        const auto& name = m_manualObject->m_groupName;
        if(name.find(_groupName) != std::string::npos && m_manualObject->m_index == _index)
        {
            const Ogre::Vector3 position(static_cast<float>(point[0]),
                                         static_cast<float>(point[1]),
                                         static_cast<float>(point[2]));
            m_manualObject->m_node->setPosition(position);
            updateLandmarkVisibility(*m_manualObject, getGroup(m_manualObject->m_groupName, constLockLandmarks()));
            break;
        }
    }

    getRenderService()->requestRender();
}

//------------------------------------------------------------------------------

void landmarks::addPoint(std::string _groupName)
{
    // Make the context as current.
    getRenderService()->makeCurrent();

    // Get landmarks.
    LandmarksOrImageSeriesConstLock lock = constLockLandmarks();

    // Get the last index.
    std::optional<std::size_t> numberOfPoints = getNumberOfPointsInGroup(_groupName, lock);

    if(!numberOfPoints.has_value() || numberOfPoints == 0)
    {
        return;
    }

    // Add the new point.
    this->createManualObject(_groupName, *numberOfPoints - 1, lock);
}

//------------------------------------------------------------------------------

void landmarks::removePoint(std::string _groupName, std::size_t _index)
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

void landmarks::insertPoint(std::string _groupName, std::size_t _index)
{
    // Make the context as current
    getRenderService()->makeCurrent();

    createManualObject(_groupName, _index, constLockLandmarks());
}

//------------------------------------------------------------------------------

std::shared_ptr<landmarks::Landmark> landmarks::createManualObject(
    const std::string& _groupName,
    std::size_t _index,
    data::landmarks::LandmarksGroup _groupData,
    data::landmarks::PointType _pointPos
)
{
    // Create the point name.
    const std::string pointName = _groupName + "_" + std::to_string(_index);

    // Create the manual object.
    const Ogre::ColourValue color =
        Ogre::ColourValue(_groupData.m_color[0], _groupData.m_color[1], _groupData.m_color[2], _groupData.m_color[3]);

    Ogre::SceneManager* sceneMgr = this->getSceneManager();
    Ogre::ManualObject* object   = sceneMgr->createManualObject(this->get_id() + "_" + pointName + "_object");
    switch(_groupData.m_shape)
    {
        case data::landmarks::Shape::SPHERE:
            sight::viz::scene3d::helper::ManualObject::createSphere(
                object,
                m_materialAdaptor->getMaterialName(),
                color,
                _groupData.m_size * 0.5F
            );
            break;

        case data::landmarks::Shape::CUBE:
            sight::viz::scene3d::helper::ManualObject::createCube(
                object,
                m_materialAdaptor->getMaterialName(),
                color,
                _groupData.m_size
            );
            break;
    }

    object->setQueryFlags(m_landmarksQueryFlag);

    Ogre::SceneNode* node = m_transNode->createChildSceneNode(this->get_id() + "_" + pointName + "_node");

    // Set the point to the right position.
    node->setPosition(Ogre::Real(_pointPos[0]), Ogre::Real(_pointPos[1]), Ogre::Real(_pointPos[2]));

    // Attach data.
    node->attachObject(object);

    // Create the label.
    sight::viz::scene3d::IText::sptr text;
    if(m_enableLabels)
    {
        // Create the label.
        text = sight::viz::scene3d::IText::make(this->getLayer());
        text->setFontSize(m_fontSize);
        text->setText(pointName);
        text->setTextColor(color);
        text->setVisible(_groupData.m_visibility && m_isVisible);
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
        auto slotTextEdited = core::com::new_slot(
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

                LandmarksOrImageSeriesLock lock = lockLandmarks();
                std::string oldGroupName        = landmark->m_groupName;
                if(lock.landmarks != nullptr)
                {
                    lock.landmarks->renameGroup(oldGroupName, newGroupName);
                    renameGroup(oldGroupName, newGroupName);
                    auto signal =
                        lock.landmarks->signal<data::landmarks::GroupRenamedSignalType>(
                            data::landmarks::GROUP_RENAMED_SIG
                        );
                    signal->async_emit(
                        oldGroupName,
                        newGroupName
                    );
                }
                else if(lock.imageSeries != nullptr)
                {
                    std::optional<std::pair<data::fiducials_series::FiducialSet, std::size_t> > fiducialSet =
                        lock.imageSeries->getFiducials()->getFiducialSetAndIndex(oldGroupName);
                    if(!fiducialSet.has_value())
                    {
                        SIGHT_WARN("Couldn't rename group '" << oldGroupName << "', it doesn't exist");
                        return;
                    }

                    lock.imageSeries->getFiducials()->setGroupName(fiducialSet->second, newGroupName);
                    renameGroup(oldGroupName, newGroupName);
                    lock.imageSeries->signal<data::has_fiducials::signals::GroupRenamed>(
                        data::has_fiducials::signals::GROUP_RENAMED
                    )->async_emit(oldGroupName, newGroupName);
                }
            });
        slotTextEdited->set_worker(core::thread::get_default_worker());
        newLandmark->m_label->signal(sight::viz::scene3d::IText::TEXT_EDITED_SIGNAL)->connect(slotTextEdited);
        newLandmark->m_slots.push_back(slotTextEdited);

        auto slotEditingFinish = core::com::new_slot(
            [label = std::weak_ptr(newLandmark->m_label)]
            {
                label.lock()->setEditMode(false);
            });
        slotEditingFinish->set_worker(core::thread::get_default_worker());
        newLandmark->m_label->signal(sight::viz::scene3d::IText::EDITING_FINISHED_SIGNAL)->connect(slotEditingFinish);
        newLandmark->m_slots.push_back(slotEditingFinish);
    }

    updateLandmarkVisibility(*m_manualObjects.back(), _groupData);

    // Request the rendering.
    this->requestRender();

    return newLandmark;
}

//------------------------------------------------------------------------------

std::shared_ptr<landmarks::Landmark> landmarks::createManualObject(
    const std::string& _groupName,
    std::size_t _index,
    data::landmarks::csptr _landmarks
)
{
    const data::landmarks::LandmarksGroup& group = _landmarks->getGroup(_groupName);
    return createManualObject(_groupName, _index, group, group.m_points[_index]);
}

//------------------------------------------------------------------------------

std::shared_ptr<landmarks::Landmark> landmarks::createManualObject(
    const std::string& _groupName,
    std::size_t _index,
    LandmarksOrImageSeriesConstPtr _lf
)
{
    if(_lf.landmarks != nullptr)
    {
        return createManualObject(_groupName, _index, _lf.landmarks);
    }

    if(_lf.imageSeries != nullptr)
    {
        std::optional<data::landmarks::LandmarksGroup> group = _lf.imageSeries->getFiducials()->getGroup(_groupName);
        if(!group.has_value())
        {
            SIGHT_ASSERT("The group '" << _groupName << "' doesn't exist", false);
            return nullptr;
        }

        if(group->m_points.size() <= _index)
        {
            SIGHT_ASSERT("The point of '" << _groupName << '_' << _index << "' doesn't exist", false);
            return nullptr;
        }

        return createManualObject(_groupName, _index, *group, group->m_points[_index]);
    }

    SIGHT_ASSERT("Either 'landmarks' or 'fiducialsSeries' must be configured as inout", false);
    return nullptr;
}

//------------------------------------------------------------------------------

void landmarks::selectPoint(std::string _groupName, std::size_t _index)
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
                    std::make_shared<SelectedLandmark>(this->worker()->create_timer(), m_manualObject);
                m_selectedLandmarks.push_back(selectedLandmark);

                // Run a thread that change the selected point.
                core::thread::timer::time_duration_t duration = std::chrono::milliseconds(500);
                selectedLandmark->m_timer->set_function(
                    [this, selectedLandmark](auto&& ...)
                    {
                        hightlight(selectedLandmark);
                    });
                selectedLandmark->m_timer->set_duration(duration);
                selectedLandmark->m_timer->start();
            }

            break;
        }
    }
}

//------------------------------------------------------------------------------

void landmarks::deselectPoint(std::string _groupName, std::size_t _index)
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

void landmarks::hightlight(std::shared_ptr<SelectedLandmark> _selectedLandmark)
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

void landmarks::changeSliceType(int _from, int _to)
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

void landmarks::changeSliceIndex(int /*_axialIndex*/, int /*_frontalIndex*/, int /*_sagittalIndex*/)
{
    // Make the context as current.
    this->getRenderService()->makeCurrent();

    bool imageExists = false;
    {
        ImageOrImageSeriesConstLock lock = constLockImage();

        if(lock.image || lock.imageSeries)
        {
            imageExists = true;

            this->getRenderService()->makeCurrent();
        }
    }

    if(imageExists)
    {
        this->updateLandmarksVisibility();

        // Request the rendering.
        this->requestRender();
    }
}

//------------------------------------------------------------------------------

void landmarks::removeLandmarks()
{
    std::optional<std::int64_t> slice_index;
    {
        ImageOrImageSeriesConstLock lock = constLockImage();
        data::image::csptr image         = lock.image ? lock.image.get_shared() : lock.imageSeries.get_shared();
        SIGHT_THROW_IF("Data is null.", !image);

        slice_index = sight::data::helper::MedicalImage::getSliceIndex(
            *image,
            m_orientation
        );
    }

    if(!slice_index)
    {
        // No slice selected, early return
        return;
    }

    LandmarksOrImageSeriesLock flLock = lockLandmarks();

    if(flLock.landmarks == nullptr && flLock.imageSeries == nullptr)
    {
        // No need to continue if there is no landmarks
        return;
    }

    bool hasDeleted = false;

    if(flLock.landmarks != nullptr)
    {
        for(const auto& name : flLock.landmarks->getGroupNames())
        {
            auto& group = flLock.landmarks->getGroup(name);

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
    }
    else if(flLock.imageSeries != nullptr)
    {
        data::fiducials_series::sptr fiducialsSeries = flLock.imageSeries.get_shared()->getFiducials();
        for(const std::string& name : fiducialsSeries->getPointFiducialsGroupNames())
        {
            std::optional<std::pair<data::fiducials_series::FiducialSet, std::size_t> > fiducialSet =
                fiducialsSeries->getFiducialSetAndIndex(name);
            if(!fiducialSet.has_value())
            {
                continue;
            }

            std::optional<data::landmarks::SizeType> size = fiducialSet->first.size;
            if(!size.has_value())
            {
                size                    = m_currentSize;
                fiducialSet->first.size = size;
            }

            for(auto it = fiducialSet->first.fiducialSequence.begin() ;
                it != fiducialSet->first.fiducialSequence.end() ; )
            {
                std::optional<std::array<double, 3> > point = getPoint(*it, fiducialSet->first);
                if(!point.has_value())
                {
                    continue;
                }

                if(isLandmarkVisible(*point, *size))
                {
                    it         = fiducialSet->first.fiducialSequence.erase(it);
                    hasDeleted = true;
                }
            }

            fiducialsSeries->setFiducialSet(fiducialSet->second, fiducialSet->first);
        }
    }

    if(hasDeleted)
    {
        data::object::sptr object;
        if(flLock.landmarks != nullptr)
        {
            object = flLock.landmarks.get_shared();
        }
        else if(flLock.imageSeries != nullptr)
        {
            object = flLock.imageSeries->getFiducials();
        }

        object->signal<sight::data::object::ModifiedSignalType>(sight::data::object::MODIFIED_SIG)->async_emit();
    }
}

//------------------------------------------------------------------------------

void landmarks::configureLandmarks(sight::viz::scene3d::LandmarksConfiguration configuration)
{
    if(configuration.group)
    {
        m_currentGroup = *configuration.group;
    }

    if(configuration.color)
    {
        m_currentColor = *configuration.color;
    }

    if(configuration.size)
    {
        m_currentSize = *configuration.size;
    }

    if(configuration.shape)
    {
        m_currentShape = *configuration.shape;
    }

    if(configuration.groupMax)
    {
        if(*configuration.groupMax >= 0)
        {
            m_groupMax[m_currentGroup] = std::size_t(*configuration.groupMax);
        }
        else
        {
            m_groupMax.erase(m_currentGroup);
        }
    }

    if(configuration.visibleMax)
    {
        if(*configuration.visibleMax >= 0)
        {
            m_visibleMax = *configuration.visibleMax;
        }
        else
        {
            m_visibleMax = std::nullopt;
        }
    }

    if(configuration.totalMax)
    {
        if(*configuration.totalMax >= 0)
        {
            m_totalMax = *configuration.totalMax;
        }
        else
        {
            m_totalMax = std::nullopt;
        }
    }

    if(configuration.movableGroups)
    {
        m_movableGroups = *configuration.movableGroups;
    }
}

//------------------------------------------------------------------------------

void landmarks::enableEditMode()
{
    if((m_editMode& EditMode::EDIT) == EditMode::EDIT)
    {
        return;
    }

    m_editMode |= EditMode::EDIT;

    setCursor(Qt::CrossCursor);

    if(m_eventFilter == nullptr)
    {
        auto interactor    = getLayer()->getRenderService()->getInteractorManager();
        auto qtInteractor  = std::dynamic_pointer_cast<window_interactor>(interactor);
        auto* parentWidget = qtInteractor->getQtWidget();
        m_eventFilter = std::make_unique<DeleteContextualMenuWhenFocusOut>(this);
        parentWidget->installEventFilter(m_eventFilter.get());
    }

    m_editModeChanged->async_emit(true);
}

//------------------------------------------------------------------------------

void landmarks::disableEditMode()
{
    if((m_editMode& EditMode::EDIT) != EditMode::EDIT)
    {
        return;
    }

    m_editMode &= ~EditMode::EDIT;

    if(m_editMode == EditMode::DISPLAY)
    {
        auto interactor    = getLayer()->getRenderService()->getInteractorManager();
        auto qtInteractor  = std::dynamic_pointer_cast<window_interactor>(interactor);
        auto* parentWidget = qtInteractor->getQtWidget();
        parentWidget->unsetCursor();
    }

    m_contextualMenu->hide();

    if(m_enableLabels && m_renamingAllowed)
    {
        std::ranges::for_each(m_manualObjects, [](std::shared_ptr<Landmark> l){l->m_label->setEditMode(false);});
    }

    m_editModeChanged->async_emit(false);
}

//------------------------------------------------------------------------------

void landmarks::enableMoveMode()
{
    if((m_editMode& EditMode::MOVE) == EditMode::MOVE)
    {
        return;
    }

    m_editMode |= EditMode::MOVE;

    setCursor(Qt::CrossCursor);
}

//------------------------------------------------------------------------------

void landmarks::disableMoveMode()
{
    if((m_editMode& EditMode::MOVE) != EditMode::MOVE)
    {
        return;
    }

    m_editMode &= ~EditMode::MOVE;

    if(m_editMode == EditMode::DISPLAY)
    {
        auto interactor    = getLayer()->getRenderService()->getInteractorManager();
        auto qtInteractor  = std::dynamic_pointer_cast<window_interactor>(interactor);
        auto* parentWidget = qtInteractor->getQtWidget();
        parentWidget->unsetCursor();
    }
}

//------------------------------------------------------------------------------

void landmarks::createLandmark(sight::data::landmarks::PointType point)
{
    createAndPickLandmark(point, false);
}

//------------------------------------------------------------------------------

void landmarks::createAndPickLandmark(const sight::data::landmarks::PointType& point, bool pick)
{
    if(isMaxLandmarksReached())
    {
        // No need to continue if the maximum number of landmarks is reached
        SIGHT_DEBUG("Maximum number of landmarks reached.");
        return;
    }

    data::landmarks::LandmarksGroup group {{}, {}, {}, {}};
    std::size_t index {};

    {
        LandmarksOrImageSeriesLock lfLock = lockLandmarks();

        if(lfLock.landmarks != nullptr)
        {
            // If the group does not exist, we create it.
            if(!lfLock.landmarks->hasGroup(m_currentGroup))
            {
                lfLock.landmarks->addGroup(m_currentGroup, m_currentColor, m_currentSize, m_currentShape);
                const auto& sig = lfLock.landmarks->signal<sight::data::landmarks::GroupAddedSignalType>(
                    sight::data::landmarks::GROUP_ADDED_SIG
                );

                sig->async_emit(m_currentGroup);
            }

            lfLock.landmarks->addPoint(m_currentGroup, point);

            // Get the last index.
            group = lfLock.landmarks->getGroup(m_currentGroup);
            index = group.m_points.size() - 1;

            // Block the signal to avoid being called back.
            const auto& sig = lfLock.landmarks->signal<sight::data::landmarks::PointAddedSignalType>(
                sight::data::landmarks::POINT_ADDED_SIG
            );

            sight::core::com::connection::blocker blocker(sig->get_connection(slot(Slots::ADD_POINT)));
            sig->async_emit(m_currentGroup);
        }
        else if(lfLock.imageSeries != nullptr)
        {
            std::optional<std::pair<data::fiducials_series::FiducialSet, std::size_t> > res =
                lfLock.imageSeries->getFiducials()->getFiducialSetAndIndex(m_currentGroup);
            data::fiducials_series::FiducialSet fiducialSet;
            std::size_t indexOfGroup {};
            // If the group does not exist, we create it.
            if(!res.has_value())
            {
                fiducialSet.groupName = m_currentGroup;
                fiducialSet.color     = m_currentColor;
                fiducialSet.size      = m_currentSize;
                switch(m_currentShape)
                {
                    case data::landmarks::Shape::SPHERE:
                        fiducialSet.shape = data::fiducials_series::PrivateShape::SPHERE;
                        break;

                    case data::landmarks::Shape::CUBE:
                        fiducialSet.shape = data::fiducials_series::PrivateShape::CUBE;
                }

                indexOfGroup = lfLock.imageSeries->getFiducials()->getFiducialSets().size();
                lfLock.imageSeries->signal<data::has_fiducials::signals::GroupAdded>(
                    data::has_fiducials::signals::GROUP_ADDED
                )->async_emit(m_currentGroup);
            }
            else
            {
                std::tie(fiducialSet, indexOfGroup) = *res;
            }

            data::fiducials_series::Fiducial fiducial;
            fiducial.shapeType = data::fiducials_series::Shape::POINT;
            index              = data::fiducials_series::getPointFiducials(fiducialSet).size();
            std::string fiducialName = m_currentGroup + '_' + std::to_string(index);
            fiducial.fiducialDescription = fiducialName; // TODO: Add a more appropriate description?
            fiducial.fiducialIdentifier  = fiducialName;
            fiducial.fiducialUID         = core::tools::UUID::generate();
            setPoint(fiducialSet, fiducial, point, lfLock.imageSeries.get_shared());
            fiducialSet.fiducialSequence.push_back(fiducial);
            lfLock.imageSeries->getFiducials()->setFiducialSet(indexOfGroup, fiducialSet);

            group = *getGroup(m_currentGroup, lfLock);

            // Block the signal to avoid being called back.
            const auto& sig =
                lfLock.imageSeries->signal<data::has_fiducials::signals::PointAdded>(
                    data::has_fiducials::signals::POINT_ADDED
                );

            sight::core::com::connection::blocker blocker(sig->get_connection(slot(Slots::ADD_POINT)));
            sig->async_emit(m_currentGroup);
        }
        else
        {
            SIGHT_ASSERT("Either 'landmarks' or 'imageSeries' must be configured as inout", false);
            return;
        }
    }

    // Add the new point.
    std::shared_ptr<Landmark> newLandmark = createManualObject(m_currentGroup, index, group, point);

    if(pick)
    {
        m_pickedData = newLandmark;
        m_pickedData->m_node->setScale(s_SELECTED_SCALE, s_SELECTED_SCALE, s_SELECTED_SCALE);
    }
    else
    {
        m_pickedData = nullptr;
    }
}

//------------------------------------------------------------------------------

bool landmarks::isMaxLandmarksReached()
{
    if(!m_totalMax && !m_visibleMax && m_groupMax.empty())
    {
        // Early return if we must not count anything
        return false;
    }

    // Count landmarks.
    LandmarksOrImageSeriesConstLock lock = constLockLandmarks();

    if(m_groupMax.contains(m_currentGroup)
       && (m_groupMax[m_currentGroup] == 0
           || (hasGroup(m_currentGroup, lock)
               && getNumberOfPointsInGroup(m_currentGroup, lock) >= m_groupMax[m_currentGroup])))
    {
        return true;
    }

    if(m_totalMax || m_visibleMax)
    {
        const auto& names = getGroupNames(lock);

        std::size_t max        = 0;
        std::size_t maxVisible = 0;

        for(const auto& name : names)
        {
            std::optional<data::landmarks::LandmarksGroup> maybeGroup = getGroup(name, lock);
            if(!maybeGroup.has_value())
            {
                continue;
            }

            data::landmarks::LandmarksGroup group = *maybeGroup;

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

void landmarks::updateLandmarksVisibility()
{
    // Make the context as current.
    getRenderService()->makeCurrent();

    bool imageExists = false;
    {
        ImageOrImageSeriesConstLock iisLock = constLockImage();
        imageExists = iisLock.image || iisLock.imageSeries;
    }

    // Hide landmarks only if there is an image.
    if(imageExists)
    {
        for(const auto& landmark : m_manualObjects)
        {
            updateLandmarkVisibility(landmark);
        }
    }
}

//------------------------------------------------------------------------------

void landmarks::updateLandmarkVisibility(std::shared_ptr<Landmark> _landmark)
{
    // Make the context as current.
    getRenderService()->makeCurrent();

    updateLandmarkVisibility(*_landmark, constLockLandmarks());
}

//------------------------------------------------------------------------------

void landmarks::updateLandmarkVisibility(Landmark& landmark, std::optional<data::landmarks::LandmarksGroup> group)
{
    const bool must_show =
        [&]
        {
            // Retrieve group
            if(!group.has_value())
            {
                return false;
            }

            if(!group->m_visibility)
            {
                return false;
            }

            const auto& position = landmark.m_node->getPosition();
            return isLandmarkVisible({position[0], position[1], position[2]}, group->m_size);
        }();

    // Show or hide the landmark.
    landmark.m_object->setVisible(must_show);
}

//------------------------------------------------------------------------------

void landmarks::updateLandmarkVisibility(Landmark& landmark, const LandmarksOrImageSeriesConstLock& lock)
{
    updateLandmarkVisibility(landmark, getGroup(landmark.m_groupName, lock));
}

//------------------------------------------------------------------------------

bool landmarks::isLandmarkVisible(
    const data::landmarks::PointType& point,
    data::landmarks::SizeType group_size
) const
{
    if(const auto& iisLock = constLockImage(); iisLock.image || iisLock.imageSeries)
    {
        const auto position       = point[m_orientation];
        data::image::csptr image  = iisLock.image ? iisLock.image.get_shared() : iisLock.imageSeries.get_shared();
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

void landmarks::setVisible(bool _visible)
{
    LandmarksOrImageSeriesConstLock lock = constLockLandmarks();
    for(const auto& landmark : m_manualObjects)
    {
        std::optional<data::landmarks::LandmarksGroup> maybeGroup = getGroup(landmark->m_groupName, lock);
        if(!maybeGroup.has_value())
        {
            continue;
        }

        data::landmarks::LandmarksGroup group = *maybeGroup;
        landmark->m_object->setVisible(_visible && group.m_visibility);
        if(m_enableLabels)
        {
            landmark->m_label->setVisible(_visible && group.m_visibility);
        }
    }

    requestRender();
}

//------------------------------------------------------------------------------

std::optional<Ogre::Vector3> landmarks::getNearestPickedPosition(int _x, int _y)
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

void landmarks::buttonPressEvent(MouseButton _button, Modifier /*_mods*/, int _x, int _y)
{
    m_contextualMenu->hide();

    if(_button != LEFT)
    {
        // We only manage left click.
        return;
    }

    if(m_editMode == EditMode::DISPLAY)
    {
        // If the EDIT mode is off, the landmarks are read-only.
        return;
    }

    const bool mustEdit = (m_editMode& EditMode::EDIT) == EditMode::EDIT;

    // Try to pick a landmark.
    if(const auto& pickedData = tryPick(_x, _y); pickedData)
    {
        // If something is picked, we will select it.
        setCursor(Qt::ClosedHandCursor);
        m_pickedData = pickedData;
        m_pickedData->m_node->setScale(s_SELECTED_SCALE, s_SELECTED_SCALE, s_SELECTED_SCALE);

        // Only show contextual menu if we are in "true" EDIT mode
        m_mustShowContextualMenu = mustEdit;
    }
    else if(mustEdit)
    {
        // Nothing is picked, we will create a new landmark if we are in EDIT mode.
        m_mustShowContextualMenu = false;

        // If nothing is picked, we will create a new landmark.
        if(auto newPos = this->getNearestPickedPosition(_x, _y); newPos)
        {
            setCursor(Qt::ClosedHandCursor);
            createAndPickLandmark({(*newPos)[0], (*newPos)[1], (*newPos)[2]});
        }
    }
}

//------------------------------------------------------------------------------

void landmarks::mouseMoveEvent(MouseButton /*_button*/, Modifier /*_mods*/, int _x, int _y, int /*_dx*/, int /*_dy*/)
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
        LandmarksOrImageSeriesLock lfLock = lockLandmarks();
        if(lfLock.landmarks != nullptr)
        {
            data::landmarks::PointType& point = lfLock.landmarks->getPoint(
                m_pickedData->m_groupName,
                m_pickedData->m_index
            );
            point[0] = newPos[0];
            point[1] = newPos[1];
            point[2] = newPos[2];

            const auto& sig = lfLock.landmarks->signal<data::landmarks::PointModifiedSigType>(
                data::landmarks::POINT_MODIFIED_SIG
            );
            sig->async_emit(m_pickedData->m_groupName, m_pickedData->m_index);
        }
        else if(lfLock.imageSeries != nullptr)
        {
            std::optional<std::pair<data::fiducials_series::FiducialSet, std::size_t> > fiducialSet =
                lfLock.imageSeries->getFiducials()->getFiducialSetAndIndex(m_pickedData->m_groupName);
            if(!fiducialSet.has_value())
            {
                return;
            }

            std::vector<data::fiducials_series::Fiducial*> pointFiducials = getPointFiducialsPtr(fiducialSet->first);
            if(m_pickedData->m_index >= pointFiducials.size())
            {
                return;
            }

            data::fiducials_series::Fiducial& fiducial = *pointFiducials[m_pickedData->m_index];
            setPoint(fiducialSet->first, fiducial, {newPos.x, newPos.y, newPos.z}, lfLock.imageSeries.get_shared());
            lfLock.imageSeries->getFiducials()->setFiducialSet(fiducialSet->second, fiducialSet->first);

            lfLock.imageSeries->signal<data::has_fiducials::signals::PointModified>(
                data::has_fiducials::signals::POINT_MODIFIED
            )->async_emit(m_pickedData->m_groupName, m_pickedData->m_index);
        }

        this->requestRender();
    }
    else if(m_editMode != EditMode::DISPLAY)
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

void landmarks::buttonReleaseEvent(MouseButton _button, Modifier /*_mods*/, int /*_x*/, int /*_y*/)
{
    if(m_pickedData != nullptr)
    {
        m_pickedData->m_node->setScale(s_DEFAULT_SCALE, s_DEFAULT_SCALE, s_DEFAULT_SCALE);

        setCursor(Qt::OpenHandCursor);

        // If we are in edit mode, we show the bin button to remove it.
        if((m_editMode& EditMode::EDIT) == EditMode::EDIT && m_mustShowContextualMenu)
        {
            std::pair<Ogre::Vector2, Ogre::Vector2> screenPos = sight::viz::scene3d::helper::Scene::computeBoundingRect(
                *getLayer()->getDefaultCamera(),
                m_pickedData->m_node
            );
            const auto ratio   = m_contextualMenu->devicePixelRatioF();
            auto interactor    = getLayer()->getRenderService()->getInteractorManager();
            auto qtInteractor  = std::dynamic_pointer_cast<window_interactor>(interactor);
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
            const QString serviceID = QString::fromStdString(get_id().substr(get_id().find_last_of('_') + 1));
            auto* binButton         = m_contextualMenu->findChild<QPushButton*>(serviceID + "/binButton");
            SIGHT_ASSERT("The contextual menu should have the bin button", binButton);
            binButton->disconnect();
            QObject::connect(
                binButton,
                &QPushButton::clicked,
                [this, groupName = m_pickedData->m_groupName, index = m_pickedData->m_index]
                {
                    m_contextualMenu->hide();
                    LandmarksOrImageSeriesLock lfLock = lockLandmarks();
                    if(lfLock.landmarks != nullptr)
                    {
                        lfLock.landmarks->removePoint(groupName, index);
                        lfLock.landmarks->signal<sight::data::landmarks::PointRemovedSignalType>(
                            sight::data::landmarks::POINT_REMOVED_SIG
                        )->async_emit(
                            groupName,
                            index
                        );
                    }
                    else if(lfLock.imageSeries != nullptr)
                    {
                        lfLock.imageSeries->getFiducials()->removePoint(groupName, index);
                        lfLock.imageSeries->signal<data::has_fiducials::signals::PointRemoved>(
                            data::has_fiducials::signals::POINT_REMOVED
                        )->async_emit(groupName, index);
                    }
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
    else if(_button == RIGHT && m_editMode != EditMode::DISPLAY)
    {
        disableEditMode();
        disableMoveMode();
    }
}

//------------------------------------------------------------------------------

void landmarks::buttonDoublePressEvent(MouseButton /*_button*/, Modifier /*_mods*/, int _x, int _y)
{
    m_contextualMenu->hide();

    std::shared_ptr<Landmark> pickedData = tryPick(_x, _y, false);

    if(pickedData != nullptr)
    {
        getLayer()->cancelFurtherInteraction();

        // Check if something is picked to update the position of the distance.
        const auto pickedPos = getNearestPickedPosition(_x, _y);
        if(pickedPos.has_value())
        {
            LandmarksOrImageSeriesConstLock lock                 = constLockLandmarks();
            std::optional<data::landmarks::PointType> maybePoint = getPoint(
                pickedData->m_groupName,
                pickedData->m_index,
                lock
            );
            if(!maybePoint.has_value())
            {
                return;
            }

            data::landmarks::PointType point = *maybePoint;

            // Send signal with world coordinates of the landmarks
            m_send_world_coord->async_emit(
                point[0],
                point[1],
                point[2]
            );
        }
    }
}

//------------------------------------------------------------------------------

void landmarks::wheelEvent(Modifier /*_mods*/, double /*_angleDelta*/, int /*_x*/, int /*_y*/)
{
    m_contextualMenu->hide();
}

//------------------------------------------------------------------------------

void landmarks::keyPressEvent(int _key, Modifier /*_mods*/, int /*_mouseX*/, int /*_mouseY*/)
{
    if(m_editMode != EditMode::DISPLAY && _key == Qt::Key_Escape)
    {
        disableEditMode();
        disableMoveMode();
    }
}

//------------------------------------------------------------------------------

std::shared_ptr<landmarks::Landmark> landmarks::tryPick(int _x, int _y, bool forModification) const
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

        LandmarksOrImageSeriesConstLock lock = constLockLandmarks();

        for(const auto& landmark : m_manualObjects)
        {
            if(landmark->m_object == object)
            {
                try
                {
                    if(auto group = getGroup(landmark->m_groupName, lock);
                       group.has_value() && group->m_visibility
                       && isLandmarkVisible(group->m_points[landmark->m_index], group->m_size)
                       && (!forModification
                           || !m_canOnlyModifyCurrent
                           || landmark->m_groupName == m_currentGroup
                           || m_movableGroups.contains(landmark->m_groupName)))
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

void landmarks::setCursor(QCursor cursor)
{
    auto interactor    = getLayer()->getRenderService()->getInteractorManager();
    auto qtInteractor  = std::dynamic_pointer_cast<window_interactor>(interactor);
    auto* parentWidget = qtInteractor->getQtWidget();
    parentWidget->setCursor(cursor);
}

//------------------------------------------------------------------------------

LandmarksOrImageSeriesLock landmarks::lockLandmarks()
{
    return {.landmarks = m_landmarks.lock(), .imageSeries = m_imageSeries.lock()};
}

//------------------------------------------------------------------------------

LandmarksOrImageSeriesConstLock landmarks::constLockLandmarks() const
{
    return {.landmarks = m_landmarks.const_lock(), .imageSeries = m_imageSeries.const_lock()};
}

//------------------------------------------------------------------------------

ImageOrImageSeriesLock landmarks::lockImage()
{
    return {.image = m_image.lock(), .imageSeries = m_imageSeries.lock()};
}

//------------------------------------------------------------------------------

ImageOrImageSeriesConstLock landmarks::constLockImage() const
{
    return {.image = m_image.const_lock(), .imageSeries = m_imageSeries.const_lock()};
}

landmarks::DeleteContextualMenuWhenFocusOut::DeleteContextualMenuWhenFocusOut(landmarks* sLandmarks) :
    m_sLandmarks(sLandmarks)
{
}

//------------------------------------------------------------------------------

bool landmarks::DeleteContextualMenuWhenFocusOut::eventFilter(QObject* /*o*/, QEvent* e)
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

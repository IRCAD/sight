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

#include "viz/scene3d/material_adaptor.hpp"

#include <core/com/slots.hxx>
#include <core/runtime/path.hpp>
#include <core/tools/uuid.hpp>

#include <ui/__/cursor.hpp>

#include <viz/scene3d/helper/manual_object.hpp>
#include <viz/scene3d/helper/scene.hpp>

#include <modules/viz/scene3d_qt/window_interactor.hpp>

#include <QHBoxLayout>

namespace sight::module::viz::scene3d_qt::adaptor
{

static constexpr Ogre::Real s_SELECTED_SCALE = 1.35F;
static constexpr Ogre::Real s_DEFAULT_SCALE  = 1.F;

namespace
{

//------------------------------------------------------------------------------

[[nodiscard]] std::optional<data::fiducials_series::FiducialSet> get_fiducial_set(
    const std::string& _group_name,
    data::fiducials_series::csptr _fiducials_series
)
{
    SIGHT_ASSERT("Fiducials series mustn't be null", _fiducials_series != nullptr);
    std::optional<std::pair<data::fiducials_series::FiducialSet, std::size_t> > res =
        _fiducials_series->getFiducialSetAndIndex(_group_name);
    return res.has_value() ? std::optional(res->first) : std::optional<data::fiducials_series::FiducialSet>();
}

//------------------------------------------------------------------------------

[[nodiscard]] std::vector<data::fiducials_series::Fiducial*> get_point_fiducials_ptr(
    data::fiducials_series::FiducialSet& _fiducial_set
)
{
    std::vector<data::fiducials_series::Fiducial*> point_fiducials;
    std::ranges::for_each(
        _fiducial_set.fiducialSequence,
        [&point_fiducials](data::fiducials_series::Fiducial& _fiducial)
            {
                if(_fiducial.shapeType == data::fiducials_series::Shape::POINT)
                {
                    point_fiducials.push_back(&_fiducial);
                }
            });
    return point_fiducials;
}

//------------------------------------------------------------------------------

[[nodiscard]] std::vector<std::string> get_group_names(const LandmarksOrImageSeriesConstLock& _lock)
{
    if(_lock.landmarks != nullptr)
    {
        return _lock.landmarks->getGroupNames();
    }

    if(_lock.imageSeries != nullptr)
    {
        return _lock.imageSeries->getFiducials()->getPointFiducialsGroupNames();
    }

    SIGHT_ASSERT("Either 'landmarks' or 'fiducialsSeries' must be configured as inout", false);
    return {};
}

//------------------------------------------------------------------------------

[[nodiscard]] std::optional<std::size_t> get_number_of_points_in_group(
    const std::string& _group_name,
    const LandmarksOrImageSeriesConstLock& _lock
)
{
    if(_lock.landmarks != nullptr)
    {
        return _lock.landmarks->getGroup(_group_name).m_points.size();
    }

    if(_lock.imageSeries != nullptr)
    {
        return _lock.imageSeries->getFiducials()->getNumberOfPointsInGroup(_group_name);
    }

    SIGHT_ASSERT("Either 'landmarks' or 'fiducialsSeries' must be configured as inout", false);
    return {};
}

//------------------------------------------------------------------------------

void set_point(
    data::fiducials_series::FiducialSet& _fiducial_set,
    data::fiducials_series::Fiducial& _fiducial,
    const data::landmarks::point_t& _point,
    data::image_series::sptr _image_series
)
{
    SIGHT_ASSERT("Only point fiducials are supported", _fiducial.shapeType == data::fiducials_series::Shape::POINT);
    if(!_fiducial_set.frameOfReferenceUID.has_value())
    {
        std::string frame_of_reference_uid = _image_series->getStringValue(
            data::dicom::attribute::Keyword::FrameOfReferenceUID
        );
        if(frame_of_reference_uid.empty())
        {
            // Generate a frame of reference UID if the image doesn't have one. It is supposed to be mandatory according
            // to the DICOM standard anyway.
            frame_of_reference_uid = core::tools::UUID::generate();
            _image_series->setStringValue(data::dicom::attribute::Keyword::FrameOfReferenceUID, frame_of_reference_uid);
        }

        _fiducial_set.frameOfReferenceUID = frame_of_reference_uid;
    }

    _fiducial.contourData = {data::fiducials_series::Point3 {.x = _point[0], .y = _point[1], .z = _point[2]}};
    // If both ContourData and GraphicCoordinatesDataSequence are set, they must be synchronized, but I'm too lazy
    // to do that, so I simply get rid of GraphicCoordinatesDataSequence.
    _fiducial.graphicCoordinatesDataSequence = std::nullopt;
}

//------------------------------------------------------------------------------

[[nodiscard]] std::optional<std::array<double, 3> > get_point(
    data::fiducials_series::Fiducial _fiducial,
    data::fiducials_series::FiducialSet _fiducial_set
)
{
    if(_fiducial_set.frameOfReferenceUID.has_value() && !_fiducial.contourData.empty())
    {
        data::fiducials_series::Point3 point = _fiducial.contourData[0];
        return {{point.x, point.y, point.z}};
    }

    // Position with Graphic Coordinates Data Sequence isn't supported
    return std::nullopt;
}

//------------------------------------------------------------------------------

[[nodiscard]] std::optional<std::array<double, 3> > get_point(
    const std::string& _group_name,
    std::size_t _index,
    data::fiducials_series::csptr _fiducials_series
)
{
    std::optional<data::fiducials_series::FiducialSet> fiducial_set = get_fiducial_set(_group_name, _fiducials_series);
    if(!fiducial_set.has_value())
    {
        return std::nullopt;
    }

    std::vector<data::fiducials_series::Fiducial> point_fiducials =
        data::fiducials_series::getPointFiducials(*fiducial_set);
    if(point_fiducials.size() <= _index)
    {
        return std::nullopt;
    }

    return get_point(point_fiducials[_index], *fiducial_set);
}

//------------------------------------------------------------------------------

[[nodiscard]] std::optional<std::array<double, 3> > get_point(
    const std::string& _group_name,
    std::size_t _index,
    const LandmarksOrImageSeriesConstLock& _lock
)
{
    if(_lock.landmarks != nullptr)
    {
        return _lock.landmarks->getPoint(_group_name, _index);
    }

    if(_lock.imageSeries != nullptr)
    {
        return get_point(_group_name, _index, _lock.imageSeries->getFiducials());
    }

    SIGHT_ASSERT("Either 'landmarks' or 'imageSeries' must be configured as inout", false);
    return {};
}

//------------------------------------------------------------------------------

[[nodiscard]] std::optional<data::landmarks::LandmarksGroup> get_group(
    const std::string& _group_name,
    const LandmarksOrImageSeriesConstPtr& _lock
)
{
    if(_lock.landmarks != nullptr)
    {
        if(!_lock.landmarks->hasGroup(_group_name))
        {
            return std::nullopt;
        }

        return _lock.landmarks->getGroup(_group_name);
    }

    if(_lock.imageSeries != nullptr)
    {
        return _lock.imageSeries->getFiducials()->getGroup(_group_name);
    }

    SIGHT_ASSERT("Either 'landmarks' or 'imageSeries' must be configured as inout", false);
    return {};
}

//------------------------------------------------------------------------------

[[nodiscard]] bool has_group(const std::string& _group_name, const LandmarksOrImageSeriesConstLock& _lock)
{
    if(_lock.landmarks != nullptr)
    {
        return _lock.landmarks->hasGroup(_group_name);
    }

    if(_lock.imageSeries != nullptr)
    {
        return get_fiducial_set(_group_name, _lock.imageSeries->getFiducials()).has_value();
    }

    SIGHT_ASSERT("Either 'landmarks' or 'imageSeries' must be configured as inout", false);
    return {};
}

//------------------------------------------------------------------------------

std::array<float, 3> get_current_slice_pos(const data::image& _image)
{
    namespace Helper = sight::data::helper::medical_image;

    const auto axial_index = std::max(
        std::int64_t(0),
        Helper::get_slice_index(_image, Helper::orientation_t::AXIAL).value_or(0)
    );

    const auto frontal_index = std::max(
        std::int64_t(0),
        Helper::get_slice_index(_image, Helper::orientation_t::FRONTAL).value_or(0)
    );

    const auto sagittal_index = std::max(
        std::int64_t(0),
        Helper::get_slice_index(_image, Helper::orientation_t::SAGITTAL).value_or(0)
    );

    const auto& img_spacing = _image.getSpacing();
    const auto& img_origin  = _image.getOrigin();

    return {
        static_cast<float>(sagittal_index) * static_cast<float>(img_spacing[0])
        + static_cast<float>(img_origin[0]),
        static_cast<float>(frontal_index) * static_cast<float>(img_spacing[1])
        + static_cast<float>(img_origin[1]),
        static_cast<float>(axial_index) * static_cast<float>(img_spacing[2])
        + static_cast<float>(img_origin[2])
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
    new_slot(Slots::CHANGE_EDIT_MODE, [this](bool _edit_mode){_edit_mode ? enableEditMode() : disableEditMode();});
    new_slot(Slots::ENABLE_MOVE_MODE, &landmarks::enableMoveMode, this);
    new_slot(Slots::DISABLE_MOVE_MODE, &landmarks::disableMoveMode, this);
    new_slot(
        Slots::TOGGLE_MOVE_MODE,
        [this]
        {
            (m_editMode& EditMode::MOVE) == EditMode::MOVE ? disableMoveMode() : enableMoveMode();
        });
    new_slot(Slots::CHANGE_MOVE_MODE, [this](bool _edit_mode){_edit_mode ? enableMoveMode() : disableMoveMode();});
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

    std::string hexa_mask = config.get<std::string>(s_QUERY_MASK_CONFIG, "");
    if(!hexa_mask.empty())
    {
        SIGHT_ASSERT(
            "Hexadecimal values (" + s_QUERY_MASK_CONFIG + ") should start with '0x'. Given value : " + hexa_mask,
            hexa_mask.length() > 2 && hexa_mask.substr(0, 2) == "0x"
        );
        m_queryMask = static_cast<std::uint32_t>(std::stoul(hexa_mask, nullptr, 16));
    }

    hexa_mask = config.get<std::string>(s_LANDMARKS_FLAGS_CONFIG, "");
    if(!hexa_mask.empty())
    {
        SIGHT_ASSERT(
            "Hexadecimal values (" + s_LANDMARKS_FLAGS_CONFIG + ") should start with '0x'. Given value : " + hexa_mask,
            hexa_mask.length() > 2 && hexa_mask.substr(0, 2) == "0x"
        );
        m_landmarksQueryFlag = static_cast<std::uint32_t>(std::stoul(hexa_mask, nullptr, 16));
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
    const std::string& initial_shape = config.get<std::string>(s_INITIAL_SHAPE, "sphere");
    if(initial_shape == "sphere")
    {
        m_currentShape = sight::data::landmarks::Shape::SPHERE;
    }
    else if(initial_shape == "cube")
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

    auto* root_scene_node = getSceneManager()->getRootSceneNode();
    m_transNode = getOrCreateTransformNode(root_scene_node);

    m_material = std::make_shared<data::material>();
    m_material->setDiffuse(std::make_shared<data::color>(1.F, 1.F, 1.F, 1.F));

    // Register the material adaptor.
    m_materialAdaptor = this->registerService<sight::viz::scene3d::material_adaptor>(
        "sight::module::viz::scene3d::adaptor::material"
    );
    m_materialAdaptor->set_inout(m_material, sight::viz::scene3d::material_adaptor::s_MATERIAL_INOUT, true);
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

    auto interactor     = getLayer()->getRenderService()->getInteractorManager();
    auto qt_interactor  = std::dynamic_pointer_cast<window_interactor>(interactor);
    auto* parent_widget = qt_interactor->getQtWidget();
    m_contextualMenu = new QWidget(parent_widget);
    m_contextualMenu->setStyleSheet(".QWidget {background-color: none;}");
    m_contextualMenu->hide();
    auto* layout = new QHBoxLayout;
    QIcon trash_bin_icon(QString::fromStdString(
                             core::runtime::get_module_resource_file_path(
                                 "sight::module::ui::flaticons/RedTrashBin.svg"
                             ).string()
    ));
    auto* bin_button             = new QPushButton(trash_bin_icon, "");
    const std::string service_id = get_id().substr(get_id().find_last_of('_') + 1);
    bin_button->setObjectName(QString::fromStdString(service_id) + "/binButton");
    bin_button->setCursor(Qt::ArrowCursor);
    bin_button->adjustSize();
    if(bin_button->width() < 40)
    {
        bin_button->setMinimumWidth(40);
    }

    if(bin_button->height() < 40)
    {
        bin_button->setMinimumHeight(40);
    }

    bin_button->adjustSize();
    bin_button->setIconSize(bin_button->size());
    layout->addWidget(bin_button);
    if(m_renamingAllowed)
    {
        QIcon pen_icon(QString::fromStdString(
                           core::runtime::get_module_resource_file_path(
                               "sight::module::ui::flaticons/YellowPen.svg"
                           ).string()
        ));
        auto* pen_button = new QPushButton(pen_icon, "");
        pen_button->setObjectName(QString::fromStdString(service_id) + "/penButton");
        pen_button->setCursor(Qt::ArrowCursor);
        pen_button->adjustSize();
        if(pen_button->width() < 40)
        {
            pen_button->setMinimumWidth(40);
        }

        if(pen_button->height() < 40)
        {
            pen_button->setMinimumHeight(40);
        }

        pen_button->adjustSize();
        pen_button->setIconSize(pen_button->size());
        layout->addWidget(pen_button);
    }

    m_contextualMenu->setLayout(layout);
    m_contextualMenu->adjustSize();
    m_contextualMenu->raise();

    if(auto image_series = m_imageSeries.lock())
    {
        image_series->getFiducials()->setGroupNamesForPointFiducials();
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
    for(const auto& group_name : get_group_names(lock))
    {
        std::optional<std::size_t> nb_of_points_in_group = get_number_of_points_in_group(group_name, lock);
        for(std::size_t index = 0 ; index < nb_of_points_in_group ; ++index)
        {
            this->createManualObject(group_name, index, lock);
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
    for(const auto& selected_landmark : m_selectedLandmarks)
    {
        selected_landmark->m_timer->stop();
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

    auto* scene_mgr = getSceneManager();

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
        scene_mgr->destroyManualObject(landmark->m_object);

        it = m_manualObjects.erase(it);
    }

    // Request the rendering.
    requestRender();
}

//------------------------------------------------------------------------------

void landmarks::removeAll()
{
    m_contextualMenu->hide();

    LandmarksOrImageSeriesLock lf_lock = lockLandmarks();
    if(lf_lock.landmarks != nullptr)
    {
        for(const std::string& grp : lf_lock.landmarks->getGroupNames())
        {
            lf_lock.landmarks->removeGroup(grp);
            lf_lock.landmarks->signal<data::landmarks::group_removed_signal_t>(data::landmarks::GROUP_REMOVED_SIG)->
            async_emit(grp);
        }
    }
    else if(lf_lock.imageSeries != nullptr)
    {
        std::vector<std::string> deleted_group_names;
        std::vector<data::fiducials_series::FiducialSet> fiducial_sets =
            lf_lock.imageSeries->getFiducials()->getFiducialSets();
        for(auto it_fiducial_set = fiducial_sets.begin() ; it_fiducial_set != fiducial_sets.end() ; )
        {
            if(!it_fiducial_set->groupName.has_value())
            {
                ++it_fiducial_set;
                continue;
            }

            bool has_deleted = false;
            for(auto it_fiducial = it_fiducial_set->fiducialSequence.begin() ;
                it_fiducial != it_fiducial_set->fiducialSequence.end() ; )
            {
                if(it_fiducial->shapeType == data::fiducials_series::Shape::POINT)
                {
                    it_fiducial = it_fiducial_set->fiducialSequence.erase(it_fiducial);
                    has_deleted = true;
                }
                else
                {
                    ++it_fiducial;
                }
            }

            if(has_deleted)
            {
                deleted_group_names.push_back(*it_fiducial_set->groupName);
            }

            if(it_fiducial_set->fiducialSequence.empty())
            {
                it_fiducial_set = fiducial_sets.erase(it_fiducial_set);
            }
            else
            {
                ++it_fiducial_set;
            }
        }

        lf_lock.imageSeries->getFiducials()->setFiducialSets(fiducial_sets);
        for(const std::string& group : deleted_group_names)
        {
            lf_lock.imageSeries->signal<data::has_fiducials::signals::GroupRemoved>(
                data::has_fiducials::signals::GROUP_REMOVED
            )->async_emit(group);
        }
    }
}

//------------------------------------------------------------------------------

void landmarks::removeGroup(std::string _group_name)
{
    // Make the context as current.
    getRenderService()->makeCurrent();

    auto* scene_mgr = getSceneManager();

    // Find object where name match _groupName and delete Ogre's resources.
    for(auto object_it = m_manualObjects.begin() ; object_it != m_manualObjects.end() ; )
    {
        const std::string& name = (*object_it)->m_groupName;
        if(name.find(_group_name) != std::string::npos)
        {
            // Stop the thread if it already run since we are deleting data.
            deselectPoint(_group_name, (*object_it)->m_index);

            if(m_enableLabels)
            {
                (*object_it)->m_label->detachFromNode();
                (*object_it)->m_label->setVisible(false);
            }

            m_transNode->removeAndDestroyChild((*object_it)->m_node);
            scene_mgr->destroyManualObject((*object_it)->m_object);

            object_it = m_manualObjects.erase(object_it);
        }
        else
        {
            ++object_it;
        }
    }

    // Request the rendering.
    requestRender();
}

//------------------------------------------------------------------------------

void landmarks::modifyGroup(std::string _group_name)
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
    removeGroup(_group_name);

    // Get landmarks.
    LandmarksOrImageSeriesConstLock lock = constLockLandmarks();

    std::optional<std::size_t> group_size = get_number_of_points_in_group(_group_name, lock);
    if(!group_size.has_value())
    {
        return;
    }

    // Re-create the group.
    for(std::size_t index = 0 ; index < group_size ; ++index)
    {
        createManualObject(_group_name, index, lock);
    }

    // Re-run selected landmark threads
    for(std::size_t index : indexes)
    {
        this->selectPoint(_group_name, index);
    }
}

//------------------------------------------------------------------------------

void landmarks::renameGroup(std::string _old_group_name, std::string _new_group_name)
{
    for(const std::shared_ptr<Landmark>& landmark : m_manualObjects)
    {
        if(landmark->m_groupName == _old_group_name)
        {
            landmark->m_groupName = _new_group_name;
            if(m_enableLabels)
            {
                landmark->m_label->setText(_new_group_name + '_' + std::to_string(landmark->m_index));
            }
        }
    }

    if(m_currentGroup == _old_group_name)
    {
        m_currentGroup = _new_group_name;
    }
}

//------------------------------------------------------------------------------

void landmarks::setCurrentGroup(std::string _new_current_group_name)
{
    m_currentGroup = _new_current_group_name;
}

//------------------------------------------------------------------------------

void landmarks::modifyPoint(std::string _group_name, std::size_t _index)
{
    std::optional<std::array<double, 3> > maybe_point = get_point(_group_name, _index, constLockLandmarks());

    if(!maybe_point.has_value())
    {
        return;
    }

    std::array<double, 3> point = *maybe_point;

    for(auto& m_manual_object : m_manualObjects)
    {
        const auto& name = m_manual_object->m_groupName;
        if(name.find(_group_name) != std::string::npos && m_manual_object->m_index == _index)
        {
            const Ogre::Vector3 position(static_cast<float>(point[0]),
                                         static_cast<float>(point[1]),
                                         static_cast<float>(point[2]));
            m_manual_object->m_node->setPosition(position);
            updateLandmarkVisibility(*m_manual_object, get_group(m_manual_object->m_groupName, constLockLandmarks()));
            break;
        }
    }

    getRenderService()->requestRender();
}

//------------------------------------------------------------------------------

void landmarks::addPoint(std::string _group_name)
{
    // Make the context as current.
    getRenderService()->makeCurrent();

    // Get landmarks.
    LandmarksOrImageSeriesConstLock lock = constLockLandmarks();

    // Get the last index.
    std::optional<std::size_t> number_of_points = get_number_of_points_in_group(_group_name, lock);

    if(!number_of_points.has_value() || number_of_points == 0)
    {
        return;
    }

    // Add the new point.
    this->createManualObject(_group_name, *number_of_points - 1, lock);
}

//------------------------------------------------------------------------------

void landmarks::removePoint(std::string _group_name, std::size_t _index)
{
    // Make the context as current.
    this->getRenderService()->makeCurrent();

    Ogre::SceneManager* scene_mgr = this->getSceneManager();

    // Find object where name match _groupName and the index, and delete Ogre's resources.
    for(auto object_it = m_manualObjects.begin() ; object_it != m_manualObjects.end() ; ++object_it)
    {
        const std::string& name = (*object_it)->m_groupName;
        if(name.find(_group_name) != std::string::npos && (*object_it)->m_index == _index)
        {
            // Stop the thread if it already run since we are deleting data.
            this->deselectPoint(_group_name, _index);

            if(m_enableLabels)
            {
                (*object_it)->m_label->detachFromNode();
                (*object_it)->m_label->setVisible(false);
            }

            m_transNode->removeAndDestroyChild((*object_it)->m_node);
            scene_mgr->destroyManualObject((*object_it)->m_object);
            object_it = m_manualObjects.erase(object_it);
            break;
        }
    }

    // Re-compute index of landmarks in the same group.
    for(auto& m_manual_object : m_manualObjects)
    {
        const std::string& name = m_manual_object->m_groupName;
        if(name.find(_group_name) != std::string::npos && m_manual_object->m_index > _index)
        {
            m_manual_object->m_index -= 1;
            if(m_enableLabels)
            {
                m_manual_object->m_label->setText(_group_name + "_" + std::to_string(m_manual_object->m_index));
            }
        }
    }

    // Request the rendering.
    this->requestRender();
}

//------------------------------------------------------------------------------

void landmarks::insertPoint(std::string _group_name, std::size_t _index)
{
    // Make the context as current
    getRenderService()->makeCurrent();

    createManualObject(_group_name, _index, constLockLandmarks());
}

//------------------------------------------------------------------------------

std::shared_ptr<landmarks::Landmark> landmarks::createManualObject(
    const std::string& _group_name,
    std::size_t _index,
    data::landmarks::LandmarksGroup _group_data,
    data::landmarks::point_t _point_pos
)
{
    // Create the point name.
    const std::string point_name = _group_name + "_" + std::to_string(_index);

    // Create the manual object.
    const Ogre::ColourValue color =
        Ogre::ColourValue(
            _group_data.m_color[0],
            _group_data.m_color[1],
            _group_data.m_color[2],
            _group_data.m_color[3]
        );

    Ogre::SceneManager* scene_mgr = this->getSceneManager();
    Ogre::ManualObject* object    = scene_mgr->createManualObject(this->get_id() + "_" + point_name + "_object");
    switch(_group_data.m_shape)
    {
        case data::landmarks::Shape::SPHERE:
            sight::viz::scene3d::helper::manual_object::createSphere(
                object,
                m_materialAdaptor->getMaterialName(),
                color,
                _group_data.m_size * 0.5F
            );
            break;

        case data::landmarks::Shape::CUBE:
            sight::viz::scene3d::helper::manual_object::createCube(
                object,
                m_materialAdaptor->getMaterialName(),
                color,
                _group_data.m_size
            );
            break;
    }

    object->setQueryFlags(m_landmarksQueryFlag);

    Ogre::SceneNode* node = m_transNode->createChildSceneNode(this->get_id() + "_" + point_name + "_node");

    // Set the point to the right position.
    node->setPosition(Ogre::Real(_point_pos[0]), Ogre::Real(_point_pos[1]), Ogre::Real(_point_pos[2]));

    // Attach data.
    node->attachObject(object);

    // Create the label.
    sight::viz::scene3d::text::sptr text;
    if(m_enableLabels)
    {
        // Create the label.
        text = sight::viz::scene3d::text::make(this->getLayer());
        text->setFontSize(m_fontSize);
        text->setText(point_name);
        text->setTextColor(color);
        text->setVisible(_group_data.m_visibility && m_isVisible);
        text->setTextAlignment("center", "");

        // Attach data.
        text->attachToNode(node, this->getLayer()->getDefaultCamera());
    }

    // Store the created data.
    auto new_landmark = std::make_shared<Landmark>(node, object, _group_name, _index, text);
    m_manualObjects.push_back(new_landmark);

    if(m_enableLabels)
    {
        // The landmark should be in a weak ptr, else there is a circular reference (the slot refers to the landmark via
        // newLandmark, and the landmark refers to the slot via m_slots).
        auto slot_text_edited = core::com::new_slot(
            [this, weak_landmark = std::weak_ptr(new_landmark)](std::string _edited_text)
            {
                auto landmark               = weak_landmark.lock();
                std::string new_group_name  = _edited_text;
                std::string index_as_string = std::to_string(landmark->m_index);
                // Remove the index suffix if present
                if(new_group_name.ends_with('_' + index_as_string))
                {
                    new_group_name = new_group_name.substr(0, new_group_name.size() - (index_as_string.size() + 1));
                }

                LandmarksOrImageSeriesLock lock = lockLandmarks();
                std::string old_group_name      = landmark->m_groupName;
                if(lock.landmarks != nullptr)
                {
                    lock.landmarks->renameGroup(old_group_name, new_group_name);
                    renameGroup(old_group_name, new_group_name);
                    auto signal =
                        lock.landmarks->signal<data::landmarks::group_renamed_signal_t>(
                            data::landmarks::GROUP_RENAMED_SIG
                        );
                    signal->async_emit(
                        old_group_name,
                        new_group_name
                    );
                }
                else if(lock.imageSeries != nullptr)
                {
                    std::optional<std::pair<data::fiducials_series::FiducialSet, std::size_t> > fiducial_set =
                        lock.imageSeries->getFiducials()->getFiducialSetAndIndex(old_group_name);
                    if(!fiducial_set.has_value())
                    {
                        SIGHT_WARN("Couldn't rename group '" << old_group_name << "', it doesn't exist");
                        return;
                    }

                    lock.imageSeries->getFiducials()->setGroupName(fiducial_set->second, new_group_name);
                    renameGroup(old_group_name, new_group_name);
                    lock.imageSeries->signal<data::has_fiducials::signals::GroupRenamed>(
                        data::has_fiducials::signals::GROUP_RENAMED
                    )->async_emit(old_group_name, new_group_name);
                }
            });
        slot_text_edited->set_worker(core::thread::get_default_worker());
        new_landmark->m_label->signal(sight::viz::scene3d::text::TEXT_EDITED_SIGNAL)->connect(slot_text_edited);
        new_landmark->m_slots.push_back(slot_text_edited);

        auto slot_editing_finish = core::com::new_slot(
            [label = std::weak_ptr(new_landmark->m_label)]
            {
                label.lock()->setEditMode(false);
            });
        slot_editing_finish->set_worker(core::thread::get_default_worker());
        new_landmark->m_label->signal(sight::viz::scene3d::text::EDITING_FINISHED_SIGNAL)->connect(slot_editing_finish);
        new_landmark->m_slots.push_back(slot_editing_finish);
    }

    updateLandmarkVisibility(*m_manualObjects.back(), _group_data);

    // Request the rendering.
    this->requestRender();

    return new_landmark;
}

//------------------------------------------------------------------------------

std::shared_ptr<landmarks::Landmark> landmarks::createManualObject(
    const std::string& _group_name,
    std::size_t _index,
    data::landmarks::csptr _landmarks
)
{
    const data::landmarks::LandmarksGroup& group = _landmarks->getGroup(_group_name);
    return createManualObject(_group_name, _index, group, group.m_points[_index]);
}

//------------------------------------------------------------------------------

std::shared_ptr<landmarks::Landmark> landmarks::createManualObject(
    const std::string& _group_name,
    std::size_t _index,
    LandmarksOrImageSeriesConstPtr _lf
)
{
    if(_lf.landmarks != nullptr)
    {
        return createManualObject(_group_name, _index, _lf.landmarks);
    }

    if(_lf.imageSeries != nullptr)
    {
        std::optional<data::landmarks::LandmarksGroup> group = _lf.imageSeries->getFiducials()->getGroup(_group_name);
        if(!group.has_value())
        {
            SIGHT_ASSERT("The group '" << _group_name << "' doesn't exist", false);
            return nullptr;
        }

        if(group->m_points.size() <= _index)
        {
            SIGHT_ASSERT("The point of '" << _group_name << '_' << _index << "' doesn't exist", false);
            return nullptr;
        }

        return createManualObject(_group_name, _index, *group, group->m_points[_index]);
    }

    SIGHT_ASSERT("Either 'landmarks' or 'fiducialsSeries' must be configured as inout", false);
    return nullptr;
}

//------------------------------------------------------------------------------

void landmarks::selectPoint(std::string _group_name, std::size_t _index)
{
    // Make the context as current.
    this->getRenderService()->makeCurrent();

    for(auto& m_manual_object : m_manualObjects)
    {
        const std::string& name = m_manual_object->m_groupName;
        if(name.find(_group_name) != std::string::npos && m_manual_object->m_index == _index)
        {
            const auto it = std::find_if(
                m_selectedLandmarks.begin(),
                m_selectedLandmarks.end(),
                [&](
                    std::shared_ptr<SelectedLandmark> _landmark)
                {
                    return _landmark->m_landmark->m_groupName == _group_name && _landmark->m_landmark->m_index == _index;
                });

            if(it == m_selectedLandmarks.end())
            {
                // This method must be synchronized with deselectPoint(std::string, std::size_t).
                std::lock_guard guard(m_selectedMutex);

                // Create thread data.
                std::shared_ptr<SelectedLandmark> selected_landmark =
                    std::make_shared<SelectedLandmark>(this->worker()->create_timer(), m_manual_object);
                m_selectedLandmarks.push_back(selected_landmark);

                // Run a thread that change the selected point.
                core::thread::timer::time_duration_t duration = std::chrono::milliseconds(500);
                selected_landmark->m_timer->set_function(
                    [this, selected_landmark](auto&& ...)
                    {
                        hightlight(selected_landmark);
                    });
                selected_landmark->m_timer->set_duration(duration);
                selected_landmark->m_timer->start();
            }

            break;
        }
    }
}

//------------------------------------------------------------------------------

void landmarks::deselectPoint(std::string _group_name, std::size_t _index)
{
    // Make the context as current.
    getRenderService()->makeCurrent();

    // This method must be synchronized with selectPoint(std::string, std::size_t).
    std::lock_guard guard(m_selectedMutex);

    // Find the thread and stop it.
    for(auto landmark_it = m_selectedLandmarks.begin() ; landmark_it != m_selectedLandmarks.end() ; ++landmark_it)
    {
        if((*landmark_it)->m_landmark->m_groupName == _group_name && (*landmark_it)->m_landmark->m_index == _index)
        {
            // Stop the timer.
            (*landmark_it)->m_timer->stop();
            (*landmark_it)->m_landmark->m_object->setVisible(true);
            updateLandmarkVisibility((*landmark_it)->m_landmark);

            // Request the rendering.
            requestRender();

            m_selectedLandmarks.erase(landmark_it);
            break;
        }
    }
}

//------------------------------------------------------------------------------

void landmarks::hightlight(std::shared_ptr<SelectedLandmark> _selected_landmark)
{
    // Make the context as current.
    this->getRenderService()->makeCurrent();

    // Hightlight the selected landmark.
    this->updateLandmarkVisibility(_selected_landmark->m_landmark);
    if(_selected_landmark->m_landmark->m_object->isVisible())
    {
        if(_selected_landmark->m_show)
        {
            _selected_landmark->m_landmark->m_object->setVisible(true);
        }
        else
        {
            _selected_landmark->m_landmark->m_object->setVisible(false);
        }

        this->requestRender();
    }

    _selected_landmark->m_show = !_selected_landmark->m_show;

    // Request the rendering.
    this->requestRender();
}

//------------------------------------------------------------------------------

void landmarks::changeSliceType(int _from, int _to)
{
    // Make the context as current.
    this->getRenderService()->makeCurrent();

    const auto to_orientation   = static_cast<OrientationMode>(_to);
    const auto from_orientation = static_cast<OrientationMode>(_from);

    const auto new_orientation = m_orientation == to_orientation ? from_orientation
                                                                 : m_orientation
                                 == from_orientation ? to_orientation : m_orientation;

    m_orientation = new_orientation;

    this->updateLandmarksVisibility();

    // Request the rendering.
    this->requestRender();
}

//------------------------------------------------------------------------------

void landmarks::changeSliceIndex(int /*_axialIndex*/, int /*_frontalIndex*/, int /*_sagittalIndex*/)
{
    // Make the context as current.
    this->getRenderService()->makeCurrent();

    bool image_exists = false;
    {
        ImageOrImageSeriesConstLock lock = constLockImage();

        if(lock.image || lock.imageSeries)
        {
            image_exists = true;

            this->getRenderService()->makeCurrent();
        }
    }

    if(image_exists)
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

        slice_index = sight::data::helper::medical_image::get_slice_index(
            *image,
            m_orientation
        );
    }

    if(!slice_index)
    {
        // No slice selected, early return
        return;
    }

    LandmarksOrImageSeriesLock fl_lock = lockLandmarks();

    if(fl_lock.landmarks == nullptr && fl_lock.imageSeries == nullptr)
    {
        // No need to continue if there is no landmarks
        return;
    }

    bool has_deleted = false;

    if(fl_lock.landmarks != nullptr)
    {
        for(const auto& name : fl_lock.landmarks->getGroupNames())
        {
            auto& group = fl_lock.landmarks->getGroup(name);

            for(auto it = group.m_points.begin() ; it < group.m_points.end() ; )
            {
                if(isLandmarkVisible(*it, group.m_size))
                {
                    it          = group.m_points.erase(it);
                    has_deleted = true;
                }
                else
                {
                    ++it;
                }
            }
        }
    }
    else if(fl_lock.imageSeries != nullptr)
    {
        data::fiducials_series::sptr fiducials_series = fl_lock.imageSeries.get_shared()->getFiducials();
        for(const std::string& name : fiducials_series->getPointFiducialsGroupNames())
        {
            std::optional<std::pair<data::fiducials_series::FiducialSet, std::size_t> > fiducial_set =
                fiducials_series->getFiducialSetAndIndex(name);
            if(!fiducial_set.has_value())
            {
                continue;
            }

            std::optional<data::landmarks::size_t> size = fiducial_set->first.size;
            if(!size.has_value())
            {
                size                     = m_currentSize;
                fiducial_set->first.size = size;
            }

            for(auto it = fiducial_set->first.fiducialSequence.begin() ;
                it != fiducial_set->first.fiducialSequence.end() ; )
            {
                std::optional<std::array<double, 3> > point = get_point(*it, fiducial_set->first);
                if(!point.has_value())
                {
                    continue;
                }

                if(isLandmarkVisible(*point, *size))
                {
                    it          = fiducial_set->first.fiducialSequence.erase(it);
                    has_deleted = true;
                }
            }

            fiducials_series->setFiducialSet(fiducial_set->second, fiducial_set->first);
        }
    }

    if(has_deleted)
    {
        data::object::sptr object;
        if(fl_lock.landmarks != nullptr)
        {
            object = fl_lock.landmarks.get_shared();
        }
        else if(fl_lock.imageSeries != nullptr)
        {
            object = fl_lock.imageSeries->getFiducials();
        }

        object->signal<sight::data::object::modified_signal_t>(sight::data::object::MODIFIED_SIG)->async_emit();
    }
}

//------------------------------------------------------------------------------

void landmarks::configureLandmarks(sight::viz::scene3d::landmarks_configuration _configuration)
{
    if(_configuration.group)
    {
        m_currentGroup = *_configuration.group;
    }

    if(_configuration.color)
    {
        m_currentColor = *_configuration.color;
    }

    if(_configuration.size)
    {
        m_currentSize = *_configuration.size;
    }

    if(_configuration.shape)
    {
        m_currentShape = *_configuration.shape;
    }

    if(_configuration.groupMax)
    {
        if(*_configuration.groupMax >= 0)
        {
            m_groupMax[m_currentGroup] = std::size_t(*_configuration.groupMax);
        }
        else
        {
            m_groupMax.erase(m_currentGroup);
        }
    }

    if(_configuration.visibleMax)
    {
        if(*_configuration.visibleMax >= 0)
        {
            m_visibleMax = *_configuration.visibleMax;
        }
        else
        {
            m_visibleMax = std::nullopt;
        }
    }

    if(_configuration.totalMax)
    {
        if(*_configuration.totalMax >= 0)
        {
            m_totalMax = *_configuration.totalMax;
        }
        else
        {
            m_totalMax = std::nullopt;
        }
    }

    if(_configuration.movableGroups)
    {
        m_movableGroups = *_configuration.movableGroups;
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
        auto interactor     = getLayer()->getRenderService()->getInteractorManager();
        auto qt_interactor  = std::dynamic_pointer_cast<window_interactor>(interactor);
        auto* parent_widget = qt_interactor->getQtWidget();
        m_eventFilter = std::make_unique<DeleteContextualMenuWhenFocusOut>(this);
        parent_widget->installEventFilter(m_eventFilter.get());
    }

    M_EDIT_MODE_CHANGED->async_emit(true);
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
        auto interactor     = getLayer()->getRenderService()->getInteractorManager();
        auto qt_interactor  = std::dynamic_pointer_cast<window_interactor>(interactor);
        auto* parent_widget = qt_interactor->getQtWidget();
        parent_widget->unsetCursor();
    }

    m_contextualMenu->hide();

    if(m_enableLabels && m_renamingAllowed)
    {
        std::ranges::for_each(m_manualObjects, [](std::shared_ptr<Landmark> _l){_l->m_label->setEditMode(false);});
    }

    M_EDIT_MODE_CHANGED->async_emit(false);
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
        auto interactor     = getLayer()->getRenderService()->getInteractorManager();
        auto qt_interactor  = std::dynamic_pointer_cast<window_interactor>(interactor);
        auto* parent_widget = qt_interactor->getQtWidget();
        parent_widget->unsetCursor();
    }
}

//------------------------------------------------------------------------------

void landmarks::createLandmark(sight::data::landmarks::point_t _point)
{
    createAndPickLandmark(_point, false);
}

//------------------------------------------------------------------------------

void landmarks::createAndPickLandmark(const sight::data::landmarks::point_t& _point, bool _pick)
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
        LandmarksOrImageSeriesLock lf_lock = lockLandmarks();

        if(lf_lock.landmarks != nullptr)
        {
            // If the group does not exist, we create it.
            if(!lf_lock.landmarks->hasGroup(m_currentGroup))
            {
                lf_lock.landmarks->addGroup(m_currentGroup, m_currentColor, m_currentSize, m_currentShape);
                const auto& sig = lf_lock.landmarks->signal<sight::data::landmarks::group_added_signal_t>(
                    sight::data::landmarks::GROUP_ADDED_SIG
                );

                sig->async_emit(m_currentGroup);
            }

            lf_lock.landmarks->addPoint(m_currentGroup, _point);

            // Get the last index.
            group = lf_lock.landmarks->getGroup(m_currentGroup);
            index = group.m_points.size() - 1;

            // Block the signal to avoid being called back.
            const auto& sig = lf_lock.landmarks->signal<sight::data::landmarks::point_added_signal_t>(
                sight::data::landmarks::POINT_ADDED_SIG
            );

            sight::core::com::connection::blocker blocker(sig->get_connection(slot(Slots::ADD_POINT)));
            sig->async_emit(m_currentGroup);
        }
        else if(lf_lock.imageSeries != nullptr)
        {
            std::optional<std::pair<data::fiducials_series::FiducialSet, std::size_t> > res =
                lf_lock.imageSeries->getFiducials()->getFiducialSetAndIndex(m_currentGroup);
            data::fiducials_series::FiducialSet fiducial_set;
            std::size_t index_of_group {};
            // If the group does not exist, we create it.
            if(!res.has_value())
            {
                fiducial_set.groupName = m_currentGroup;
                fiducial_set.color     = m_currentColor;
                fiducial_set.size      = m_currentSize;
                switch(m_currentShape)
                {
                    case data::landmarks::Shape::SPHERE:
                        fiducial_set.shape = data::fiducials_series::PrivateShape::SPHERE;
                        break;

                    case data::landmarks::Shape::CUBE:
                        fiducial_set.shape = data::fiducials_series::PrivateShape::CUBE;
                }

                index_of_group = lf_lock.imageSeries->getFiducials()->getFiducialSets().size();
                lf_lock.imageSeries->signal<data::has_fiducials::signals::GroupAdded>(
                    data::has_fiducials::signals::GROUP_ADDED
                )->async_emit(m_currentGroup);
            }
            else
            {
                std::tie(fiducial_set, index_of_group) = *res;
            }

            data::fiducials_series::Fiducial fiducial;
            fiducial.shapeType = data::fiducials_series::Shape::POINT;
            index              = data::fiducials_series::getPointFiducials(fiducial_set).size();
            std::string fiducial_name = m_currentGroup + '_' + std::to_string(index);
            fiducial.fiducialDescription = fiducial_name; // TODO: Add a more appropriate description?
            fiducial.fiducialIdentifier  = fiducial_name;
            fiducial.fiducialUID         = core::tools::UUID::generate();
            set_point(fiducial_set, fiducial, _point, lf_lock.imageSeries.get_shared());
            fiducial_set.fiducialSequence.push_back(fiducial);
            lf_lock.imageSeries->getFiducials()->setFiducialSet(index_of_group, fiducial_set);

            group = *get_group(m_currentGroup, lf_lock);

            // Block the signal to avoid being called back.
            const auto& sig =
                lf_lock.imageSeries->signal<data::has_fiducials::signals::PointAdded>(
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
    std::shared_ptr<Landmark> new_landmark = createManualObject(m_currentGroup, index, group, _point);

    if(_pick)
    {
        m_pickedData = new_landmark;
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
           || (has_group(m_currentGroup, lock)
               && get_number_of_points_in_group(m_currentGroup, lock) >= m_groupMax[m_currentGroup])))
    {
        return true;
    }

    if(m_totalMax || m_visibleMax)
    {
        const auto& names = get_group_names(lock);

        std::size_t max         = 0;
        std::size_t max_visible = 0;

        for(const auto& name : names)
        {
            std::optional<data::landmarks::LandmarksGroup> maybe_group = get_group(name, lock);
            if(!maybe_group.has_value())
            {
                continue;
            }

            data::landmarks::LandmarksGroup group = *maybe_group;

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
                        ++max_visible;

                        if(max_visible >= *m_visibleMax)
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

    bool image_exists = false;
    {
        ImageOrImageSeriesConstLock iis_lock = constLockImage();
        image_exists = iis_lock.image || iis_lock.imageSeries;
    }

    // Hide landmarks only if there is an image.
    if(image_exists)
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

void landmarks::updateLandmarkVisibility(Landmark& _landmark, std::optional<data::landmarks::LandmarksGroup> _group)
{
    const bool must_show =
        [&]
        {
            // Retrieve group
            if(!_group.has_value())
            {
                return false;
            }

            if(!_group->m_visibility)
            {
                return false;
            }

            const auto& position = _landmark.m_node->getPosition();
            return isLandmarkVisible({position[0], position[1], position[2]}, _group->m_size);
        }();

    // Show or hide the landmark.
    _landmark.m_object->setVisible(must_show);
}

//------------------------------------------------------------------------------

void landmarks::updateLandmarkVisibility(Landmark& _landmark, const LandmarksOrImageSeriesConstLock& _lock)
{
    updateLandmarkVisibility(_landmark, get_group(_landmark.m_groupName, _lock));
}

//------------------------------------------------------------------------------

bool landmarks::isLandmarkVisible(
    const data::landmarks::point_t& _point,
    data::landmarks::size_t _group_size
) const
{
    if(const auto& iis_lock = constLockImage(); iis_lock.image || iis_lock.imageSeries)
    {
        const auto position       = _point[m_orientation];
        data::image::csptr image  = iis_lock.image ? iis_lock.image.get_shared() : iis_lock.imageSeries.get_shared();
        const auto slice_position = get_current_slice_pos(*image)[m_orientation];
        const auto spacing        = image->getSpacing()[m_orientation];

        switch(m_viewDistance)
        {
            case ViewDistance::SLICES_IN_RANGE:
            {
                // Check if the position is the same than slice position
                const auto group_half_size = _group_size * 0.5;
                const auto max_size        = std::max(group_half_size, spacing);

                return core::tools::is_greater(position, (slice_position - _group_size))
                       && core::tools::is_less(position, (slice_position + max_size));
            }

            case ViewDistance::CURRENT_SLICE:
            {
                // Check if the position is the same than slice position
                const auto rounded_position       = std::round(position / spacing);
                const auto rounded_slice_position = std::round(slice_position / spacing);
                return core::tools::is_equal(rounded_position, rounded_slice_position);
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
        std::optional<data::landmarks::LandmarksGroup> maybe_group = get_group(landmark->m_groupName, lock);
        if(!maybe_group.has_value())
        {
            continue;
        }

        data::landmarks::LandmarksGroup group = *maybe_group;
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
    const auto result      = sight::viz::scene3d::utils::pickObject(_x, _y, m_queryMask, *sm);

    if(result.has_value())
    {
        const auto* const camera = sm->getCamera(sight::viz::scene3d::layer::s_DEFAULT_CAMERA_NAME);
        const auto* const vp     = camera->getViewport();

        // Screen to viewport space conversion.
        const float vp_x = static_cast<float>(_x - vp->getActualLeft()) / static_cast<float>(vp->getActualWidth());
        const float vp_y = static_cast<float>(_y - vp->getActualTop()) / static_cast<float>(vp->getActualHeight());

        const Ogre::Ray ray = camera->getCameraToViewportRay(vp_x, vp_y);

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

    const bool must_edit = (m_editMode& EditMode::EDIT) == EditMode::EDIT;

    // Try to pick a landmark.
    if(const auto& picked_data = tryPick(_x, _y); picked_data)
    {
        // If something is picked, we will select it.
        setCursor(Qt::ClosedHandCursor);
        m_pickedData = picked_data;
        m_pickedData->m_node->setScale(s_SELECTED_SCALE, s_SELECTED_SCALE, s_SELECTED_SCALE);

        // Only show contextual menu if we are in "true" EDIT mode
        m_mustShowContextualMenu = must_edit;
    }
    else if(must_edit)
    {
        // Nothing is picked, we will create a new landmark if we are in EDIT mode.
        m_mustShowContextualMenu = false;

        // If nothing is picked, we will create a new landmark.
        if(auto new_pos = this->getNearestPickedPosition(_x, _y); new_pos)
        {
            setCursor(Qt::ClosedHandCursor);
            createAndPickLandmark({(*new_pos)[0], (*new_pos)[1], (*new_pos)[2]});
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

        bool move_in_camera_plane = true;

        Ogre::Vector3 new_pos;
        if(cam->getProjectionType() == Ogre::ProjectionType::PT_PERSPECTIVE)
        {
            // If something is picked, we will snap the landmark to it
            std::optional<Ogre::Vector3> picked_pos = this->getNearestPickedPosition(_x, _y);
            if(picked_pos.has_value())
            {
                new_pos              = picked_pos.value();
                move_in_camera_plane = false;
            }
        }

        // Else we move the distance along a plane.
        if(move_in_camera_plane)
        {
            const auto* const vp = cam->getViewport();

            const float vp_x = static_cast<float>(_x - vp->getActualLeft()) / static_cast<float>(vp->getActualWidth());
            const float vp_y = static_cast<float>(_y - vp->getActualTop()) / static_cast<float>(vp->getActualHeight());

            const Ogre::Ray ray           = cam->getCameraToViewportRay(vp_x, vp_y);
            const Ogre::Vector3 direction = this->getCamDirection(cam);

            const Ogre::Vector3 position = m_pickedData->m_node->getPosition();

            const Ogre::Plane plane(direction, position);

            const std::pair<bool, Ogre::Real> hit = Ogre::Math::intersects(ray, plane);

            if(!hit.first)
            {
                SIGHT_ERROR("The ray must intersect the plane")
                return;
            }

            new_pos = ray.getPoint(hit.second);
        }

        // Reset the query flag.
        m_pickedData->m_object->setQueryFlags(m_landmarksQueryFlag);

        // Update the data, the autoconnection will call modifyPoint.
        LandmarksOrImageSeriesLock lf_lock = lockLandmarks();
        if(lf_lock.landmarks != nullptr)
        {
            data::landmarks::point_t& point = lf_lock.landmarks->getPoint(
                m_pickedData->m_groupName,
                m_pickedData->m_index
            );
            point[0] = new_pos[0];
            point[1] = new_pos[1];
            point[2] = new_pos[2];

            const auto& sig = lf_lock.landmarks->signal<data::landmarks::point_modified_sig_t>(
                data::landmarks::POINT_MODIFIED_SIG
            );
            sig->async_emit(m_pickedData->m_groupName, m_pickedData->m_index);
        }
        else if(lf_lock.imageSeries != nullptr)
        {
            std::optional<std::pair<data::fiducials_series::FiducialSet, std::size_t> > fiducial_set =
                lf_lock.imageSeries->getFiducials()->getFiducialSetAndIndex(m_pickedData->m_groupName);
            if(!fiducial_set.has_value())
            {
                return;
            }

            std::vector<data::fiducials_series::Fiducial*> point_fiducials =
                get_point_fiducials_ptr(fiducial_set->first);
            if(m_pickedData->m_index >= point_fiducials.size())
            {
                return;
            }

            data::fiducials_series::Fiducial& fiducial = *point_fiducials[m_pickedData->m_index];
            set_point(
                fiducial_set->first,
                fiducial,
                {new_pos.x, new_pos.y, new_pos.z},
                lf_lock.imageSeries.get_shared()
            );
            lf_lock.imageSeries->getFiducials()->setFiducialSet(fiducial_set->second, fiducial_set->first);

            lf_lock.imageSeries->signal<data::has_fiducials::signals::PointModified>(
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
            std::pair<Ogre::Vector2,
                      Ogre::Vector2> screen_pos = sight::viz::scene3d::helper::scene::computeBoundingRect(
                *getLayer()->getDefaultCamera(),
                m_pickedData->m_node
                      );
            const auto ratio    = m_contextualMenu->devicePixelRatioF();
            auto interactor     = getLayer()->getRenderService()->getInteractorManager();
            auto qt_interactor  = std::dynamic_pointer_cast<window_interactor>(interactor);
            auto* parent_widget = qt_interactor->getQtWidget();
            const int x         = std::clamp(
                int(((screen_pos.first.x + screen_pos.second.x) / 2) / ratio),
                0,
                parent_widget->width() - m_contextualMenu->width()
            );

            int y = int((screen_pos.first.y / ratio) - m_contextualMenu->height());
            if(y < 0)
            {
                // If there isn't enough place upward the landmark, place the menu downward.
                y = int(screen_pos.second.y / ratio);
            }

            m_contextualMenu->move(x, y);
            m_contextualMenu->raise();
            const QString service_id = QString::fromStdString(get_id().substr(get_id().find_last_of('_') + 1));
            auto* bin_button         = m_contextualMenu->findChild<QPushButton*>(service_id + "/binButton");
            SIGHT_ASSERT("The contextual menu should have the bin button", bin_button);
            bin_button->disconnect();
            QObject::connect(
                bin_button,
                &QPushButton::clicked,
                [this, group_name = m_pickedData->m_groupName, index = m_pickedData->m_index]
                {
                    m_contextualMenu->hide();
                    LandmarksOrImageSeriesLock lf_lock = lockLandmarks();
                    if(lf_lock.landmarks != nullptr)
                    {
                        lf_lock.landmarks->removePoint(group_name, index);
                        lf_lock.landmarks->signal<sight::data::landmarks::point_removed_signal_t>(
                            sight::data::landmarks::POINT_REMOVED_SIG
                        )->async_emit(
                            group_name,
                            index
                        );
                    }
                    else if(lf_lock.imageSeries != nullptr)
                    {
                        lf_lock.imageSeries->getFiducials()->removePoint(group_name, index);
                        lf_lock.imageSeries->signal<data::has_fiducials::signals::PointRemoved>(
                            data::has_fiducials::signals::POINT_REMOVED
                        )->async_emit(group_name, index);
                    }
                });
            if(m_renamingAllowed)
            {
                auto* pen_button = m_contextualMenu->findChild<QPushButton*>(service_id + "/penButton");
                SIGHT_ASSERT("The contextual menu should have the pen button", pen_button);
                pen_button->disconnect();
                QObject::connect(
                    pen_button,
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

    std::shared_ptr<Landmark> picked_data = tryPick(_x, _y, false);

    if(picked_data != nullptr)
    {
        getLayer()->cancelFurtherInteraction();

        // Check if something is picked to update the position of the distance.
        const auto picked_pos = getNearestPickedPosition(_x, _y);
        if(picked_pos.has_value())
        {
            LandmarksOrImageSeriesConstLock lock                = constLockLandmarks();
            std::optional<data::landmarks::point_t> maybe_point = get_point(
                picked_data->m_groupName,
                picked_data->m_index,
                lock
            );
            if(!maybe_point.has_value())
            {
                return;
            }

            data::landmarks::point_t point = *maybe_point;

            // Send signal with world coordinates of the landmarks
            M_SEND_WORLD_COORD->async_emit(
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

std::shared_ptr<landmarks::Landmark> landmarks::tryPick(int _x, int _y, bool _for_modification) const
{
    const auto layer = this->getLayer();

    Ogre::SceneManager* const scene_mgr = layer->getSceneManager();

    const Ogre::Camera* const cam = layer->getDefaultCamera();
    const auto* const vp          = cam->getViewport();

    const float vp_x = static_cast<float>(_x - vp->getActualLeft()) / static_cast<float>(vp->getActualWidth());
    const float vp_y = static_cast<float>(_y - vp->getActualTop()) / static_cast<float>(vp->getActualHeight());

    const Ogre::Ray ray = cam->getCameraToViewportRay(vp_x, vp_y);

    const auto ray_scene_query = std::unique_ptr<Ogre::RaySceneQuery>(
        scene_mgr->createRayQuery(ray, m_landmarksQueryFlag)
    );

    ray_scene_query->setSortByDistance(true);
    const auto& query_result = ray_scene_query->execute();

    if(query_result.empty())
    {
        // Nothing found.
        return nullptr;
    }

    // Find the ogre landmark and apply a scale.
    bool found = false;

    // The plane is always intercepted, if a landmark is found
    for(std::size_t index = 0 ; !found && index < query_result.size() ; ++index)
    {
        // The landmark should be the nearest object found.
        const auto* const object = query_result[index].movable;

        LandmarksOrImageSeriesConstLock lock = constLockLandmarks();

        for(const auto& landmark : m_manualObjects)
        {
            if(landmark->m_object == object)
            {
                try
                {
                    if(auto group = get_group(landmark->m_groupName, lock);
                       group.has_value() && group->m_visibility
                       && isLandmarkVisible(group->m_points[landmark->m_index], group->m_size)
                       && (!_for_modification
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

void landmarks::setCursor(QCursor _cursor)
{
    auto interactor     = getLayer()->getRenderService()->getInteractorManager();
    auto qt_interactor  = std::dynamic_pointer_cast<window_interactor>(interactor);
    auto* parent_widget = qt_interactor->getQtWidget();
    parent_widget->setCursor(_cursor);
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

landmarks::DeleteContextualMenuWhenFocusOut::DeleteContextualMenuWhenFocusOut(landmarks* _s_landmarks) :
    m_sLandmarks(_s_landmarks)
{
}

//------------------------------------------------------------------------------

bool landmarks::DeleteContextualMenuWhenFocusOut::eventFilter(QObject* /*o*/, QEvent* _e)
{
    if(m_sLandmarks->m_contextualMenu != nullptr
       && !m_sLandmarks->m_contextualMenu->findChild<QPushButton*>()->hasFocus()
       && (_e->type() == QEvent::FocusOut || _e->type() == QEvent::Resize))
    {
        m_sLandmarks->m_contextualMenu->hide();
    }

    return false;
}

} // namespace sight::module::viz::scene3d_qt::adaptor.

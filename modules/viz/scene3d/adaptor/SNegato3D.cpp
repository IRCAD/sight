/************************************************************************
 *
 * Copyright (C) 2014-2022 IRCAD France
 * Copyright (C) 2014-2020 IHU Strasbourg
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

#include "modules/viz/scene3d/adaptor/SNegato3D.hpp"

#include <core/com/Signal.hxx>
#include <core/com/Slots.hxx>

#include <data/Boolean.hpp>
#include <data/helper/MedicalImage.hpp>
#include <data/Image.hpp>
#include <data/Integer.hpp>
#include <data/tools/Color.hpp>

#include <service/macros.hpp>

#include <viz/scene3d/compositor/Core.hpp>
#include <viz/scene3d/ogre.hpp>
#include <viz/scene3d/picker/IPicker.hpp>
#include <viz/scene3d/Utils.hpp>

#include <OGRE/OgreCamera.h>
#include <OGRE/OgreSceneNode.h>
#include <OGRE/OgreVector.h>

#include <algorithm>

namespace sight::module::viz::scene3d::adaptor
{

static const core::com::Slots::SlotKeyType s_NEWIMAGE_SLOT            = "newImage";
static const core::com::Slots::SlotKeyType s_SLICETYPE_SLOT           = "sliceType";
static const core::com::Slots::SlotKeyType s_SLICEINDEX_SLOT          = "sliceIndex";
static const core::com::Slots::SlotKeyType s_UPDATE_SLICES_FROM_WORLD = "updateSlicesFromWorld";
static const core::com::Slots::SlotKeyType s_SET_TRANSPARENCY_SLOT    = "setTransparency";
static const core::com::Slots::SlotKeyType s_UPDATE_TF_SLOT           = "updateTF";

static const core::com::Signals::SignalKeyType s_PICKED_VOXEL_SIG = "pickedVoxel";

static const std::string s_AUTORESET_CAMERA_CONFIG = "autoresetcamera";
static const std::string s_TRANSFORM_CONFIG        = "transform";
static const std::string s_FILTERING_CONFIG        = "filtering";
static const std::string s_TF_ALPHA_CONFIG         = "tfAlpha";
static const std::string s_INTERACTIVE_CONFIG      = "interactive";
static const std::string s_PRIORITY_CONFIG         = "priority";
static const std::string s_QUERY_CONFIG            = "queryFlags";
static const std::string s_BORDER_CONFIG           = "border";

static const std::string s_VISIBILITY_FIELD = "VISIBILITY";

static constexpr std::uint8_t s_NEGATO_WIDGET_RQ_GROUP_ID =
    sight::viz::scene3d::compositor::Core::s_SURFACE_RQ_GROUP_ID
    - 1;

//------------------------------------------------------------------------------

SNegato3D::SNegato3D() noexcept
{
    newSlot(s_NEWIMAGE_SLOT, &SNegato3D::newImage, this);
    newSlot(s_SLICETYPE_SLOT, &SNegato3D::changeSliceType, this);
    newSlot(s_SLICEINDEX_SLOT, &SNegato3D::changeSliceIndex, this);
    newSlot(s_SET_TRANSPARENCY_SLOT, &SNegato3D::setTransparency, this);
    newSlot(s_UPDATE_SLICES_FROM_WORLD, &SNegato3D::updateSlicesFromWorld, this);
    newSlot(s_UPDATE_TF_SLOT, &SNegato3D::updateTF, this);

    m_pickedVoxelSignal = newSignal<PickedVoxelSigType>(s_PICKED_VOXEL_SIG);
}

//------------------------------------------------------------------------------

SNegato3D::~SNegato3D() noexcept
{
}

//------------------------------------------------------------------------------

void SNegato3D::configuring()
{
    this->configureParams();

    const ConfigType configType = this->getConfigTree();
    const ConfigType config     = configType.get_child("config.<xmlattr>");

    m_autoResetCamera = config.get<bool>(s_AUTORESET_CAMERA_CONFIG, true);

    if(config.count(s_FILTERING_CONFIG))
    {
        const std::string filteringValue = config.get<std::string>(s_FILTERING_CONFIG);
        sight::viz::scene3d::Plane::FilteringEnumType filtering(sight::viz::scene3d::Plane::FilteringEnumType::LINEAR);

        if(filteringValue == "none")
        {
            filtering = sight::viz::scene3d::Plane::FilteringEnumType::NONE;
        }
        else if(filteringValue == "anisotropic")
        {
            filtering = sight::viz::scene3d::Plane::FilteringEnumType::ANISOTROPIC;
        }

        m_filtering = filtering;
    }

    const std::string hexaMask = config.get<std::string>(s_QUERY_CONFIG, "");
    if(!hexaMask.empty())
    {
        SIGHT_ASSERT(
            "Hexadecimal values should start with '0x'"
            "Given value : " + hexaMask,
            hexaMask.length() > 2
            && hexaMask.substr(0, 2) == "0x"
        );
        m_queryFlags = static_cast<std::uint32_t>(std::stoul(hexaMask, nullptr, 16));
    }

    m_enableAlpha = config.get<bool>(s_TF_ALPHA_CONFIG, m_enableAlpha);
    m_interactive = config.get<bool>(s_INTERACTIVE_CONFIG, m_interactive);
    m_priority    = config.get<int>(s_PRIORITY_CONFIG, m_priority);
    m_border      = config.get<bool>(s_BORDER_CONFIG, m_border);

    const std::string transformId =
        config.get<std::string>(sight::viz::scene3d::ITransformable::s_TRANSFORM_CONFIG, this->getID() + "_transform");
    this->setTransformId(transformId);
}

//------------------------------------------------------------------------------

void SNegato3D::starting()
{
    this->initialize();

    this->getRenderService()->makeCurrent();
    {
        // 3D source texture instantiation
        const auto image = m_image.lock();
        m_3DOgreTexture = std::make_shared<sight::viz::scene3d::Texture>(image.get_shared());

        // TF texture initialization
        const auto tf = m_tf.lock();
        m_gpuTF = std::make_unique<sight::viz::scene3d::TransferFunction>(tf.get_shared());
    }

    // Scene node's instantiation
    Ogre::SceneNode* const rootSceneNode = this->getSceneManager()->getRootSceneNode();
    Ogre::SceneNode* const transformNode = this->getOrCreateTransformNode(rootSceneNode);
    m_negatoSceneNode = transformNode->createChildSceneNode();

    // Instantiation of the planes
    int orientationNum {0};
    for(auto& plane : m_planes)
    {
        auto imgOrientation = static_cast<OrientationMode>(orientationNum++);
        plane = std::make_shared<sight::viz::scene3d::Plane>(
            this->getID(),
            m_negatoSceneNode,
            this->getSceneManager(),
            imgOrientation,
            m_3DOgreTexture->get(),
            m_filtering,
            m_border
        );
    }

    if(m_autoResetCamera)
    {
        this->getRenderService()->resetCameraCoordinates(m_layerID);
    }

    this->newImage();

    if(m_interactive)
    {
        auto interactor = std::dynamic_pointer_cast<sight::viz::scene3d::interactor::IInteractor>(this->getSptr());
        this->getLayer()->addInteractor(interactor, m_priority);

        m_pickingCross = this->getSceneManager()->createManualObject(this->getID() + "_PickingCross");
        const auto basicAmbientMat = Ogre::MaterialManager::getSingleton().getByName(
            "BasicAmbient",
            sight::viz::scene3d::RESOURCE_GROUP
        );
        auto crossMat = basicAmbientMat->clone(this->getID() + "_CrossMaterial");
        crossMat->setAmbient(Ogre::ColourValue::Red);
        crossMat->setDiffuse(Ogre::ColourValue::Red);
        crossMat->setDepthCheckEnabled(false);
        m_pickingCross->estimateVertexCount(4);
        m_pickingCross->begin(crossMat, Ogre::RenderOperation::OT_LINE_LIST);
        for(std::uint8_t i = 0 ; i < 4 ; ++i)
        {
            m_pickingCross->position(Ogre::Vector3::ZERO);
        }

        m_pickingCross->end();
        m_pickingCross->setVisible(false);
        // Always render the widget after the plane it is displayed on.
        m_pickingCross->setRenderQueueGroupAndPriority(s_NEGATO_WIDGET_RQ_GROUP_ID, 1);
        m_negatoSceneNode->attachObject(m_pickingCross);
    }

    // Set the visibility of the 3D Negato
    this->setVisible(m_isVisible);
}

//-----------------------------------------------------------------------------

service::IService::KeyConnectionsMap SNegato3D::getAutoConnections() const
{
    return {
        {s_IMAGE_IN, data::Image::s_MODIFIED_SIG, s_NEWIMAGE_SLOT},
        {s_IMAGE_IN, data::Image::s_BUFFER_MODIFIED_SIG, s_NEWIMAGE_SLOT},
        {s_IMAGE_IN, data::Image::s_SLICE_TYPE_MODIFIED_SIG, s_SLICETYPE_SLOT},
        {s_IMAGE_IN, data::Image::s_SLICE_INDEX_MODIFIED_SIG, s_SLICEINDEX_SLOT},
        {s_IMAGE_IN, data::Image::s_VISIBILITY_MODIFIED_SIG, s_UPDATE_VISIBILITY_SLOT},
        {s_TF_INOUT, data::TransferFunction::s_MODIFIED_SIG, s_UPDATE_TF_SLOT},
        {s_TF_INOUT, data::TransferFunction::s_POINTS_MODIFIED_SIG, s_UPDATE_TF_SLOT},
        {s_TF_INOUT, data::TransferFunction::s_WINDOWING_MODIFIED_SIG, s_UPDATE_TF_SLOT}
    };
}

//------------------------------------------------------------------------------

void SNegato3D::updating()
{
    this->requestRender();
}

//------------------------------------------------------------------------------

void SNegato3D::stopping()
{
    this->getRenderService()->makeCurrent();

    if(m_interactive)
    {
        auto interactor = std::dynamic_pointer_cast<sight::viz::scene3d::interactor::IInteractor>(this->getSptr());
        this->getLayer()->removeInteractor(interactor);
    }

    m_pickedPlane.reset();

    for(auto& plane : m_planes)
    {
        plane.reset();
    }

    m_negatoSceneNode->removeAndDestroyAllChildren();
    this->getSceneManager()->destroySceneNode(m_negatoSceneNode);

    if(m_pickingCross)
    {
        auto crossMtl = m_pickingCross->getSection(0)->getMaterial();
        Ogre::MaterialManager::getSingleton().remove(crossMtl);

        this->getSceneManager()->destroyManualObject(m_pickingCross);
    }

    m_3DOgreTexture.reset();
    m_gpuTF.reset();

    this->requestRender();
}

//------------------------------------------------------------------------------

void SNegato3D::createPlanes(const Ogre::Vector3& _spacing, const Ogre::Vector3& _origin)
{
    // Fits the planes to the new texture
    for(const auto& plane : m_planes)
    {
        plane->setVoxelSpacing(_spacing);
        plane->setOriginPosition(_origin);
        plane->initializePlane();
        plane->enableAlpha(m_enableAlpha);
        plane->setQueryFlags(m_queryFlags);
    }
}

//------------------------------------------------------------------------------

void SNegato3D::newImage()
{
    this->getRenderService()->makeCurrent();

    int axialIdx    = 0;
    int frontalIdx  = 0;
    int sagittalIdx = 0;
    {
        const auto image = m_image.lock();

        if(!data::helper::MedicalImage::checkImageValidity(image.get_shared()))
        {
            return;
        }

        // Retrieves or creates the slice index fields
        m_3DOgreTexture->update();

        const auto [spacing, origin] = sight::viz::scene3d::Utils::convertSpacingAndOrigin(image.get_shared());
        this->createPlanes(spacing, origin);

        // Update Slice
        namespace imHelper = data::helper::MedicalImage;

        axialIdx    = std::max(0, int(imHelper::getSliceIndex(*image, imHelper::orientation_t::AXIAL).value_or(0)));
        frontalIdx  = std::max(0, int(imHelper::getSliceIndex(*image, imHelper::orientation_t::FRONTAL).value_or(0)));
        sagittalIdx = std::max(0, int(imHelper::getSliceIndex(*image, imHelper::orientation_t::SAGITTAL).value_or(0)));
    }

    this->changeSliceIndex(axialIdx, frontalIdx, sagittalIdx);

    // Update transfer function in Gpu programs
    this->updateTF();

    if(m_autoResetCamera)
    {
        this->getRenderService()->resetCameraCoordinates(m_layerID);
    }

    this->requestRender();
}

//------------------------------------------------------------------------------

void SNegato3D::changeSliceType(int, int)
{
    this->getRenderService()->makeCurrent();

    this->updateTF();

    this->requestRender();
}

//------------------------------------------------------------------------------

void SNegato3D::changeSliceIndex(int _axialIndex, int _frontalIndex, int _sagittalIndex)
{
    const auto image = m_image.lock();

    auto imgSize = image->getSize();

    // Sometimes, the image can contain only one slice,
    // it results into a division by 0 when the range is transformed between [0-1].
    // So we increase the image size to 2 to divide by 1.
    imgSize[0] = imgSize[0] == 1 ? 2 : imgSize[0];
    imgSize[1] = imgSize[1] == 1 ? 2 : imgSize[1];
    imgSize[2] = imgSize[2] == 1 ? 2 : imgSize[2];

    const Ogre::Vector3 sliceIndices = {
        static_cast<float>(_sagittalIndex) / (static_cast<float>(imgSize[0] - 1)),
        static_cast<float>(_frontalIndex) / (static_cast<float>(imgSize[1] - 1)),
        static_cast<float>(_axialIndex) / (static_cast<float>(imgSize[2] - 1))
    };

    for(std::uint8_t i = 0 ; i < 3 ; ++i)
    {
        m_planes[i]->changeSlice(sliceIndices[i]);
    }

    this->requestRender();
}

//-----------------------------------------------------------------------------

void SNegato3D::updateTF()
{
    this->getRenderService()->makeCurrent();
    m_gpuTF->update();

    for(const auto& plane : m_planes)
    {
        // Sends the TF texture to the negato-related passes
        plane->setTFData(*m_gpuTF.get());
    }

    this->requestRender();
}

//-----------------------------------------------------------------------------

void SNegato3D::setTransparency(double _transparency)
{
    const auto notNull =
        std::bind(std::not_equal_to<sight::viz::scene3d::Plane::sptr>(), std::placeholders::_1, nullptr);

    if(std::all_of(m_planes.begin(), m_planes.end(), notNull))
    {
        const auto image = m_image.lock();

        const float opacity = 1.f - static_cast<float>(_transparency);

        for(const auto& plane : m_planes)
        {
            plane->setEntityOpacity(opacity);
        }

        this->requestRender();
    }
}

//------------------------------------------------------------------------------

void SNegato3D::setVisible(bool _visible)
{
    // True if the visibility need to be propagated
    bool visibilityChanged = false;

    using VisModSigType = data::Image::VisibilityModifiedSignalType;
    std::shared_ptr<VisModSigType> visModSig;

    {
        const auto image      = m_image.lock();
        const auto visibility = image->getField<data::Boolean>(s_VISIBILITY_FIELD);

        // We propagate the visibility change if it has never been applied or if have changed
        visibilityChanged = !visibility || visibility->getValue() != _visible;

        if(visibilityChanged)
        {
            //image->setField(s_VISIBILITY_FIELD, data::Boolean::New(_visible));
            visModSig = image->signal<VisModSigType>(data::Image::s_VISIBILITY_MODIFIED_SIG);
        }
    }

    if(visibilityChanged)
    {
        for(const auto& plane : m_planes)
        {
            plane->setVisible(m_isVisible);
        }

        const core::com::Connection::Blocker blocker(visModSig->getConnection(this->slot(s_UPDATE_VISIBILITY_SLOT)));
        visModSig->asyncEmit(_visible);

        this->requestRender();
    }
}

//------------------------------------------------------------------------------

void SNegato3D::setPlanesQueryFlags(std::uint32_t _flags)
{
    for(const auto& p : m_planes)
    {
        p->setQueryFlags(_flags);
    }
}

//------------------------------------------------------------------------------

void SNegato3D::mouseMoveEvent(MouseButton _button, Modifier, int _x, int _y, int, int)
{
    if(m_pickedPlane)
    {
        if(_button == MouseButton::MIDDLE)
        {
            this->moveSlices(_x, _y);
        }
        else if(_button == MouseButton::RIGHT)
        {
            const double dx = static_cast<double>(_x - m_initialPos[0]);
            const double dy = static_cast<double>(m_initialPos[1] - _y);

            this->updateWindowing(dx, dy);
        }
        else if(_button == MouseButton::LEFT)
        {
            this->pickIntensity(_x, _y);
        }

        this->getLayer()->cancelFurtherInteraction();
    }
}

//------------------------------------------------------------------------------

void SNegato3D::buttonPressEvent(MouseButton _button, Modifier, int _x, int _y)
{
    m_pickedPlane.reset();
    m_pickingCross->setVisible(false);

    if(_button == MouseButton::MIDDLE)
    {
        this->moveSlices(_x, _y);
    }
    else if(_button == MouseButton::RIGHT)
    {
        if(this->getPickedSlices(_x, _y) != std::nullopt)
        {
            const auto tf = m_tf.const_lock();

            m_initialLevel  = tf->level();
            m_initialWindow = tf->window();

            m_initialPos = {_x, _y};
        }
    }
    else if(_button == MouseButton::LEFT)
    {
        this->pickIntensity(_x, _y);
    }

    if(m_pickedPlane)
    {
        this->getLayer()->cancelFurtherInteraction();
    }
}

//------------------------------------------------------------------------------

void SNegato3D::buttonReleaseEvent(MouseButton, Modifier, int, int)
{
    if(m_pickedPlane)
    {
        m_pickedPlane->setRenderQueuerGroupAndPriority(sight::viz::scene3d::compositor::Core::s_SURFACE_RQ_GROUP_ID, 0);
        m_pickedPlane.reset();
    }

    m_pickingCross->setVisible(false);
    m_pickedVoxelSignal->asyncEmit("");
    this->setPlanesQueryFlags(m_queryFlags); // Make all planes pickable again.
}

//------------------------------------------------------------------------------

void SNegato3D::moveSlices(int _x, int _y)
{
    const auto pickRes = this->getPickedSlices(_x, _y);

    if(pickRes.has_value())
    {
        const auto image = m_image.lock();

        auto pickedPt = pickRes.value();

        for(const auto& p : m_planes)
        {
            if(p != m_pickedPlane)
            {
                p->setQueryFlags(0x0);
            }
        }

        const auto [spacing, origin] = sight::viz::scene3d::Utils::convertSpacingAndOrigin(image.get_shared());
        pickedPt                     = (pickedPt - origin) / spacing;

        const Ogre::Vector3i pickedPtI(pickedPt);
        const auto sig = image->signal<data::Image::SliceIndexModifiedSignalType>
                             (data::Image::s_SLICE_INDEX_MODIFIED_SIG);
        sig->asyncEmit(pickedPtI[2], pickedPtI[1], pickedPtI[0]);
    }
}

//------------------------------------------------------------------------------

void SNegato3D::updateSlicesFromWorld(double _x, double _y, double _z)
{
    const auto image = m_image.lock();

    Ogre::Vector3 point = {static_cast<float>(_x), static_cast<float>(_y), static_cast<float>(_z)};
    Ogre::Vector3i slice_idx;
    try
    {
        slice_idx = sight::viz::scene3d::Utils::worldToSlices(*image, point);
    }
    catch(core::Exception& _e)
    {
        SIGHT_WARN("Cannot update slice index: " << _e.what());
        return;
    }

    const auto sig = image->signal<data::Image::SliceIndexModifiedSignalType>
                         (data::Image::s_SLICE_INDEX_MODIFIED_SIG);

    sig->asyncEmit(slice_idx[2], slice_idx[1], slice_idx[0]);
}

//------------------------------------------------------------------------------

void SNegato3D::pickIntensity(int _x, int _y)
{
    if(m_pickedVoxelSignal->numConnections() > 0)
    {
        const auto pickedPos = this->getPickedSlices(_x, _y);

        if(pickedPos.has_value())
        {
            const auto image = m_image.lock();

            if(!data::helper::MedicalImage::checkImageValidity(image.get_shared()))
            {
                return;
            }

            const auto imageBufferLock = image->dump_lock();

            const auto [spacing, origin] = sight::viz::scene3d::Utils::convertSpacingAndOrigin(image.get_shared());
            const auto pickedPosImageSpace = (pickedPos.value() - origin) / spacing;

            this->updatePickingCross(pickedPos.value(), origin);

            const auto& imgSize = image->getSize();
            data::Image::Size pickedVoxel;
            for(std::uint8_t i = 0 ; i < pickedVoxel.size() ; ++i)
            {
                pickedVoxel[i] = std::clamp(
                    static_cast<std::size_t>(pickedPosImageSpace[i]),
                    std::size_t {0},
                    imgSize[i] - 1
                );
            }

            const auto intensity   = image->getPixelAsString(pickedVoxel[0], pickedVoxel[1], pickedVoxel[2]);
            const auto pickingText = "(" + std::to_string(pickedVoxel[0]) + ", " + std::to_string(pickedVoxel[1])
                                     + ", " + std::to_string(pickedVoxel[2]) + "): " + intensity;

            m_pickedVoxelSignal->asyncEmit(pickingText);

            // Render the picked plane before the widget.
            m_pickedPlane->setRenderQueuerGroupAndPriority(s_NEGATO_WIDGET_RQ_GROUP_ID, 0);
        }
    }
}

//------------------------------------------------------------------------------

std::optional<Ogre::Vector3> SNegato3D::getPickedSlices(int _x, int _y)
{
    auto* const sceneManager = this->getSceneManager();
    SIGHT_ASSERT("Scene manager not created yet.", sceneManager);

    sight::viz::scene3d::picker::IPicker picker;
    picker.setSceneManager(sceneManager);
    picker.executeRaySceneQuery(_x, _y, m_queryFlags);

    const auto isPicked = [&picker](const sight::viz::scene3d::Plane::sptr& _p)
                          {
                              return (_p->getMovableObject() != nullptr)
                                     && (_p->getMovableObject() == picker.getSelectedObject());
                          };

    auto it = std::find_if(m_planes.cbegin(), m_planes.cend(), isPicked);

    if(it != m_planes.cend())
    {
        m_pickedPlane = *it;
        return picker.getIntersectionInWorldSpace();
    }

    return std::nullopt;
}

//------------------------------------------------------------------------------

void SNegato3D::updatePickingCross(const Ogre::Vector3& _pickedPos, const Ogre::Vector3& _imgOrigin)
{
    const float h = m_pickedPlane->getHeight();
    const float w = m_pickedPlane->getWidth();

    m_pickingCross->beginUpdate(0);
    switch(m_pickedPlane->getOrientationMode())
    {
        case sight::viz::scene3d::Plane::OrientationMode::X_AXIS:
            m_pickingCross->position(_pickedPos.x, 0 + _imgOrigin.y, _pickedPos.z);
            m_pickingCross->position(_pickedPos.x, h + _imgOrigin.y, _pickedPos.z);
            m_pickingCross->position(_pickedPos.x, _pickedPos.y, 0 + _imgOrigin.z);
            m_pickingCross->position(_pickedPos.x, _pickedPos.y, w + _imgOrigin.z);
            break;

        case sight::viz::scene3d::Plane::OrientationMode::Y_AXIS:
            m_pickingCross->position(0 + _imgOrigin.x, _pickedPos.y, _pickedPos.z);
            m_pickingCross->position(w + _imgOrigin.x, _pickedPos.y, _pickedPos.z);
            m_pickingCross->position(_pickedPos.x, _pickedPos.y, 0 + _imgOrigin.z);
            m_pickingCross->position(_pickedPos.x, _pickedPos.y, h + _imgOrigin.z);
            break;

        case sight::viz::scene3d::Plane::OrientationMode::Z_AXIS:
            m_pickingCross->position(0 + _imgOrigin.x, _pickedPos.y, _pickedPos.z);
            m_pickingCross->position(w + _imgOrigin.x, _pickedPos.y, _pickedPos.z);
            m_pickingCross->position(_pickedPos.x, 0 + _imgOrigin.y, _pickedPos.z);
            m_pickingCross->position(_pickedPos.x, h + _imgOrigin.y, _pickedPos.z);
            break;
    }

    m_pickingCross->end();
    m_pickingCross->setVisible(true);
}

//------------------------------------------------------------------------------

void SNegato3D::updateWindowing(double _dw, double _dl)
{
    const double newWindow = m_initialWindow + _dw;
    const double newLevel  = m_initialLevel - _dl;

    {
        const auto tf = m_tf.lock();

        tf->setWindow(newWindow);
        tf->setLevel(newLevel);
        const auto sig = tf->signal<data::TransferFunction::WindowingModifiedSignalType>(
            data::TransferFunction::s_WINDOWING_MODIFIED_SIG
        );
        {
            sig->asyncEmit(newWindow, newLevel);
        }
    }
}

} // namespace sight::module::viz::scene3d::adaptor.

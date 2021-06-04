/************************************************************************
 *
 * Copyright (C) 2014-2021 IRCAD France
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
#include <core/com/Slot.hxx>
#include <core/com/Slots.hxx>

#include <data/Boolean.hpp>
#include <data/fieldHelper/Image.hpp>
#include <data/fieldHelper/MedicalImageHelpers.hpp>
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
#include <OGRE/OgreTextureManager.h>
#include <OGRE/OgreVector.h>

#include <algorithm>

namespace sight::module::viz::scene3d::adaptor
{

static const core::com::Slots::SlotKeyType s_NEWIMAGE_SLOT       = "newImage";
static const core::com::Slots::SlotKeyType s_SLICETYPE_SLOT      = "sliceType";
static const core::com::Slots::SlotKeyType s_SLICEINDEX_SLOT     = "sliceIndex";
static const core::com::Slots::SlotKeyType s_UPDATE_OPACITY_SLOT = "updateOpacity";

static const core::com::Signals::SignalKeyType s_PICKED_VOXEL_SIG = "pickedVoxel";

static const std::string s_IMAGE_INOUT = "image";
static const std::string s_TF_INOUT    = "tf";

static const std::string s_AUTORESET_CAMERA_CONFIG = "autoresetcamera";
static const std::string s_TRANSFORM_CONFIG        = "transform";
static const std::string s_FILTERING_CONFIG        = "filtering";
static const std::string s_TF_ALPHA_CONFIG         = "tfAlpha";
static const std::string s_INTERACTIVE_CONFIG      = "interactive";
static const std::string s_PRIORITY_CONFIG         = "priority";
static const std::string s_QUERY_CONFIG            = "queryFlags";
static const std::string s_BORDER_CONFIG           = "border";

static const std::string s_TRANSPARENCY_FIELD = "TRANSPARENCY";
static const std::string s_VISIBILITY_FIELD   = "VISIBILITY";

static constexpr std::uint8_t s_NEGATO_WIDGET_RQ_GROUP_ID =
    sight::viz::scene3d::compositor::Core::s_SURFACE_RQ_GROUP_ID
    - 1;

//------------------------------------------------------------------------------

SNegato3D::SNegato3D() noexcept :
    m_helperTF(std::bind(&SNegato3D::updateTF, this))
{
    newSlot(s_NEWIMAGE_SLOT, &SNegato3D::newImage, this);
    newSlot(s_SLICETYPE_SLOT, &SNegato3D::changeSliceType, this);
    newSlot(s_SLICEINDEX_SLOT, &SNegato3D::changeSliceIndex, this);
    newSlot(s_UPDATE_OPACITY_SLOT, &SNegato3D::setPlanesOpacity, this);

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

    m_autoResetCamera = config.get<std::string>(s_AUTORESET_CAMERA_CONFIG, "yes") == "yes";

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
        const auto image = this->getLockedInOut<data::Image>(s_IMAGE_INOUT);

        const auto tfW = this->getWeakInOut<data::TransferFunction>(s_TF_INOUT);
        const auto tf  = tfW.lock();
        m_helperTF.setOrCreateTF(tf.get_shared(), image.get_shared());
    }

    // TF texture initialization
    m_gpuTF = std::make_unique<sight::viz::scene3d::TransferFunction>();
    m_gpuTF->createTexture(this->getID());

    // 3D source texture instantiation
    m_3DOgreTexture = ::Ogre::TextureManager::getSingleton().create(
        this->getID() + "_Negato3DTexture",
        sight::viz::scene3d::RESOURCE_GROUP,
        true
    );

    // Scene node's instantiation
    ::Ogre::SceneNode* const rootSceneNode = this->getSceneManager()->getRootSceneNode();
    ::Ogre::SceneNode* const transformNode = this->getTransformNode(rootSceneNode);
    m_negatoSceneNode = transformNode->createChildSceneNode();

    // Instanciation of the planes
    int orientationNum {0};
    for(auto& plane : m_planes)
    {
        auto imgOrientation = static_cast<OrientationMode>(orientationNum++);
        plane = std::make_shared<sight::viz::scene3d::Plane>(
            this->getID(),
            m_negatoSceneNode,
            this->getSceneManager(),
            imgOrientation,
            m_3DOgreTexture,
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
        const auto basicAmbientMat = ::Ogre::MaterialManager::getSingleton().getByName(
            "BasicAmbient",
            sight::viz::scene3d::RESOURCE_GROUP
        );
        auto crossMat = basicAmbientMat->clone(this->getID() + "_CrossMaterial");
        crossMat->setAmbient(::Ogre::ColourValue::Red);
        crossMat->setDiffuse(::Ogre::ColourValue::Red);
        crossMat->setDepthCheckEnabled(false);
        m_pickingCross->estimateVertexCount(4);
        m_pickingCross->begin(crossMat, ::Ogre::RenderOperation::OT_LINE_LIST);
        for(std::uint8_t i = 0 ; i < 4 ; ++i)
        {
            m_pickingCross->position(::Ogre::Vector3::ZERO);
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
    service::IService::KeyConnectionsMap connections;
    connections.push(s_IMAGE_INOUT, data::Image::s_MODIFIED_SIG, s_NEWIMAGE_SLOT);
    connections.push(s_IMAGE_INOUT, data::Image::s_BUFFER_MODIFIED_SIG, s_NEWIMAGE_SLOT);
    connections.push(s_IMAGE_INOUT, data::Image::s_SLICE_TYPE_MODIFIED_SIG, s_SLICETYPE_SLOT);
    connections.push(s_IMAGE_INOUT, data::Image::s_SLICE_INDEX_MODIFIED_SIG, s_SLICEINDEX_SLOT);
    connections.push(s_IMAGE_INOUT, data::Image::s_VISIBILITY_MODIFIED_SIG, s_UPDATE_VISIBILITY_SLOT);
    connections.push(s_IMAGE_INOUT, data::Image::s_TRANSPARENCY_MODIFIED_SIG, s_UPDATE_OPACITY_SLOT);
    return connections;
}

//------------------------------------------------------------------------------

void SNegato3D::updating()
{
    this->requestRender();
}

//------------------------------------------------------------------------------

void SNegato3D::swapping(const KeyType& key)
{
    if(key == s_TF_INOUT)
    {
        const auto image = this->getLockedInOut<data::Image>(s_IMAGE_INOUT);

        const auto tfW = this->getWeakInOut<data::TransferFunction>(s_TF_INOUT);
        const auto tf  = tfW.lock();
        m_helperTF.setOrCreateTF(tf.get_shared(), image.get_shared());

        this->updateTF();
    }
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

    m_helperTF.removeTFConnections();

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
        ::Ogre::MaterialManager::getSingleton().remove(crossMtl);

        this->getSceneManager()->destroyManualObject(m_pickingCross);
    }

    ::Ogre::TextureManager::getSingleton().remove(m_3DOgreTexture);

    m_3DOgreTexture.reset();
    m_gpuTF.reset();

    this->requestRender();
}

//------------------------------------------------------------------------------

void SNegato3D::createPlanes(const ::Ogre::Vector3& _spacing, const ::Ogre::Vector3& _origin)
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
        const auto image = this->getLockedInOut<data::Image>(s_IMAGE_INOUT);

        const auto tfW = this->getWeakInOut<data::TransferFunction>(s_TF_INOUT);
        const auto tf  = tfW.lock();
        m_helperTF.setOrCreateTF(tf.get_shared(), image.get_shared());

        if(!data::fieldHelper::MedicalImageHelpers::checkImageValidity(image.get_shared()))
        {
            return;
        }

        sight::viz::scene3d::Utils::convertImageForNegato(m_3DOgreTexture.get(), image.get_shared());

        const auto [spacing, origin] = sight::viz::scene3d::Utils::convertSpacingAndOrigin(image.get_shared());
        this->createPlanes(spacing, origin);

        // Update Slice
        const auto imgSize       = image->getSize2();
        const auto axialIdxField = image->getField<data::Integer>(
            data::fieldHelper::Image::m_axialSliceIndexId
        );
        SIGHT_INFO_IF("Axial Idx field missing", !axialIdxField);
        axialIdx = axialIdxField
                   ? static_cast<int>(axialIdxField->getValue()) : static_cast<int>(imgSize[2] / 2);

        const auto frontalIdxField = image->getField<data::Integer>(
            data::fieldHelper::Image::m_frontalSliceIndexId
        );
        SIGHT_INFO_IF("Frontal Idx field missing", !frontalIdxField);
        frontalIdx = frontalIdxField
                     ? static_cast<int>(frontalIdxField->getValue()) : static_cast<int>(imgSize[1] / 2);

        const auto sagittalIdxField = image->getField<data::Integer>(
            data::fieldHelper::Image::m_sagittalSliceIndexId
        );
        SIGHT_INFO_IF("Sagittal Idx field missing", !sagittalIdxField);
        sagittalIdx = sagittalIdxField
                      ? static_cast<int>(sagittalIdxField->getValue()) : static_cast<int>(imgSize[0] / 2);
    }

    this->changeSliceIndex(axialIdx, frontalIdx, sagittalIdx);

    // Update tranfer function in Gpu programs
    this->updateTF();

    if(m_autoResetCamera)
    {
        this->getRenderService()->resetCameraCoordinates(m_layerID);
    }

    this->setPlanesOpacity();

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
    const auto image = this->getLockedInOut<data::Image>(s_IMAGE_INOUT);

    auto imgSize = image->getSize2();

    // Sometimes, the image can contain only one slice,
    // it results into a division by 0 when the range is transformed between [0-1].
    // So we increase the image size to 2 to divide by 1.
    imgSize[0] = imgSize[0] == 1 ? 2 : imgSize[0];
    imgSize[1] = imgSize[1] == 1 ? 2 : imgSize[1];
    imgSize[2] = imgSize[2] == 1 ? 2 : imgSize[2];

    const ::Ogre::Vector3 sliceIndices = {
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
    const data::TransferFunction::csptr tf = m_helperTF.getTransferFunction();
    {
        const data::mt::locked_ptr lock(tf);
        m_gpuTF->updateTexture(tf);

        for(const auto& plane : m_planes)
        {
            plane->switchThresholding(tf->getIsClamped());

            // Sends the TF texture to the negato-related passes
            plane->setTFData(*m_gpuTF.get());
        }
    }

    this->requestRender();
}

//-----------------------------------------------------------------------------

void SNegato3D::setPlanesOpacity()
{
    const auto notNull =
        std::bind(std::not_equal_to<sight::viz::scene3d::Plane::sptr>(), std::placeholders::_1, nullptr);

    if(std::all_of(m_planes.begin(), m_planes.end(), notNull))
    {
        const auto image = this->getLockedInOut<data::Image>(s_IMAGE_INOUT);

        const auto transparency = image->setDefaultField(s_TRANSPARENCY_FIELD, data::Integer::New(0));
        const auto isVisible    = image->setDefaultField(s_VISIBILITY_FIELD, data::Boolean::New(true));

        const bool visible  = isVisible->getValue();
        const float opacity = (100.f - static_cast<float>(transparency->getValue())) / 100.f;

        for(const auto& plane : m_planes)
        {
            plane->setVisible(visible);
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
        const auto image      = this->getLockedInOut<data::Image>(s_IMAGE_INOUT);
        const auto visibility = image->getField<data::Boolean>(s_VISIBILITY_FIELD);

        // We propagate the visibility change if it has never been applied or if have changed
        visibilityChanged = !visibility || visibility->getValue() != _visible;

        if(visibilityChanged)
        {
            image->setField(s_VISIBILITY_FIELD, data::Boolean::New(_visible));
            visModSig = image->signal<VisModSigType>(data::Image::s_VISIBILITY_MODIFIED_SIG);
        }
    }

    if(visibilityChanged)
    {
        const core::com::Connection::Blocker blocker(visModSig->getConnection(this->slot(s_UPDATE_VISIBILITY_SLOT)));
        this->setPlanesOpacity();
        visModSig->asyncEmit(_visible);
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
            const data::TransferFunction::sptr tf = m_helperTF.getTransferFunction();
            const data::mt::locked_ptr lock(tf);

            m_initialLevel  = tf->getLevel();
            m_initialWindow = tf->getWindow();

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
        const auto image = this->getLockedInOut<data::Image>(s_IMAGE_INOUT);

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

        const ::Ogre::Vector3i pickedPtI(pickedPt);
        const auto sig = image->signal<data::Image::SliceIndexModifiedSignalType>
                             (data::Image::s_SLICE_INDEX_MODIFIED_SIG);
        sig->asyncEmit(pickedPtI[2], pickedPtI[1], pickedPtI[0]);
    }
}

//------------------------------------------------------------------------------

void SNegato3D::pickIntensity(int _x, int _y)
{
    if(m_pickedVoxelSignal->getNumberOfConnections() > 0)
    {
        const auto pickedPos = this->getPickedSlices(_x, _y);

        if(pickedPos.has_value())
        {
            const auto image = this->getLockedInOut<data::Image>(s_IMAGE_INOUT);

            if(!data::fieldHelper::MedicalImageHelpers::checkImageValidity(image.get_shared()))
            {
                return;
            }

            const auto imageBufferLock = image->lock();

            const auto [spacing, origin] = sight::viz::scene3d::Utils::convertSpacingAndOrigin(image.get_shared());
            const auto pickedPosImageSpace = (pickedPos.value() - origin) / spacing;

            this->updatePickingCross(pickedPos.value(), origin);

            const auto& imgSize = image->getSize2();
            data::Image::Size pickedVoxel;
            for(std::uint8_t i = 0 ; i < pickedVoxel.size() ; ++i)
            {
                pickedVoxel[i] = std::clamp(static_cast<size_t>(pickedPosImageSpace[i]), size_t {0}, imgSize[i] - 1);
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

std::optional< ::Ogre::Vector3> SNegato3D::getPickedSlices(int _x, int _y)
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

void SNegato3D::updatePickingCross(const ::Ogre::Vector3& _pickedPos, const ::Ogre::Vector3& _imgOrigin)
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

    const data::TransferFunction::sptr tf = m_helperTF.getTransferFunction();
    {
        const data::mt::locked_ptr lock(tf);

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

/************************************************************************
 *
 * Copyright (C) 2014-2019 IRCAD France
 * Copyright (C) 2014-2019 IHU Strasbourg
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

#include "visuOgreAdaptor/SNegato3D.hpp"

#include <fwCom/Signal.hxx>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hxx>

#include <fwData/Boolean.hpp>
#include <fwData/Image.hpp>
#include <fwData/Integer.hpp>
#include <fwData/mt/ObjectReadLock.hpp>
#include <fwData/mt/ObjectWriteLock.hpp>

#include <fwDataTools/fieldHelper/Image.hpp>
#include <fwDataTools/fieldHelper/MedicalImageHelpers.hpp>

#include <fwRenderOgre/picker/IPicker.hpp>
#include <fwRenderOgre/Utils.hpp>

#include <fwServices/macros.hpp>

#include <OGRE/OgreCamera.h>
#include <OGRE/OgreSceneNode.h>
#include <OGRE/OgreTextureManager.h>

#include <algorithm>

namespace visuOgreAdaptor
{

fwServicesRegisterMacro( ::fwRenderOgre::IAdaptor, ::visuOgreAdaptor::SNegato3D, ::fwData::Image)

const ::fwCom::Slots::SlotKeyType SNegato3D::s_NEWIMAGE_SLOT = "newImage";
const ::fwCom::Slots::SlotKeyType SNegato3D::s_SLICETYPE_SLOT         = "sliceType";
const ::fwCom::Slots::SlotKeyType SNegato3D::s_SLICEINDEX_SLOT        = "sliceIndex";
const ::fwCom::Slots::SlotKeyType SNegato3D::s_UPDATE_OPACITY_SLOT    = "updateOpacity";
const ::fwCom::Slots::SlotKeyType SNegato3D::s_UPDATE_VISIBILITY_SLOT = "updateVisibility";
const ::fwCom::Slots::SlotKeyType SNegato3D::s_SET_VISIBILITY_SLOT    = "setVisibility";

static const ::fwCom::Signals::SignalKeyType s_PICKED_VOXEL_SIG = "pickedVoxel";

static const std::string s_IMAGE_INOUT = "image";
static const std::string s_TF_INOUT    = "tf";

static const std::string s_ENABLE_APLHA_CONFIG = "tfalpha";

static const std::string TRANSPARENCY_FIELD = "TRANSPARENCY";
static const std::string VISIBILITY_FIELD   = "VISIBILITY";

//------------------------------------------------------------------------------

SNegato3D::SNegato3D() noexcept :
    m_helperTF(std::bind(&SNegato3D::updateTF, this))
{
    newSlot(s_NEWIMAGE_SLOT, &SNegato3D::newImage, this);
    newSlot(s_SLICETYPE_SLOT, &SNegato3D::changeSliceType, this);
    newSlot(s_SLICEINDEX_SLOT, &SNegato3D::changeSliceIndex, this);
    newSlot(s_UPDATE_OPACITY_SLOT, &SNegato3D::setPlanesOpacity, this);
    newSlot(s_UPDATE_VISIBILITY_SLOT, &SNegato3D::setPlanesOpacity, this);
    newSlot(s_SET_VISIBILITY_SLOT, &SNegato3D::setVisibility, this);

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

    const ConfigType config = this->getConfigTree().get_child("config.<xmlattr>");

    if(config.count("autoresetcamera"))
    {
        m_autoResetCamera = config.get<std::string>("autoresetcamera") == "yes";
    }
    if(config.count("transform"))
    {
        this->setTransformId(config.get<std::string>("transform"));
    }
    if(config.count("filtering"))
    {
        const std::string filteringValue = config.get<std::string>("filtering");
        ::fwRenderOgre::Plane::FilteringEnumType filtering(::fwRenderOgre::Plane::FilteringEnumType::LINEAR);

        if(filteringValue == "none")
        {
            filtering = ::fwRenderOgre::Plane::FilteringEnumType::NONE;
        }
        else if(filteringValue == "anisotropic")
        {
            filtering = ::fwRenderOgre::Plane::FilteringEnumType::ANISOTROPIC;
        }

        this->setFiltering(filtering);
    }

    m_enableAlpha = config.get<bool>("tfalpha", m_enableAlpha);
    m_interactive = config.get<bool>("interactive", m_interactive);
}

//------------------------------------------------------------------------------

void SNegato3D::starting()
{
    this->initialize();

    ::fwData::TransferFunction::sptr tf = this->getInOut< ::fwData::TransferFunction>(s_TF_INOUT);

    ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
    SLM_ASSERT("inout '" + s_IMAGE_INOUT + "' is missing", image);

    m_helperTF.setOrCreateTF(tf, image);

    // 3D source texture instantiation
    m_3DOgreTexture = ::Ogre::dynamic_pointer_cast< ::Ogre::Texture >(
        ::Ogre::TextureManager::getSingleton().createOrRetrieve(this->getID() + "_Texture",
                                                                ::Ogre::ResourceGroupManager::
                                                                DEFAULT_RESOURCE_GROUP_NAME, true).first );

    // TF texture initialization
    m_gpuTF = std::unique_ptr< ::fwRenderOgre::TransferFunction>(new ::fwRenderOgre::TransferFunction());
    m_gpuTF->createTexture(this->getID());

    // Scene node's instantiation
    m_negatoSceneNode = this->getSceneManager()->getRootSceneNode()->createChildSceneNode();

    // Instanciation of the planes
    m_planes[0] = std::make_shared< ::fwRenderOgre::Plane >(this->getID(), m_negatoSceneNode,
                                                            getSceneManager(),
                                                            OrientationMode::X_AXIS, true, m_3DOgreTexture,
                                                            m_filtering);
    m_planes[1] = std::make_shared< ::fwRenderOgre::Plane >(this->getID(), m_negatoSceneNode,
                                                            getSceneManager(),
                                                            OrientationMode::Y_AXIS, true, m_3DOgreTexture,
                                                            m_filtering);
    m_planes[2] = std::make_shared< ::fwRenderOgre::Plane >(this->getID(), m_negatoSceneNode,
                                                            getSceneManager(),
                                                            OrientationMode::Z_AXIS, true, m_3DOgreTexture,
                                                            m_filtering);

    this->setPlanesOpacity();

    if (m_autoResetCamera)
    {
        this->getRenderService()->resetCameraCoordinates(m_layerID);
    }

    bool isValid = ::fwDataTools::fieldHelper::MedicalImageHelpers::checkImageValidity(image);
    if (isValid)
    {
        this->newImage();
    }

    if(m_interactive)
    {
        auto interactor = std::dynamic_pointer_cast< ::fwRenderOgre::interactor::IInteractor >(this->getSptr());
        this->getLayer()->addInteractor(interactor, 1);

        m_pickingCross = this->getSceneManager()->createManualObject(this->getID() + "_PickingCross");
        const auto basicAmbientMat = ::Ogre::MaterialManager::getSingleton().getByName("BasicAmbient");
        auto crossMat              = basicAmbientMat->clone(this->getID() + "_CrossMaterial");
        crossMat->setAmbient(::Ogre::ColourValue::Red);
        crossMat->setDiffuse(::Ogre::ColourValue::Red);
        crossMat->setDepthBias(1.f, 1.f);
        m_pickingCross->estimateVertexCount(4);
        m_pickingCross->begin(crossMat, ::Ogre::RenderOperation::OT_LINE_LIST);
        for(std::uint8_t i = 0; i < 4; ++i)
        {
            m_pickingCross->position(::Ogre::Vector3::ZERO);
        }
        m_pickingCross->end();
        m_pickingCross->setVisible(false);
        m_negatoSceneNode->attachObject(m_pickingCross);
    }
}

//------------------------------------------------------------------------------

void SNegato3D::stopping()
{
    m_helperTF.removeTFConnections();

    m_pickedPlane.reset();
    m_planes[0].reset();
    m_planes[1].reset();
    m_planes[2].reset();

    this->getSceneManager()->destroySceneNode(m_negatoSceneNode);

    if(m_pickingCross)
    {
        auto crossMtl = m_pickingCross->getSection(0)->getMaterial();
        ::Ogre::MaterialManager::getSingleton().remove(crossMtl);

        this->getSceneManager()->destroyManualObject(m_pickingCross);
    }

    m_3DOgreTexture.reset();
    m_gpuTF.reset();

    this->requestRender();
}

//------------------------------------------------------------------------------

void SNegato3D::updating()
{
    this->requestRender();
}

//------------------------------------------------------------------------------

void SNegato3D::swapping(const KeyType& key)
{
    if (key == s_TF_INOUT)
    {
        ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
        SLM_ASSERT("Missing image", image);

        ::fwData::TransferFunction::sptr tf = this->getInOut< ::fwData::TransferFunction>(s_TF_INOUT);
        m_helperTF.setOrCreateTF(tf, image);

        this->updateTF();
    }
}

//------------------------------------------------------------------------------

void SNegato3D::createPlanes(const ::fwData::Image::SpacingType& _spacing, const ::fwData::Image::OriginType& _origin)
{
    ::Ogre::Vector3 origin(static_cast< ::Ogre::Real >(_origin[0]),
                           static_cast< ::Ogre::Real >(_origin[1]),
                           static_cast< ::Ogre::Real >(_origin[2]));

    // Fits the planes to the new texture
    for(const auto& plane : m_planes)
    {
        plane->setDepthSpacing(_spacing);
        plane->setOriginPosition(origin);
        plane->initializePlane();
        plane->enableAlpha(m_enableAlpha);
        plane->setQueryFlags(0x1);
    }
}

//------------------------------------------------------------------------------

void SNegato3D::newImage()
{
    this->getRenderService()->makeCurrent();

    ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
    SLM_ASSERT("inout '" + s_IMAGE_INOUT + "' is missing", image);

    ::fwRenderOgre::Utils::convertImageForNegato(m_3DOgreTexture.get(), image);

    createPlanes(image->getSpacing(), image->getOrigin());

    // Update Slice
    const auto axialIndex =
        image->getField< ::fwData::Integer >(::fwDataTools::fieldHelper::Image::m_axialSliceIndexId)->getValue();
    const auto frontalIndex =
        image->getField< ::fwData::Integer >(::fwDataTools::fieldHelper::Image::m_frontalSliceIndexId)->getValue();
    const auto sagittalIndex =
        image->getField< ::fwData::Integer >(::fwDataTools::fieldHelper::Image::m_sagittalSliceIndexId)->getValue();

    this->changeSliceIndex(
        static_cast<int>(axialIndex),
        static_cast<int>(frontalIndex),
        static_cast<int>(sagittalIndex)
        );

    // Update tranfer function in Gpu programs
    this->updateTF();

    if (m_autoResetCamera)
    {
        this->getRenderService()->resetCameraCoordinates(m_layerID);
    }

    this->setPlanesOpacity();

    this->requestRender();
}

//------------------------------------------------------------------------------

void SNegato3D::changeSliceType(int /*_from*/, int /* _to */)
{
    this->getRenderService()->makeCurrent();

    // Update TF
    this->updateTF();

    this->requestRender();
}

//------------------------------------------------------------------------------

void SNegato3D::changeSliceIndex(int _axialIndex, int _frontalIndex, int _sagittalIndex)
{
    ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
    SLM_ASSERT("inout '" + s_IMAGE_INOUT + "' is missing", image);

    ::Ogre::Vector3 sliceIndices = {
        static_cast<float>(_sagittalIndex ) / (static_cast<float>(image->getSize()[0] - 1)),
        static_cast<float>(_frontalIndex  ) / (static_cast<float>(image->getSize()[1] - 1)),
        static_cast<float>(_axialIndex    ) / (static_cast<float>(image->getSize()[2] - 1))
    };

    for (int i = 0; i < 3; ++i)
    {
        m_planes[i]->changeSlice( sliceIndices[i] );
    }

    this->requestRender();
}

//-----------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SNegato3D::getAutoConnections() const
{
    ::fwServices::IService::KeyConnectionsMap connections;
    connections.push( s_IMAGE_INOUT, ::fwData::Image::s_MODIFIED_SIG, s_NEWIMAGE_SLOT );
    connections.push( s_IMAGE_INOUT, ::fwData::Image::s_BUFFER_MODIFIED_SIG, s_NEWIMAGE_SLOT );
    connections.push( s_IMAGE_INOUT, ::fwData::Image::s_SLICE_TYPE_MODIFIED_SIG, s_SLICETYPE_SLOT );
    connections.push( s_IMAGE_INOUT, ::fwData::Image::s_SLICE_INDEX_MODIFIED_SIG, s_SLICEINDEX_SLOT );
    connections.push( s_IMAGE_INOUT, ::fwData::Image::s_VISIBILITY_MODIFIED_SIG, s_UPDATE_VISIBILITY_SLOT );
    connections.push( s_IMAGE_INOUT, ::fwData::Image::s_TRANSPARENCY_MODIFIED_SIG, s_UPDATE_VISIBILITY_SLOT );
    return connections;
}

//-----------------------------------------------------------------------------

void SNegato3D::updateTF()
{
    const ::fwData::TransferFunction::csptr tf = m_helperTF.getTransferFunction();
    {
        const ::fwData::mt::ObjectReadLock tfLock(tf);
        m_gpuTF->updateTexture(tf);

        for(int i(0); i < 3; ++i)
        {
            m_planes[i]->switchThresholding(tf->getIsClamped());

            // Sends the TF texture to the negato-related passes
            m_planes[i]->setTFData(*m_gpuTF.get());
        }
    }

    this->requestRender();
}

//-----------------------------------------------------------------------------

void SNegato3D::setPlanesOpacity()
{
    ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
    SLM_ASSERT("inout '" + s_IMAGE_INOUT + "' is missing", image);

    ::fwData::Integer::sptr transparency = image->setDefaultField(TRANSPARENCY_FIELD, ::fwData::Integer::New(0));
    ::fwData::Boolean::sptr isVisible    = image->setDefaultField(VISIBILITY_FIELD, ::fwData::Boolean::New(true));

    const bool visible  = isVisible->getValue();
    const float opacity = (100.f - static_cast<float>(transparency->getValue()))/100.f;

    if(m_planes[0] && m_planes[1] && m_planes[2])
    {
        m_planes[0]->setVisible(visible);
        m_planes[1]->setVisible(visible);
        m_planes[2]->setVisible(visible);

        m_planes[0]->setEntityOpacity(opacity);
        m_planes[1]->setEntityOpacity(opacity);
        m_planes[2]->setEntityOpacity(opacity);
    }

    this->requestRender();
}

//------------------------------------------------------------------------------

void SNegato3D::setVisibility(bool visibility)
{
    ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
    SLM_ASSERT("inout '" + s_IMAGE_INOUT + "' is missing", image);

    image->setField(VISIBILITY_FIELD, ::fwData::Boolean::New(visibility));

    this->setPlanesOpacity();

    using VisModSigType = ::fwData::Image::VisibilityModifiedSignalType;
    auto visUpdateSig = image->signal<VisModSigType>( ::fwData::Image::s_VISIBILITY_MODIFIED_SIG );

    {
        ::fwCom::Connection::Blocker(visUpdateSig->getConnection(this->slot(s_UPDATE_VISIBILITY_SLOT)));
        visUpdateSig->asyncEmit(visibility);
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

void SNegato3D::mouseMoveEvent(MouseButton _button, int _x, int _y, int, int)
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
            this->getLayer()->cancelFurtherInteraction();
        }
        else if(_button == MouseButton::LEFT)
        {
            this->pickIntensity(_x, _y);
        }

        this->getLayer()->cancelFurtherInteraction();
    }
}

//------------------------------------------------------------------------------

void SNegato3D::buttonPressEvent(MouseButton _button, int _x, int _y)
{
    this->setPlanesQueryFlags(0x1); // Make all planes pickable again.
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
            ::fwData::TransferFunction::sptr tf = m_helperTF.getTransferFunction();
            ::fwData::mt::ObjectReadLock tfLock(tf);

            m_initialLevel  = tf->getLevel();
            m_initialWindow = tf->getWindow();

            m_initialPos = { _x, _y };
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

void SNegato3D::buttonReleaseEvent(MouseButton, int, int)
{
    m_pickedPlane.reset();
    m_pickingCross->setVisible(false);
    m_pickedVoxelSignal->asyncEmit("");
}

//------------------------------------------------------------------------------

void SNegato3D::moveSlices(int _x, int _y)
{
    const auto pickRes = this->getPickedSlices(_x, _y);

    if(pickRes.has_value())
    {
        ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
        SLM_ASSERT("inout '" + s_IMAGE_INOUT + "' is missing", image);
        ::fwData::mt::ObjectReadLock imgLock(image);

        auto pickedPt = pickRes.value();

        for(const auto&  p : m_planes)
        {
            if(p != m_pickedPlane)
            {
                p->setQueryFlags(0x0);
            }
        }

        const auto [spacing, origin] = ::fwRenderOgre::Utils::convertSpacingAndOrigin(image);
        pickedPt                     = pickedPt / spacing - origin;

        const ::Ogre::Vector3i pickedPtI(pickedPt);
        auto sig = image->signal< ::fwData::Image::SliceIndexModifiedSignalType >
                       (::fwData::Image::s_SLICE_INDEX_MODIFIED_SIG);
        sig->asyncEmit(pickedPtI[2], pickedPtI[1], pickedPtI[0]);
    }
}

//------------------------------------------------------------------------------

void SNegato3D::pickIntensity(int _x, int _y)
{
    const auto pickedPos = this->getPickedSlices(_x, _y);

    if(pickedPos.has_value() && m_pickedVoxelSignal->getNumberOfConnections() > 0)
    {
        this->updatePickingCross(pickedPos.value());

        ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
        SLM_ASSERT("inout '" + s_IMAGE_INOUT + "' is missing", image);
        ::fwData::mt::ObjectReadLock imgLock(image);
        auto imageBufferLock = image->lock();

        const auto [spacing, origin] = ::fwRenderOgre::Utils::convertSpacingAndOrigin(image);
        const ::Ogre::Vector3i pickedVoxel(pickedPos.value() / spacing - origin);

        const auto intensity   = image->getPixelAsString(pickedVoxel[0], pickedVoxel[1], pickedVoxel[2]);
        const auto pickingText = "(" + std::to_string(pickedVoxel[0]) + ", " + std::to_string(pickedVoxel[1])
                                 + ", " + std::to_string(pickedVoxel[2]) + "): " + intensity;

        m_pickedVoxelSignal->asyncEmit(pickingText);
    }
}

//------------------------------------------------------------------------------

std::optional< ::Ogre::Vector3 > SNegato3D::getPickedSlices(int _x, int _y)
{
    auto* const sceneManager = this->getSceneManager();
    SLM_ASSERT("Scene manager not yet created.", sceneManager);
    ::Ogre::Camera* camera = sceneManager->getCamera(::fwRenderOgre::Layer::DEFAULT_CAMERA_NAME);

    const int height = camera->getViewport()->getActualHeight();
    const int width  = camera->getViewport()->getActualWidth();

    ::fwRenderOgre::picker::IPicker picker;
    picker.setSceneManager(this->getSceneManager());
    picker.executeRaySceneQuery(_x, _y, width, height, 0x1);

    auto isPicked = [&picker](const ::fwRenderOgre::Plane::sptr& _p)
                    {
                        return _p->getMovableObject() == picker.getSelectedObject();
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

void SNegato3D::updatePickingCross(const ::Ogre::Vector3& _pickedPos)
{
    const float h = m_pickedPlane->getHeight();
    const float w = m_pickedPlane->getWidth();

    m_pickingCross->beginUpdate(0);
    switch(m_pickedPlane->getOrientationMode())
    {
        case ::fwRenderOgre::Plane::OrientationMode::X_AXIS:
            m_pickingCross->position(_pickedPos.x, 0, _pickedPos.z);
            m_pickingCross->position(_pickedPos.x, h, _pickedPos.z);
            m_pickingCross->position(_pickedPos.x, _pickedPos.y, 0);
            m_pickingCross->position(_pickedPos.x, _pickedPos.y, w);
            break;
        case ::fwRenderOgre::Plane::OrientationMode::Y_AXIS:
            m_pickingCross->position(0, _pickedPos.y, _pickedPos.z);
            m_pickingCross->position(w, _pickedPos.y, _pickedPos.z);
            m_pickingCross->position(_pickedPos.x, _pickedPos.y, 0);
            m_pickingCross->position(_pickedPos.x, _pickedPos.y, h);
            break;
        case ::fwRenderOgre::Plane::OrientationMode::Z_AXIS:
            m_pickingCross->position(0, _pickedPos.y, _pickedPos.z);
            m_pickingCross->position(w, _pickedPos.y, _pickedPos.z);
            m_pickingCross->position(_pickedPos.x, 0, _pickedPos.z);
            m_pickingCross->position(_pickedPos.x, h, _pickedPos.z);
            break;
    }
    m_pickingCross->end();
    m_pickingCross->setVisible(true);
}

//------------------------------------------------------------------------------

void SNegato3D::updateWindowing( double _dw, double _dl )
{
    const double newWindow = m_initialWindow + _dw;
    const double newLevel  = m_initialLevel - _dl;

    ::fwData::TransferFunction::sptr tf = m_helperTF.getTransferFunction();
    {
        ::fwData::mt::ObjectWriteLock tfLock(tf);

        tf->setWindow( newWindow );
        tf->setLevel( newLevel );
        auto sig = tf->signal< ::fwData::TransferFunction::WindowingModifiedSignalType >(
            ::fwData::TransferFunction::s_WINDOWING_MODIFIED_SIG);
        {
            sig->asyncEmit(newWindow, newLevel);
        }
    }
}

//------------------------------------------------------------------------------

void SNegato3D::wheelEvent(int, int, int)
{
}
//------------------------------------------------------------------------------

void SNegato3D::resizeEvent(int, int)
{
}
//------------------------------------------------------------------------------

void SNegato3D::keyPressEvent(int)
{
}
//------------------------------------------------------------------------------

void SNegato3D::keyReleaseEvent(int)
{
}
//------------------------------------------------------------------------------

void SNegato3D::focusInEvent()
{
}
//------------------------------------------------------------------------------

void SNegato3D::focusOutEvent()
{
}

} // namespace visuOgreAdaptor

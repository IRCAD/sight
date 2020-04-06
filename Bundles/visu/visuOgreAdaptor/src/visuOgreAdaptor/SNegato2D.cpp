/************************************************************************
 *
 * Copyright (C) 2014-2020 IRCAD France
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

#include "visuOgreAdaptor/SNegato2D.hpp"

#include <fwCom/Signals.hpp>
#include <fwCom/Slots.hxx>

#include <fwData/Image.hpp>
#include <fwData/mt/ObjectReadLock.hpp>
#include <fwData/mt/ObjectWriteLock.hpp>

#include <fwDataTools/fieldHelper/Image.hpp>
#include <fwDataTools/fieldHelper/MedicalImageHelpers.hpp>

#include <fwRenderOgre/Utils.hpp>

#include <fwServices/macros.hpp>

#include <OgreSceneNode.h>
#include <OgreTextureManager.h>

#include <algorithm>

namespace visuOgreAdaptor
{

const ::fwCom::Slots::SlotKeyType s_NEWIMAGE_SLOT   = "newImage";
const ::fwCom::Slots::SlotKeyType s_SLICETYPE_SLOT  = "sliceType";
const ::fwCom::Slots::SlotKeyType s_SLICEINDEX_SLOT = "sliceIndex";

static const ::fwCom::Signals::SignalKeyType s_SLICE_INDEX_CHANGED_SIG = "sliceIndexChanged";

static const ::fwServices::IService::KeyType s_IMAGE_INOUT = "image";
static const ::fwServices::IService::KeyType s_TF_INOUT    = "tf";

static const std::string s_SLICE_INDEX_CONFIG = "sliceIndex";
static const std::string s_FILTERING_CONFIG   = "filtering";
static const std::string s_TF_ALPHA_CONFIG    = "tfAlpha";
static const std::string s_BORDER_CONFIG      = "border";

//------------------------------------------------------------------------------

SNegato2D::SNegato2D() noexcept :
    m_helperTF(std::bind(&SNegato2D::updateTF, this))
{
    newSlot(s_NEWIMAGE_SLOT, &SNegato2D::newImageDeprecatedSlot, this);
    newSlot(s_SLICETYPE_SLOT, &SNegato2D::changeSliceType, this);
    newSlot(s_SLICEINDEX_SLOT, &SNegato2D::changeSliceIndex, this);

    m_sliceIndexChangedSig = this->newSignal<SliceIndexChangedSignalType>(s_SLICE_INDEX_CHANGED_SIG);
}

//------------------------------------------------------------------------------

SNegato2D::~SNegato2D() noexcept
{
}

//------------------------------------------------------------------------------

void SNegato2D::configuring()
{
    this->configureParams();

    const ConfigType configType = this->getConfigTree();
    const ConfigType config     = configType.get_child("config.<xmlattr>");

    const std::string orientation = config.get<std::string>(s_SLICE_INDEX_CONFIG, "axial");
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

    if(config.count(s_FILTERING_CONFIG))
    {
        const std::string filteringValue = config.get<std::string>(s_FILTERING_CONFIG);
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

    m_enableAlpha = config.get<bool>(s_TF_ALPHA_CONFIG, m_enableAlpha);
    m_border      = config.get<bool>(s_TF_ALPHA_CONFIG, m_border);
}

//------------------------------------------------------------------------------

void SNegato2D::starting()
{
    this->initialize();
    this->getRenderService()->makeCurrent();

    ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
    SLM_ASSERT("inout '" + s_IMAGE_INOUT + "' is missing.", image);

    ::fwData::TransferFunction::sptr tf = this->getInOut< ::fwData::TransferFunction>(s_TF_INOUT);
    m_helperTF.setOrCreateTF(tf, image);

    // 3D source texture instantiation
    m_3DOgreTexture = ::Ogre::TextureManager::getSingleton().create(
        this->getID() + "_Texture",
        ::Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
        true);

    // TF texture initialization
    m_gpuTF = std::unique_ptr< ::fwRenderOgre::TransferFunction>(new ::fwRenderOgre::TransferFunction());
    m_gpuTF->createTexture(this->getID());

    // Scene node's instanciation
    m_negatoSceneNode = this->getSceneManager()->getRootSceneNode()->createChildSceneNode();

    // Plane's instanciation
    m_plane = std::make_unique< ::fwRenderOgre::Plane >(this->getID(), m_negatoSceneNode, getSceneManager(),
                                                        m_orientation, m_3DOgreTexture, m_filtering, m_border);

    this->newImage();
}

//------------------------------------------------------------------------------

void SNegato2D::stopping()
{
    this->getRenderService()->makeCurrent();

    m_helperTF.removeTFConnections();

    m_plane.reset();

    m_3DOgreTexture.reset();
    m_gpuTF.reset();

    this->requestRender();
}

//------------------------------------------------------------------------------

void SNegato2D::updating()
{
    this->newImage();
    this->requestRender();
}

//------------------------------------------------------------------------------

void SNegato2D::swapping(const KeyType& _key)
{
    if (_key == s_TF_INOUT)
    {
        ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
        SLM_ASSERT("inout '" + s_IMAGE_INOUT + "' is missing.", image);

        ::fwData::TransferFunction::sptr tf = this->getInOut< ::fwData::TransferFunction>(s_TF_INOUT);
        m_helperTF.setOrCreateTF(tf, image);

        this->updateTF();
    }
}

//------------------------------------------------------------------------------

void SNegato2D::newImage()
{
    if(!m_3DOgreTexture)
    {
        // The adaptor hasn't start yet (the window is maybe not visible)
        return;
    }
    this->getRenderService()->makeCurrent();

    const ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
    SLM_ASSERT("inout '" + s_IMAGE_INOUT + "' is missing.", image);

    const ::fwData::TransferFunction::sptr tf = this->getInOut< ::fwData::TransferFunction>(s_TF_INOUT);
    m_helperTF.setOrCreateTF(tf, image);

    const ::fwData::mt::ObjectReadLock imgLock(image);

    if(::fwDataTools::fieldHelper::MedicalImageHelpers::checkImageValidity(image))
    {
        // Retrieves or creates the slice index fields
        ::fwRenderOgre::Utils::convertImageForNegato(m_3DOgreTexture.get(), image);

        const auto [spacing, origin] = ::fwRenderOgre::Utils::convertSpacingAndOrigin(image);
        this->createPlane(spacing);
        m_plane->setOriginPosition(origin);

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

        this->requestRender();
    }
}

//------------------------------------------------------------------------------

void SNegato2D::newImageDeprecatedSlot()
{
    FW_DEPRECATED_MSG("The 'newImage' slot will be removed in sight 21.0. Call 'update' instead.", "21.0");
    this->newImage();
}

//------------------------------------------------------------------------------

void SNegato2D::changeSliceType(int _from, int _to)
{
    const ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
    SLM_ASSERT("inout '" + s_IMAGE_INOUT + "' is missing.", image);
    const ::fwData::mt::ObjectReadLock imgLock(image);

    const auto toOrientation   = static_cast<OrientationMode>(_to);
    const auto fromOrientation = static_cast<OrientationMode>(_from);

    const auto planeOrientation = m_plane->getOrientationMode();
    const auto newOrientation   = planeOrientation == toOrientation ? fromOrientation :
                                  planeOrientation == fromOrientation ? toOrientation : planeOrientation;

    if(planeOrientation != newOrientation)
    {
        this->getRenderService()->makeCurrent();

        m_plane->setOrientationMode(newOrientation);

        const auto origin = ::fwRenderOgre::Utils::convertSpacingAndOrigin(image).second;
        m_plane->setOriginPosition(origin);

        // Update threshold if necessary
        this->updateTF();

        this->updateShaderSliceIndexParameter();

        this->requestRender();
    }
}

//------------------------------------------------------------------------------

void SNegato2D::changeSliceIndex(int _axialIndex, int _frontalIndex, int _sagittalIndex)
{
    const ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
    SLM_ASSERT("inout '" + s_IMAGE_INOUT + "' is missing.", image);
    const ::fwData::mt::ObjectReadLock imgLock(image);

    this->getRenderService()->makeCurrent();

    const auto& imgSize = image->getSize2();

    m_currentSliceIndex = {
        static_cast<float>(_sagittalIndex ) / (static_cast<float>(imgSize[0] - 1)),
        static_cast<float>(_frontalIndex  ) / (static_cast<float>(imgSize[1] - 1)),
        static_cast<float>(_axialIndex    ) / (static_cast<float>(imgSize[2] - 1))
    };

    this->updateShaderSliceIndexParameter();

    m_sliceIndexChangedSig->emit();
}

//------------------------------------------------------------------------------

void SNegato2D::updateShaderSliceIndexParameter()
{
    this->getRenderService()->makeCurrent();
    m_plane->changeSlice( m_currentSliceIndex[static_cast<size_t>(m_plane->getOrientationMode())] );

    this->requestRender();
}

//------------------------------------------------------------------------------

void SNegato2D::updateTF()
{
    this->getRenderService()->makeCurrent();

    const ::fwData::TransferFunction::csptr tf = m_helperTF.getTransferFunction();
    {
        const ::fwData::mt::ObjectReadLock tfLock(tf);
        m_gpuTF->updateTexture(tf);

        m_plane->switchThresholding(tf->getIsClamped());
    }

    // Sends the TF texture to the negato-related passes
    m_plane->setTFData(*m_gpuTF.get());

    this->requestRender();
}

//-----------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SNegato2D::getAutoConnections() const
{
    ::fwServices::IService::KeyConnectionsMap connections;
    connections.push( s_IMAGE_INOUT, ::fwData::Image::s_MODIFIED_SIG, s_UPDATE_SLOT );
    connections.push( s_IMAGE_INOUT, ::fwData::Image::s_BUFFER_MODIFIED_SIG, s_UPDATE_SLOT );
    connections.push( s_IMAGE_INOUT, ::fwData::Image::s_SLICE_TYPE_MODIFIED_SIG, s_SLICETYPE_SLOT );
    connections.push( s_IMAGE_INOUT, ::fwData::Image::s_SLICE_INDEX_MODIFIED_SIG, s_SLICEINDEX_SLOT );

    return connections;
}

//------------------------------------------------------------------------------

void SNegato2D::createPlane(const ::Ogre::Vector3& _spacing)
{
    this->getRenderService()->makeCurrent();
    // Fits the plane to the new texture
    m_plane->setVoxelSpacing(_spacing);
    m_plane->initializePlane();
    m_plane->enableAlpha(m_enableAlpha);
}

} // namespace visuOgreAdaptor.

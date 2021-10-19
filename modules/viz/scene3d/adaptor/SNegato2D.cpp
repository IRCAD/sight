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

#include "modules/viz/scene3d/adaptor/SNegato2D.hpp"

#include <core/com/Signals.hpp>
#include <core/com/Slots.hxx>

#include <data/fieldHelper/Image.hpp>
#include <data/fieldHelper/MedicalImageHelpers.hpp>
#include <data/Image.hpp>

#include <service/macros.hpp>

#include <viz/scene3d/ogre.hpp>
#include <viz/scene3d/Utils.hpp>

#include <OgreSceneNode.h>
#include <OgreTextureManager.h>

#include <algorithm>

namespace sight::module::viz::scene3d::adaptor
{

const core::com::Slots::SlotKeyType s_SLICETYPE_SLOT  = "sliceType";
const core::com::Slots::SlotKeyType s_SLICEINDEX_SLOT = "sliceIndex";

static const core::com::Signals::SignalKeyType s_SLICE_INDEX_CHANGED_SIG = "sliceIndexChanged";

static const std::string s_SLICE_INDEX_CONFIG = "sliceIndex";
static const std::string s_FILTERING_CONFIG   = "filtering";
static const std::string s_TF_ALPHA_CONFIG    = "tfAlpha";
static const std::string s_BORDER_CONFIG      = "border";

//------------------------------------------------------------------------------

SNegato2D::SNegato2D() noexcept :
    m_helperTF(std::bind(&SNegato2D::updateTF, this))
{
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
        sight::viz::scene3d::Plane::FilteringEnumType filtering(sight::viz::scene3d::Plane::FilteringEnumType::LINEAR);

        if(filteringValue == "none")
        {
            filtering = sight::viz::scene3d::Plane::FilteringEnumType::NONE;
        }
        else if(filteringValue == "anisotropic")
        {
            filtering = sight::viz::scene3d::Plane::FilteringEnumType::ANISOTROPIC;
        }

        this->setFiltering(filtering);
    }

    m_enableAlpha = config.get<bool>(s_TF_ALPHA_CONFIG, m_enableAlpha);
    m_border      = config.get<bool>(s_BORDER_CONFIG, m_border);
}

//------------------------------------------------------------------------------

void SNegato2D::starting()
{
    this->initialize();
    this->getRenderService()->makeCurrent();
    {
        const auto image = m_image.lock();
        const auto tf    = m_tf.lock();
        m_helperTF.setOrCreateTF(tf.get_shared(), image.get_shared());
    }

    // 3D source texture instantiation
    m_3DOgreTexture = ::Ogre::TextureManager::getSingleton().create(
        this->getID() + "_Texture",
        sight::viz::scene3d::RESOURCE_GROUP,
        true
    );

    // TF texture initialization
    m_gpuTF = std::unique_ptr<sight::viz::scene3d::TransferFunction>(new sight::viz::scene3d::TransferFunction());
    m_gpuTF->createTexture(this->getID());

    // Scene node's instanciation
    m_negatoSceneNode = this->getSceneManager()->getRootSceneNode()->createChildSceneNode();

    // Plane's instanciation
    m_plane = std::make_unique<sight::viz::scene3d::Plane>(
        this->getID(),
        m_negatoSceneNode,
        getSceneManager(),
        m_orientation,
        m_3DOgreTexture,
        m_filtering,
        m_border
    );

    this->newImage();
    this->setVisible(m_isVisible);
}

//------------------------------------------------------------------------------

void SNegato2D::stopping()
{
    this->getRenderService()->makeCurrent();

    m_helperTF.removeTFConnections();

    m_plane.reset();

    ::Ogre::TextureManager::getSingleton().remove(m_3DOgreTexture);
    m_3DOgreTexture.reset();
    m_gpuTF.reset();

    this->requestRender();
}

//------------------------------------------------------------------------------

void SNegato2D::updating()
{
    this->newImage();
    this->setVisible(m_isVisible);
}

//------------------------------------------------------------------------------

void SNegato2D::swapping(std::string_view _key)
{
    if(_key == s_TF_INOUT)
    {
        const auto image = m_image.lock();
        const auto tf    = m_tf.lock();
        m_helperTF.setOrCreateTF(tf.get_shared(), image.get_shared());

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

    int axialIdx    = 0;
    int frontalIdx  = 0;
    int sagittalIdx = 0;
    {
        const auto image = m_image.lock();
        const auto tf    = m_tf.lock();
        m_helperTF.setOrCreateTF(tf.get_shared(), image.get_shared());

        if(!data::fieldHelper::MedicalImageHelpers::checkImageValidity(image.get_shared()))
        {
            return;
        }

        // Retrieves or creates the slice index fields
        sight::viz::scene3d::Utils::convertImageForNegato(m_3DOgreTexture.get(), image.get_shared());

        const auto [spacing, origin] = sight::viz::scene3d::Utils::convertSpacingAndOrigin(image.get_shared());
        this->createPlane(spacing);
        m_plane->setOriginPosition(origin);

        // Update Slice
        const auto imgSize       = image->getSize();
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

    this->requestRender();
}

//------------------------------------------------------------------------------

void SNegato2D::changeSliceType(int _from, int _to)
{
    const auto image = m_image.lock();

    const auto toOrientation   = static_cast<OrientationMode>(_to);
    const auto fromOrientation = static_cast<OrientationMode>(_from);

    const auto planeOrientation = m_plane->getOrientationMode();
    const auto newOrientation   = planeOrientation == toOrientation ? fromOrientation
                                                                    : planeOrientation
                                  == fromOrientation ? toOrientation : planeOrientation;

    if(planeOrientation != newOrientation)
    {
        this->getRenderService()->makeCurrent();

        m_plane->setOrientationMode(newOrientation);

        const auto origin = sight::viz::scene3d::Utils::convertSpacingAndOrigin(image.get_shared()).second;
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
    const auto image = m_image.lock();

    this->getRenderService()->makeCurrent();

    auto imgSize = image->getSize();

    // Sometimes, the image can contain only one slice,
    // it results into a division by 0 when the range is transformed between [0-1].
    // So we increase the image size to 2 to divide by 1.
    imgSize[0] = imgSize[0] == 1 ? 2 : imgSize[0];
    imgSize[1] = imgSize[1] == 1 ? 2 : imgSize[1];
    imgSize[2] = imgSize[2] == 1 ? 2 : imgSize[2];

    m_currentSliceIndex = {
        static_cast<float>(_sagittalIndex) / (static_cast<float>(imgSize[0] - 1)),
        static_cast<float>(_frontalIndex) / (static_cast<float>(imgSize[1] - 1)),
        static_cast<float>(_axialIndex) / (static_cast<float>(imgSize[2] - 1))
    };

    this->updateShaderSliceIndexParameter();

    m_sliceIndexChangedSig->emit();
}

//------------------------------------------------------------------------------

void SNegato2D::updateShaderSliceIndexParameter()
{
    this->getRenderService()->makeCurrent();
    m_plane->changeSlice(m_currentSliceIndex[static_cast<size_t>(m_plane->getOrientationMode())]);

    this->requestRender();
}

//------------------------------------------------------------------------------

void SNegato2D::updateTF()
{
    this->getRenderService()->makeCurrent();

    const data::TransferFunction::csptr tf = m_helperTF.getTransferFunction();
    {
        const data::mt::locked_ptr lock(tf);
        m_gpuTF->updateTexture(tf);

        m_plane->switchThresholding(tf->getIsClamped());
    }

    // Sends the TF texture to the negato-related passes
    m_plane->setTFData(*m_gpuTF.get());

    this->requestRender();
}

//-----------------------------------------------------------------------------

service::IService::KeyConnectionsMap SNegato2D::getAutoConnections() const
{
    service::IService::KeyConnectionsMap connections;
    connections.push(s_IMAGE_INOUT, data::Image::s_MODIFIED_SIG, s_UPDATE_SLOT);
    connections.push(s_IMAGE_INOUT, data::Image::s_BUFFER_MODIFIED_SIG, s_UPDATE_SLOT);
    connections.push(s_IMAGE_INOUT, data::Image::s_SLICE_TYPE_MODIFIED_SIG, s_SLICETYPE_SLOT);
    connections.push(s_IMAGE_INOUT, data::Image::s_SLICE_INDEX_MODIFIED_SIG, s_SLICEINDEX_SLOT);

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

//------------------------------------------------------------------------------
void SNegato2D::setVisible(bool _visible)
{
    if(m_negatoSceneNode)
    {
        m_negatoSceneNode->setVisible(_visible);

        this->requestRender();
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::module::viz::scene3d::adaptor.

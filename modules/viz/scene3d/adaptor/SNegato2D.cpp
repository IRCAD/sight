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

#include "modules/viz/scene3d/adaptor/SNegato2D.hpp"

#include <core/com/Signals.hpp>
#include <core/com/Slots.hxx>

#include <data/helper/MedicalImage.hpp>
#include <data/Image.hpp>

#include <service/macros.hpp>

#include <viz/scene3d/ogre.hpp>
#include <viz/scene3d/Utils.hpp>

#include <OgreSceneNode.h>

#include <algorithm>

namespace sight::module::viz::scene3d::adaptor
{

const core::com::Slots::SlotKeyType s_SLICETYPE_SLOT        = "sliceType";
const core::com::Slots::SlotKeyType s_SLICEINDEX_SLOT       = "sliceIndex";
static const core::com::Slots::SlotKeyType s_UPDATE_TF_SLOT = "updateTF";

static const core::com::Slots::SlotKeyType s_UPDATE_SLICES_FROM_WORLD = "updateSlicesFromWorld";

static const core::com::Signals::SignalKeyType s_SLICE_INDEX_CHANGED_SIG = "sliceIndexChanged";

static const std::string s_SLICE_INDEX_CONFIG = "sliceIndex";
static const std::string s_FILTERING_CONFIG   = "filtering";
static const std::string s_TF_ALPHA_CONFIG    = "tfAlpha";
static const std::string s_BORDER_CONFIG      = "border";
static const std::string s_TRANSFORM_CONFIG   = "transform";

//------------------------------------------------------------------------------

SNegato2D::SNegato2D() noexcept
{
    newSlot(s_SLICETYPE_SLOT, &SNegato2D::changeSliceType, this);
    newSlot(s_SLICEINDEX_SLOT, &SNegato2D::changeSliceIndex, this);
    newSlot(s_UPDATE_SLICES_FROM_WORLD, &SNegato2D::updateSlicesFromWorld, this);
    newSlot(s_UPDATE_TF_SLOT, &SNegato2D::updateTF, this);

    m_sliceIndexChangedSig = this->newSignal<SliceIndexChangedSignalType>(s_SLICE_INDEX_CHANGED_SIG);
}

//------------------------------------------------------------------------------

SNegato2D::~SNegato2D() noexcept =
    default;

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

    if(config.count(s_FILTERING_CONFIG) != 0U)
    {
        const auto filteringValue = config.get<std::string>(s_FILTERING_CONFIG);
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

    const std::string transformId =
        config.get<std::string>(sight::viz::scene3d::ITransformable::s_TRANSFORM_CONFIG, this->getID() + "_transform");
    this->setTransformId(transformId);
}

//------------------------------------------------------------------------------

void SNegato2D::starting()
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

    // Plane's instantiation
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

    m_plane.reset();
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

        if(!data::helper::MedicalImage::checkImageValidity(image.get_shared()))
        {
            return;
        }

        // Retrieves or creates the slice index fields
        m_3DOgreTexture->update();

        const auto [spacing, origin] = sight::viz::scene3d::Utils::convertSpacingAndOrigin(image.get_shared());
        this->createPlane(spacing);
        m_plane->setOriginPosition(origin);

        // Update Slice
        namespace imHelper = data::helper::MedicalImage;

        axialIdx    = std::max(0, int(imHelper::getSliceIndex(*image, imHelper::orientation_t::AXIAL).value_or(0)));
        frontalIdx  = std::max(0, int(imHelper::getSliceIndex(*image, imHelper::orientation_t::FRONTAL).value_or(0)));
        sagittalIdx = std::max(0, int(imHelper::getSliceIndex(*image, imHelper::orientation_t::SAGITTAL).value_or(0)));
    }

    this->changeSliceIndex(axialIdx, frontalIdx, sagittalIdx);

    // Update transfer function in Gpu programs
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

void SNegato2D::updateSlicesFromWorld(double _x, double _y, double _z)
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

void SNegato2D::updateShaderSliceIndexParameter()
{
    this->getRenderService()->makeCurrent();
    m_plane->changeSlice(m_currentSliceIndex[static_cast<std::size_t>(m_plane->getOrientationMode())]);

    this->requestRender();
}

//------------------------------------------------------------------------------

void SNegato2D::updateTF()
{
    this->getRenderService()->makeCurrent();

    m_gpuTF->update();

    // Sends the TF texture to the negato-related passes
    m_plane->setTFData(*m_gpuTF);

    this->requestRender();
}

//-----------------------------------------------------------------------------

service::IService::KeyConnectionsMap SNegato2D::getAutoConnections() const
{
    return {
        {s_IMAGE_IN, data::Image::s_MODIFIED_SIG, s_UPDATE_SLOT},
        {s_IMAGE_IN, data::Image::s_BUFFER_MODIFIED_SIG, s_UPDATE_SLOT},
        {s_IMAGE_IN, data::Image::s_SLICE_TYPE_MODIFIED_SIG, s_SLICETYPE_SLOT},
        {s_IMAGE_IN, data::Image::s_SLICE_INDEX_MODIFIED_SIG, s_SLICEINDEX_SLOT},
        {s_TF_IN, data::TransferFunction::s_MODIFIED_SIG, s_UPDATE_TF_SLOT},
        {s_TF_IN, data::TransferFunction::s_POINTS_MODIFIED_SIG, s_UPDATE_TF_SLOT},
        {s_TF_IN, data::TransferFunction::s_WINDOWING_MODIFIED_SIG, s_UPDATE_TF_SLOT}
    };
}

//------------------------------------------------------------------------------

void SNegato2D::createPlane(const Ogre::Vector3& _spacing)
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
    if(m_negatoSceneNode != nullptr)
    {
        m_negatoSceneNode->setVisible(_visible);

        this->requestRender();
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::module::viz::scene3d::adaptor.

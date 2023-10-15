/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
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

// cspell:ignore NOLINTNEXTLINE NOLINT

#include "modules/viz/scene3d/adaptor/negato3d.hpp"

#include <core/com/signal.hxx>
#include <core/com/slots.hxx>

#include <data/boolean.hpp>
#include <data/helper/MedicalImage.hpp>
#include <data/image.hpp>
#include <data/tools/color.hpp>

#include <viz/scene3d/ogre.hpp>
#include <viz/scene3d/Utils.hpp>

#include <OGRE/OgreCamera.h>
#include <OGRE/OgreSceneNode.h>
#include <OGRE/OgreVector.h>

#include <algorithm>

namespace sight::module::viz::scene3d::adaptor
{

static const core::com::slots::key_t NEWIMAGE_SLOT            = "newImage";
static const core::com::slots::key_t SLICETYPE_SLOT           = "sliceType";
static const core::com::slots::key_t SLICEINDEX_SLOT          = "sliceIndex";
static const core::com::slots::key_t UPDATE_SLICES_FROM_WORLD = "updateSlicesFromWorld";
static const core::com::slots::key_t SET_TRANSPARENCY_SLOT    = "setTransparency";
static const core::com::slots::key_t UPDATE_TF_SLOT           = "updateTF";

static const core::com::signals::key_t PICKED_VOXEL_SIG = "pickedVoxel";

//------------------------------------------------------------------------------

negato3d::negato3d() noexcept
{
    new_slot(NEWIMAGE_SLOT, &negato3d::newImage, this);
    new_slot(SLICETYPE_SLOT, &negato3d::changeSliceType, this);
    new_slot(SLICEINDEX_SLOT, &negato3d::changeSliceIndex, this);
    new_slot(SET_TRANSPARENCY_SLOT, &negato3d::setTransparency, this);
    new_slot(UPDATE_SLICES_FROM_WORLD, &negato3d::updateSlicesFromWorld, this);
    new_slot(UPDATE_TF_SLOT, &negato3d::updateTF, this);

    m_pickedVoxelSignal = new_signal<PickedVoxelSigType>(PICKED_VOXEL_SIG);
}

//------------------------------------------------------------------------------

negato3d::~negato3d() noexcept =
    default;

//------------------------------------------------------------------------------

void negato3d::configuring()
{
    this->configureParams();

    const config_t config = this->get_config();

    static const std::string s_AUTORESET_CAMERA_CONFIG = s_CONFIG + "autoresetcamera";
    static const std::string s_FILTERING_CONFIG        = s_CONFIG + "filtering";
    static const std::string s_TF_ALPHA_CONFIG         = s_CONFIG + "tfAlpha";
    static const std::string s_INTERACTIVE_CONFIG      = s_CONFIG + "interactive";
    static const std::string s_PRIORITY_CONFIG         = s_CONFIG + "priority";
    static const std::string s_QUERY_CONFIG            = s_CONFIG + "queryFlags";
    static const std::string s_BORDER_CONFIG           = s_CONFIG + "border";

    m_autoResetCamera = config.get<bool>(s_AUTORESET_CAMERA_CONFIG, true);

    if(config.count(s_FILTERING_CONFIG) != 0U)
    {
        const auto filteringValue = config.get<std::string>(s_FILTERING_CONFIG);
        sight::viz::scene3d::plane::filter_t filtering(sight::viz::scene3d::plane::filter_t::LINEAR);

        if(filteringValue == "none")
        {
            filtering = sight::viz::scene3d::plane::filter_t::NONE;
        }
        else if(filteringValue == "anisotropic")
        {
            filtering = sight::viz::scene3d::plane::filter_t::ANISOTROPIC;
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
        config.get<std::string>(sight::viz::scene3d::transformable::s_TRANSFORM_CONFIG, this->get_id() + "_transform");
    this->setTransformId(transformId);
}

//------------------------------------------------------------------------------

void negato3d::starting()
{
    this->initialize();

    this->getRenderService()->makeCurrent();
    {
        // 3D source texture instantiation
        const auto image = m_image.lock();
        m_3DOgreTexture = std::make_shared<sight::viz::scene3d::Texture>(image.get_shared());

        // TF texture initialization
        const auto tf = m_tf.lock();
        m_gpuTF = std::make_unique<sight::viz::scene3d::transfer_function>(tf.get_shared());
    }

    // Scene node's instantiation
    Ogre::SceneNode* const rootSceneNode = this->getSceneManager()->getRootSceneNode();
    Ogre::SceneNode* const transformNode = this->getOrCreateTransformNode(rootSceneNode);
    m_negatoSceneNode = transformNode->createChildSceneNode();

    // Instantiation of the planes
    for(auto& plane : m_planes)
    {
        plane = std::make_shared<sight::viz::scene3d::plane>(
            this->get_id(),
            m_negatoSceneNode,
            this->getSceneManager(),
            m_3DOgreTexture,
            m_filtering,
            m_border,
            false
        );
    }

    if(m_autoResetCamera)
    {
        this->getRenderService()->resetCameraCoordinates(m_layerID);
    }

    this->newImage();

    if(m_interactive)
    {
        auto interactor = std::dynamic_pointer_cast<sight::viz::scene3d::interactor::base>(this->get_sptr());
        this->getLayer()->addInteractor(interactor, m_priority);

        m_pickingCross = std::make_unique<sight::viz::scene3d::PickingCross>(
            this->get_id(),
            *this->getSceneManager(),
            *m_negatoSceneNode
        );
    }

    // Set the visibility of the 3D Negato
    this->setVisible(m_isVisible);
}

//-----------------------------------------------------------------------------

service::connections_t negato3d::auto_connections() const
{
    return {
        {s_IMAGE_IN, data::image::MODIFIED_SIG, NEWIMAGE_SLOT},
        {s_IMAGE_IN, data::image::BUFFER_MODIFIED_SIG, NEWIMAGE_SLOT},
        {s_IMAGE_IN, data::image::SLICE_TYPE_MODIFIED_SIG, SLICETYPE_SLOT},
        {s_IMAGE_IN, data::image::SLICE_INDEX_MODIFIED_SIG, SLICEINDEX_SLOT},
        {s_TF_INOUT, data::transfer_function::MODIFIED_SIG, UPDATE_TF_SLOT},
        {s_TF_INOUT, data::transfer_function::POINTS_MODIFIED_SIG, UPDATE_TF_SLOT},
        {s_TF_INOUT, data::transfer_function::WINDOWING_MODIFIED_SIG, UPDATE_TF_SLOT}
    };
}

//------------------------------------------------------------------------------

void negato3d::updating()
{
    this->requestRender();
}

//------------------------------------------------------------------------------

void negato3d::stopping()
{
    this->getRenderService()->makeCurrent();

    if(m_interactive)
    {
        auto interactor = std::dynamic_pointer_cast<sight::viz::scene3d::interactor::base>(this->get_sptr());
        this->getLayer()->removeInteractor(interactor);
    }

    m_pickedPlane.reset();
    std::ranges::for_each(m_planes, [](auto& p){p.reset();});

    m_negatoSceneNode->removeAndDestroyAllChildren();
    this->getSceneManager()->destroySceneNode(m_negatoSceneNode);

    m_pickingCross.reset();

    m_3DOgreTexture.reset();
    m_gpuTF.reset();

    this->requestRender();
}

//------------------------------------------------------------------------------

void negato3d::newImage()
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

        // Fits the planes to the new texture
        for(int orientationNum = 0 ; const auto& plane : m_planes)
        {
            plane->update(static_cast<OrientationMode>(orientationNum++), spacing, origin, m_enableAlpha);
            plane->setQueryFlags(m_queryFlags);
        }

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

void negato3d::changeSliceType(int /*unused*/, int /*unused*/)
{
    this->getRenderService()->makeCurrent();

    this->updateTF();

    this->requestRender();
}

//------------------------------------------------------------------------------

void negato3d::changeSliceIndex(int _axialIndex, int _frontalIndex, int _sagittalIndex)
{
    const auto image = m_image.lock();

    auto imgSize = image->size();

    // Sometimes, the image can contain only one slice,
    // it results into a division by 0 when the range is transformed between [0-1].
    // So we increase the image size to 2 to divide by 1.
    imgSize[0] = imgSize[0] == 1 ? 2 : imgSize[0];
    imgSize[1] = imgSize[1] == 1 ? 2 : imgSize[1];
    imgSize[2] = imgSize[2] == 1 ? 2 : imgSize[2];

    const std::array<float, 3> sliceIndices = {
        static_cast<float>(_sagittalIndex) / (static_cast<float>(imgSize[0] - 1)),
        static_cast<float>(_frontalIndex) / (static_cast<float>(imgSize[1] - 1)),
        static_cast<float>(_axialIndex) / (static_cast<float>(imgSize[2] - 1))
    };

    for(std::uint8_t i = 0 ; i < 3 ; ++i)
    {
        m_planes[i]->changeSlice(sliceIndices);
    }

    this->requestRender();
}

//-----------------------------------------------------------------------------

void negato3d::updateTF()
{
    this->getRenderService()->makeCurrent();
    m_gpuTF->update();

    // Sends the TF texture to the negato-related passes
    std::ranges::for_each(m_planes, [this](auto& p){p->setTFData(*m_gpuTF.get());});

    this->requestRender();
}

//-----------------------------------------------------------------------------

void negato3d::setTransparency(double _transparency)
{
    SIGHT_ASSERT("Service not started", this->started());

    const float opacity = 1.F - static_cast<float>(_transparency);
    std::ranges::for_each(m_planes, [opacity](auto& p){p->setEntityOpacity(opacity);});

    this->requestRender();
}

//------------------------------------------------------------------------------

void negato3d::setVisible(bool _visible)
{
    std::ranges::for_each(m_planes, [_visible](auto& p){p->setVisible(_visible);});
    this->requestRender();
}

//------------------------------------------------------------------------------

void negato3d::setPlanesQueryFlags(std::uint32_t _flags)
{
    std::ranges::for_each(m_planes, [_flags](auto& p){p->setQueryFlags(_flags);});
}

//------------------------------------------------------------------------------

void negato3d::mouseMoveEvent(MouseButton _button, Modifier /*_mods*/, int _x, int _y, int /*_dx*/, int /*_dy*/)
{
    if(m_pickedPlane)
    {
        if(_button == MouseButton::MIDDLE)
        {
            this->moveSlices(_x, _y);
        }
        else if(_button == MouseButton::RIGHT)
        {
            const auto dx = static_cast<double>(_x - m_initialPos[0]);
            const auto dy = static_cast<double>(m_initialPos[1] - _y);

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

void negato3d::buttonPressEvent(MouseButton _button, Modifier /*_mods*/, int _x, int _y)
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

void negato3d::buttonReleaseEvent(MouseButton /*_button*/, Modifier /*_mods*/, int /*_x*/, int /*_y*/)
{
    if(m_pickedPlane)
    {
        m_pickedPlane->setRenderQueuerGroupAndPriority(sight::viz::scene3d::rq::s_SURFACE_ID, 0);
        m_pickedPlane.reset();
    }

    m_pickingCross->setVisible(false);
    m_pickedVoxelSignal->async_emit("");
    this->setPlanesQueryFlags(m_queryFlags); // Make all planes pickable again.
}

//------------------------------------------------------------------------------

void negato3d::moveSlices(int _x, int _y)
{
    const auto pickRes = this->getPickedSlices(_x, _y);

    if(pickRes.has_value())
    {
        const auto image = m_image.lock();

        auto pickedPt = pickRes.value();

        std::ranges::for_each(
            m_planes,
            [this](auto& p)
            {
                if(p != m_pickedPlane)
                {
                    p->setQueryFlags(0x0);
                }
            });

        const auto [spacing, origin] = sight::viz::scene3d::Utils::convertSpacingAndOrigin(image.get_shared());
        pickedPt                     = (pickedPt - origin) / spacing;

        const Ogre::Vector3i pickedPtI(pickedPt);
        const auto sig = image->signal<data::image::SliceIndexModifiedSignalType>
                             (data::image::SLICE_INDEX_MODIFIED_SIG);
        sig->async_emit(pickedPtI[2], pickedPtI[1], pickedPtI[0]);
    }
}

//------------------------------------------------------------------------------

void negato3d::updateSlicesFromWorld(double _x, double _y, double _z)
{
    const auto image = m_image.lock();

    Ogre::Vector3 point = {static_cast<float>(_x), static_cast<float>(_y), static_cast<float>(_z)};
    Ogre::Vector3i slice_idx;
    try
    {
        slice_idx = sight::viz::scene3d::Utils::worldToSlices(*image, point);
    }
    catch(core::exception& _e)
    {
        SIGHT_WARN("Cannot update slice index: " << _e.what());
        return;
    }

    const auto sig = image->signal<data::image::SliceIndexModifiedSignalType>
                         (data::image::SLICE_INDEX_MODIFIED_SIG);

    sig->async_emit(slice_idx[2], slice_idx[1], slice_idx[0]);
}

//------------------------------------------------------------------------------

void negato3d::pickIntensity(int _x, int _y)
{
    if(m_pickedVoxelSignal->num_connections() > 0)
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

            auto crossLines = m_pickedPlane->computeCross(*pickedPos, origin);
            m_pickingCross->update(crossLines[0], crossLines[1], crossLines[2], crossLines[3]);
            const std::string pickingText = sight::viz::scene3d::Utils::pickImage(*image, *pickedPos, origin, spacing);
            m_pickedVoxelSignal->async_emit(pickingText);

            this->requestRender();

            // Render the picked plane before the widget.
            m_pickedPlane->setRenderQueuerGroupAndPriority(sight::viz::scene3d::rq::s_NEGATO_WIDGET_ID, 0);
        }
    }
}

//------------------------------------------------------------------------------

std::optional<Ogre::Vector3> negato3d::getPickedSlices(int _x, int _y)
{
    Ogre::SceneManager* sm = this->getSceneManager();
    const auto result      = sight::viz::scene3d::Utils::pickObject(_x, _y, m_queryFlags, *sm);

    if(result.has_value())
    {
        const auto isPicked = [&result](const auto& _p){return _p->getMovableObject() == result->first;};
        const auto it = std::ranges::find_if(m_planes, isPicked); // NOLINT(readability-qualified-auto,llvm-qualified-auto)

        if(it != m_planes.cend())
        {
            m_pickedPlane = *it;
            return result->second;
        }
    }

    return std::nullopt;
}

//------------------------------------------------------------------------------

void negato3d::updateWindowing(double _dw, double _dl)
{
    const double newWindow = m_initialWindow + _dw;
    const double newLevel  = m_initialLevel - _dl;

    {
        const auto tf = m_tf.lock();

        tf->setWindow(newWindow);
        tf->setLevel(newLevel);
        const auto sig = tf->signal<data::transfer_function::WindowingModifiedSignalType>(
            data::transfer_function::WINDOWING_MODIFIED_SIG
        );
        {
            sig->async_emit(newWindow, newLevel);
        }
    }
}

} // namespace sight::module::viz::scene3d::adaptor.

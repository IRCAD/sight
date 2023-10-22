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

#include "modules/viz/scene3d/adaptor/negato2d.hpp"

#include <core/com/signals.hpp>
#include <core/com/slots.hxx>

#include <data/helper/medical_image.hpp>
#include <data/image.hpp>

#include <service/macros.hpp>

#include <viz/scene3d/ogre.hpp>
#include <viz/scene3d/utils.hpp>

#include <OgreSceneNode.h>

#include <algorithm>

namespace sight::module::viz::scene3d::adaptor
{

const core::com::slots::key_t SLICETYPE_SLOT        = "sliceType";
const core::com::slots::key_t SLICEINDEX_SLOT       = "sliceIndex";
static const core::com::slots::key_t UPDATE_TF_SLOT = "updateTF";

static const core::com::slots::key_t UPDATE_SLICES_FROM_WORLD = "updateSlicesFromWorld";

static const core::com::signals::key_t SLICE_INDEX_CHANGED_SIG = "sliceIndexChanged";
static const core::com::signals::key_t PICKED_VOXEL_SIG        = "pickedVoxel";

//------------------------------------------------------------------------------

negato2d::negato2d() noexcept
{
    new_slot(SLICETYPE_SLOT, &negato2d::changeSliceType, this);
    new_slot(SLICEINDEX_SLOT, &negato2d::changeSliceIndex, this);
    new_slot(UPDATE_SLICES_FROM_WORLD, &negato2d::updateSlicesFromWorld, this);
    new_slot(UPDATE_TF_SLOT, &negato2d::updateTF, this);

    m_sliceIndexChangedSig = new_signal<SliceIndexChangedSignalType>(SLICE_INDEX_CHANGED_SIG);
    m_pickedVoxelSignal    = new_signal<picked_voxel_sig_t>(PICKED_VOXEL_SIG);
}

//------------------------------------------------------------------------------

void negato2d::configuring()
{
    this->configureParams();

    const config_t config = this->get_config();

    static const std::string s_SLICE_INDEX_CONFIG  = s_CONFIG + "sliceIndex";
    static const std::string s_FILTERING_CONFIG    = s_CONFIG + "filtering";
    static const std::string s_TF_ALPHA_CONFIG     = s_CONFIG + "tfAlpha";
    static const std::string s_BORDER_CONFIG       = s_CONFIG + "border";
    static const std::string s_SLICES_CROSS_CONFIG = s_CONFIG + "slicesCross";
    static const std::string s_INTERACTIVE_CONFIG  = s_CONFIG + "interactive";

    const auto orientation = config.get<std::string>(s_SLICE_INDEX_CONFIG, "axial");
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

    if(const auto filtering_cfg = config.get_optional<std::string>(s_FILTERING_CONFIG); filtering_cfg.has_value())
    {
        sight::viz::scene3d::plane::filter_t filtering(sight::viz::scene3d::plane::filter_t::LINEAR);

        if(filtering_cfg.value() == "none")
        {
            filtering = sight::viz::scene3d::plane::filter_t::NONE;
        }
        else if(filtering_cfg.value() == "anisotropic")
        {
            filtering = sight::viz::scene3d::plane::filter_t::ANISOTROPIC;
        }

        this->setFiltering(filtering);
    }

    m_enableAlpha = config.get<bool>(s_TF_ALPHA_CONFIG, m_enableAlpha);
    m_border      = config.get<bool>(s_BORDER_CONFIG, m_border);
    m_slicesCross = config.get<bool>(s_SLICES_CROSS_CONFIG, m_slicesCross);
    m_interactive = config.get<bool>(s_INTERACTIVE_CONFIG, m_interactive);

    const std::string transform_id =
        config.get<std::string>(sight::viz::scene3d::transformable::s_TRANSFORM_CONFIG, this->get_id() + "_transform");
    this->setTransformId(transform_id);
}

//------------------------------------------------------------------------------

void negato2d::starting()
{
    this->initialize();
    this->getRenderService()->makeCurrent();
    {
        // 3D source texture instantiation
        const auto image = m_image.lock();
        m_3DOgreTexture = std::make_shared<sight::viz::scene3d::texture>(image.get_shared());

        // TF texture initialization
        const auto tf = m_tf.lock();
        m_gpuTF = std::make_unique<sight::viz::scene3d::transfer_function>(tf.get_shared());
    }

    // scene node's instantiation
    Ogre::SceneNode* const root_scene_node = this->getSceneManager()->getRootSceneNode();
    Ogre::SceneNode* const transform_node  = this->getOrCreateTransformNode(root_scene_node);
    m_negatoSceneNode = transform_node->createChildSceneNode();

    // Plane's instantiation
    m_plane = std::make_unique<sight::viz::scene3d::plane>(
        this->get_id(),
        m_negatoSceneNode,
        getSceneManager(),
        m_3DOgreTexture,
        m_filtering,
        m_border,
        m_slicesCross
    );

    this->newImage();
    this->setVisible(m_isVisible);

    if(m_interactive)
    {
        auto interactor = std::dynamic_pointer_cast<sight::viz::scene3d::interactor::base>(this->get_sptr());
        this->getLayer()->addInteractor(interactor, 0);

        m_pickingCross = std::make_unique<sight::viz::scene3d::picking_cross>(
            this->get_id(),
            *this->getSceneManager(),
            *m_negatoSceneNode
        );
    }
}

//------------------------------------------------------------------------------

void negato2d::stopping()
{
    this->getRenderService()->makeCurrent();

    m_pickingCross.reset();
    m_plane.reset();
    m_3DOgreTexture.reset();
    m_gpuTF.reset();

    this->requestRender();
}

//------------------------------------------------------------------------------

void negato2d::updating()
{
    this->newImage();
    this->setVisible(m_isVisible);
}

//------------------------------------------------------------------------------

void negato2d::newImage()
{
    if(!m_3DOgreTexture)
    {
        // The adaptor hasn't start yet (the window is maybe not visible)
        return;
    }

    this->getRenderService()->makeCurrent();

    int axial_idx    = 0;
    int frontal_idx  = 0;
    int sagittal_idx = 0;
    {
        const auto image = m_image.lock();

        if(!data::helper::medical_image::check_image_validity(image.get_shared()))
        {
            return;
        }

        // Retrieves or creates the slice index fields
        m_3DOgreTexture->update();

        const auto [spacing, origin] = sight::viz::scene3d::utils::convertSpacingAndOrigin(image.get_shared());

        // Fits the plane to the new texture
        m_plane->update(m_orientation, spacing, origin, m_enableAlpha);

        // Render the picked plane before the picking cross
        m_plane->setRenderQueuerGroupAndPriority(sight::viz::scene3d::rq::s_NEGATO_WIDGET_ID, 0);

        // Update Slice
        namespace imHelper = data::helper::medical_image;

        axial_idx   = std::max(0, int(imHelper::get_slice_index(*image, imHelper::orientation_t::AXIAL).value_or(0)));
        frontal_idx =
            std::max(0, int(imHelper::get_slice_index(*image, imHelper::orientation_t::FRONTAL).value_or(0)));
        sagittal_idx =
            std::max(0, int(imHelper::get_slice_index(*image, imHelper::orientation_t::SAGITTAL).value_or(0)));
    }

    this->changeSliceIndex(axial_idx, frontal_idx, sagittal_idx);

    // Update transfer function in Gpu programs
    this->updateTF();

    this->requestRender();
}

//------------------------------------------------------------------------------

void negato2d::changeSliceType(int _from, int _to)
{
    const auto image = m_image.lock();

    const auto to_orientation   = static_cast<OrientationMode>(_to);
    const auto from_orientation = static_cast<OrientationMode>(_from);

    const auto plane_orientation = m_orientation;
    const auto new_orientation   = m_orientation == to_orientation ? from_orientation
                                                                   : plane_orientation
                                   == from_orientation ? to_orientation : plane_orientation;

    if(plane_orientation != new_orientation)
    {
        m_orientation = new_orientation;
        this->getRenderService()->makeCurrent();

        const auto& [spacing, origin] = sight::viz::scene3d::utils::convertSpacingAndOrigin(image.get_shared());
        m_plane->update(m_orientation, spacing, origin, m_enableAlpha);

        // Update threshold if necessary
        this->updateTF();

        this->updateShaderSliceIndexParameter();

        this->requestRender();
    }
}

//------------------------------------------------------------------------------

void negato2d::changeSliceIndex(int _axial_index, int _frontal_index, int _sagittal_index)
{
    const auto image = m_image.lock();

    this->getRenderService()->makeCurrent();

    auto img_size = image->size();

    // Sometimes, the image can contain only one slice,
    // it results into a division by 0 when the range is transformed between [0-1].
    // So we increase the image size to 2 to divide by 1.
    img_size[0] = img_size[0] == 1 ? 2 : img_size[0];
    img_size[1] = img_size[1] == 1 ? 2 : img_size[1];
    img_size[2] = img_size[2] == 1 ? 2 : img_size[2];

    m_currentSliceIndex = {
        static_cast<float>(_sagittal_index) / (static_cast<float>(img_size[0] - 1)),
        static_cast<float>(_frontal_index) / (static_cast<float>(img_size[1] - 1)),
        static_cast<float>(_axial_index) / (static_cast<float>(img_size[2] - 1))
    };

    this->updateShaderSliceIndexParameter();

    m_sliceIndexChangedSig->emit();

    this->requestRender();
}

//------------------------------------------------------------------------------

void negato2d::updateSlicesFromWorld(double _x, double _y, double _z)
{
    const auto image = m_image.lock();

    Ogre::Vector3 point = {static_cast<float>(_x), static_cast<float>(_y), static_cast<float>(_z)};
    Ogre::Vector3i slice_idx;
    try
    {
        slice_idx = sight::viz::scene3d::utils::worldToSlices(*image, point);
    }
    catch(core::exception& e)
    {
        SIGHT_WARN("Cannot update slice index: " << e.what());
        return;
    }

    const auto sig = image->signal<data::image::SliceIndexModifiedSignalType>
                         (data::image::SLICE_INDEX_MODIFIED_SIG);

    sig->async_emit(slice_idx[2], slice_idx[1], slice_idx[0]);
}

//------------------------------------------------------------------------------

void negato2d::updateShaderSliceIndexParameter()
{
    m_plane->changeSlice(m_currentSliceIndex);
}

//------------------------------------------------------------------------------

void negato2d::updateTF()
{
    this->getRenderService()->makeCurrent();

    m_gpuTF->update();

    // Sends the TF texture to the negato-related passes
    m_plane->setTFData(*m_gpuTF);

    this->requestRender();
}

//------------------------------------------------------------------------------

void negato2d::mouseMoveEvent(MouseButton _button, Modifier /*_mods*/, int _x, int _y, int /*_dx*/, int /*_dy*/)
{
    if(m_picked)
    {
        if(_button == MouseButton::LEFT)
        {
            this->pickIntensity(_x, _y);
        }

        this->getLayer()->cancelFurtherInteraction();
    }
}

//------------------------------------------------------------------------------

void negato2d::buttonPressEvent(MouseButton _button, Modifier /*_mods*/, int _x, int _y)
{
    m_pickingCross->setVisible(false);
    if(_button == MouseButton::LEFT)
    {
        this->pickIntensity(_x, _y);
    }

    if(m_picked)
    {
        this->getLayer()->cancelFurtherInteraction();
    }
}

//------------------------------------------------------------------------------

void negato2d::buttonReleaseEvent(MouseButton /*_button*/, Modifier /*_mods*/, int /*_x*/, int /*_y*/)
{
    m_picked = false;
    m_pickingCross->setVisible(false);
    m_pickedVoxelSignal->async_emit("");
}

//-----------------------------------------------------------------------------

service::connections_t negato2d::auto_connections() const
{
    return {
        {s_IMAGE_IN, data::image::MODIFIED_SIG, service::slots::UPDATE},
        {s_IMAGE_IN, data::image::BUFFER_MODIFIED_SIG, service::slots::UPDATE},
        {s_IMAGE_IN, data::image::SLICE_TYPE_MODIFIED_SIG, SLICETYPE_SLOT},
        {s_IMAGE_IN, data::image::SLICE_INDEX_MODIFIED_SIG, SLICEINDEX_SLOT},
        {s_TF_IN, data::transfer_function::MODIFIED_SIG, UPDATE_TF_SLOT},
        {s_TF_IN, data::transfer_function::POINTS_MODIFIED_SIG, UPDATE_TF_SLOT},
        {s_TF_IN, data::transfer_function::WINDOWING_MODIFIED_SIG, UPDATE_TF_SLOT}
    };
}

//------------------------------------------------------------------------------
void negato2d::setVisible(bool _visible)
{
    if(m_negatoSceneNode != nullptr)
    {
        m_negatoSceneNode->setVisible(_visible);

        this->requestRender();
    }
}

//------------------------------------------------------------------------------

void negato2d::pickIntensity(int _x, int _y)
{
    Ogre::SceneManager* sm = this->getSceneManager();

    const auto result = sight::viz::scene3d::utils::pickObject(_x, _y, Ogre::SceneManager::ENTITY_TYPE_MASK, *sm);

    if(result.has_value())
    {
        if(m_plane->getMovableObject() == result->first)
        {
            m_picked = true;
            const auto image = m_image.lock();

            if(!data::helper::medical_image::check_image_validity(image.get_shared()))
            {
                return;
            }

            const auto image_buffer_lock = image->dump_lock();

            const auto [spacing, origin] = sight::viz::scene3d::utils::convertSpacingAndOrigin(image.get_shared());
            auto cross_lines = m_plane->computeCross(result->second, origin);
            m_pickingCross->update(cross_lines[0], cross_lines[1], cross_lines[2], cross_lines[3]);

            const auto picking_text = sight::viz::scene3d::utils::pickImage(*image, result->second, origin, spacing);
            m_pickedVoxelSignal->async_emit(picking_text);

            this->requestRender();
        }
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::module::viz::scene3d::adaptor.

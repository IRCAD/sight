/************************************************************************
 *
 * Copyright (C) 2016-2023 IRCAD France
 * Copyright (C) 2016-2020 IHU Strasbourg
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

#pragma once

#include "viz/scene3d/config.hpp"

#include <data/image.hpp>
#include <data/transfer_function.hpp>

#include <viz/scene3d/texture.hpp>
#include <viz/scene3d/transfer_function.hpp>

#include <OGRE/OgreSceneManager.h>
#include <OGRE/OgreTexture.h>

namespace sight::viz::scene3d::vr
{

/**
 * @brief Summed area table of a 3D image.
 */
class VIZ_SCENE3D_CLASS_API summed_area_table
{
public:

    /// Constructor, creates an SAT with the given resolution.
    VIZ_SCENE3D_API summed_area_table(
        std::string _parent_id,
        Ogre::SceneManager* _scene_manager,
        float _size_ratio = 0.25F
    );

    /// Destructor, does nothing.
    VIZ_SCENE3D_API ~summed_area_table();

    /// Computes the SAT sequentially on the CPU based on the given image and TF.
    VIZ_SCENE3D_API void computeSequential(data::image::sptr _image, data::transfer_function::sptr _tf);

    /// Computes the SAT using Hensley's recursive doubling algorithm.
    VIZ_SCENE3D_API void computeParallel(
        const texture::sptr& _img_texture,
        const viz::scene3d::transfer_function::sptr& _gpu_tf,
        float _sample_distance
    );

    /// Returns the texture holding the SAT.
    [[nodiscard]] VIZ_SCENE3D_API Ogre::TexturePtr getTexture() const;

    /// Returns the texture used as a ping-pong buffer during SAT computation allowing it to be repurposed.
    [[nodiscard]] VIZ_SCENE3D_API Ogre::TexturePtr getSpareTexture() const;

    /// Updates the current size of the image according to the passed texture and updates the SAT
    VIZ_SCENE3D_API void updateSatFromTexture(const texture::sptr& _img_texture);

    /// Updates the SAT size ratio and updates the SAT.
    VIZ_SCENE3D_API void updateSatFromRatio(float _size_ratio);

private:

    class summed_area_tableInitCompositorListener;
    class summed_area_tableCompositorListener;

    /// Internal wrapper keeping trace of the current listeners in use.
    struct listeners_t
    {
        /// SAT init listener.
        summed_area_tableInitCompositorListener* init = nullptr;

        /// SAT table update listener.
        summed_area_tableCompositorListener* table = nullptr;
    };

    /// Creates the buffers and initializes the SAT.
    void updateBuffers();

    listeners_t m_listeners {};

    /// SAT size ratio used to computes its resolution.
    float m_satSizeRatio;

    /// SAT resolution. It's computed thanks to the configured ratio and the associated image size.
    data::image::Size m_satSize;

    /// Current image size used to resize the SAT in case of a ratio change.
    data::image::Size m_currentImageSize;

    /// texture used as source during SAT GPU computation, holds the result at the end.
    Ogre::TexturePtr m_sourceBuffer {nullptr};

    /// texture used as target during SAT GPU computation.
    Ogre::TexturePtr m_targetBuffer {nullptr};

    /// Prefix used to name the buffers.
    std::string m_parentId;

    /// scene manager.
    Ogre::SceneManager* m_sceneManager;

    /// Camera used as a viewport for each slice of the SAT buffers.
    Ogre::Camera* m_dummyCamera {nullptr};

    /// The pass orientation, horizontal = 0, vertical = 1, z-wise = 2.
    int m_passOrientation {};

    /// The index of the slice to which we currently render.
    std::size_t m_sliceIndex {};

    /// The read offset based on the number of reads per fragment shader (r) and the pass index (i) : m_readOffset =
    /// r^i.
    int m_readOffset {};

    /// The depth of the current slice.
    float m_currentSliceDepth {};

    /// Number of texture reads per pass. A higher number will result in fewer passes.
    /// /!\ This number must be the same as the one used in the fragment shader.
    static constexpr int s_nbTextReads = 32;

    /// Resource name of the source buffer.
    static inline const std::string SOURCE_BUFFER_NAME = "__GPU_summed_area_table_Ping";

    // Resource name of the target buffer.
    static inline const std::string TARGET_BUFFER_NAME = "__GPU_summed_area_table_Pong";
};

//-----------------------------------------------------------------------------

inline Ogre::TexturePtr summed_area_table::getTexture() const
{
    return m_sourceBuffer;
}

//-----------------------------------------------------------------------------

inline Ogre::TexturePtr summed_area_table::getSpareTexture() const
{
    return m_targetBuffer;
}

//-----------------------------------------------------------------------------

} // namespace sight::viz::scene3d::vr

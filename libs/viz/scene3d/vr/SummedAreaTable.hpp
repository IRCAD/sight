/************************************************************************
 *
 * Copyright (C) 2016-2021 IRCAD France
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

#include <data/Image.hpp>
#include <data/TransferFunction.hpp>

#include <glm/glm.hpp>

#include <viz/scene3d/TransferFunction.hpp>

#include <OGRE/OgreSceneManager.h>
#include <OGRE/OgreTexture.h>

namespace sight::viz::scene3d
{

namespace vr
{

/**
 * @brief Summed area table of a 3D image.
 */
class VIZ_SCENE3D_CLASS_API SummedAreaTable
{
public:

    /// Constructor, creates an SAT with the given resolution.
    VIZ_SCENE3D_API SummedAreaTable(
        std::string _parentId,
        ::Ogre::SceneManager* _sceneManager,
        float _sizeRatio = 0.25f
    );

    /// Destructor, does nothing.
    VIZ_SCENE3D_API ~SummedAreaTable();

    /// Computes the SAT sequentially on the CPU based on the given image and TF.
    VIZ_SCENE3D_API void computeSequential(data::Image::sptr _image, data::TransferFunction::sptr _tf);

    /// Computes the SAT using Hensley's recursive doubling algorithm.
    VIZ_SCENE3D_API void computeParallel(
        ::Ogre::TexturePtr _imgTexture,
        const viz::scene3d::TransferFunction::sptr& _gpuTf,
        float _sampleDistance
    );

    /// Returns the texture holding the SAT.
    VIZ_SCENE3D_API ::Ogre::TexturePtr getTexture() const;

    /// Returns the texture used as a ping-pong buffer during SAT computation allowing it to be repurposed.
    VIZ_SCENE3D_API ::Ogre::TexturePtr getSpareTexture() const;

    /// Updates the current size of the image according to the passed texture and updates the SAT
    VIZ_SCENE3D_API void updateSatFromTexture(::Ogre::TexturePtr _imgTexture);

    /// Updates the SAT size ratio and updates the SAT.
    VIZ_SCENE3D_API void updateSatFromRatio(float _sizeRatio);

private:

    /// Creates the buffers and initializes the SAT
    void initializeSAT();

    /// SAT size ratio used to computes its resolution.
    float m_satSizeRatio;

    /// SAT resolution. It's computed thanks to the configured ratio and the associated image size.
    data::Image::Size m_satSize;

    /// Current image size used to resize the SAT in case of a ratio change.
    data::Image::Size m_currentImageSize;

    /// Texture used as source during SAT GPU computation, holds the result at the end.
    ::Ogre::TexturePtr m_sourceBuffer;

    /// Texture used as target during SAT GPU computation.
    ::Ogre::TexturePtr m_targetBuffer;

    /// Prefix used to name the buffers.
    std::string m_parentId;

    /// Scene manager.
    ::Ogre::SceneManager* m_sceneManager;

    /// Camera used as a viewport for each slice of the SAT buffers.
    ::Ogre::Camera* m_dummyCamera;

    /// The pass orientation, horizontal = 0, vertical = 1, z-wise = 2.
    int m_passOrientation;

    /// The index of the slice to which we currently render.
    size_t m_sliceIndex;

    /// The read offset based on the number of reads per fragment shader (r) and the pass index (i) : m_readOffset =
    /// r^i.
    int m_readOffset;

    /// The depth of the current slice.
    float m_currentSliceDepth;

    /// Number of texture reads per pass. A higher number will result in fewer passes.
    /// /!\ This number must be the same as the one used in the FS.
    const int m_nbTextReads = 32;

    /// Returns the voxel colour after TF application.
    ::glm::vec4 applyTf(data::TransferFunction::sptr _tf, int16_t imgValue);

    /// Returns the SAT value at position (x, y, z).
    ::glm::vec4 getSatValue(glm::vec4* satBuffer, int x, int y, int z);

    /// Sets the SAT value at position (x, y, z).
    void setSatValue(glm::vec4* satBuffer, ::glm::vec4 value, int x, int y, int z);

    /// Resource name of the source buffer.
    const std::string SOURCE_BUFFER_NAME = "__GPU_SummedAreaTable_Ping";

    // Resource name of the target buffer.
    const std::string TARGET_BUFFER_NAME = "__GPU_SummedAreaTable_Pong";
};

//-----------------------------------------------------------------------------

inline Ogre::TexturePtr SummedAreaTable::getTexture() const
{
    return m_sourceBuffer;
}

//-----------------------------------------------------------------------------

inline Ogre::TexturePtr SummedAreaTable::getSpareTexture() const
{
    return m_targetBuffer;
}

//-----------------------------------------------------------------------------

} // namespace vr

} // namespace sight::viz::scene3d

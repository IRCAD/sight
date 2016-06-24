#ifndef __FWRENDEROGRE_SUMMEDAREATABLE_HPP__
#define __FWRENDEROGRE_SUMMEDAREATABLE_HPP__

#include <fwData/Image.hpp>
#include <fwData/TransferFunction.hpp>

#include <fwComEd/helper/Image.hpp>

#include "fwRenderOgre/config.hpp"

#include <glm/glm.hpp>

#include <OGRE/OgreSceneManager.h>
#include <OGRE/OgreTexture.h>

namespace fwRenderOgre
{

class FWRENDEROGRE_CLASS_API SummedAreaTable
{
public:

    FWRENDEROGRE_API SummedAreaTable(::Ogre::SceneManager *_sceneManager, int width, int height, int depth);

    FWRENDEROGRE_API ~SummedAreaTable();

    FWRENDEROGRE_API void computeSequential(::fwData::Image::sptr _image, fwData::TransferFunction::sptr _tf);

    FWRENDEROGRE_API void computeParallel(::Ogre::TexturePtr _imgTexture, ::Ogre::TexturePtr _gpuTf);

    FWRENDEROGRE_API ::Ogre::TexturePtr getTexture() const;

private:

    ::fwData::Image::SizeType m_satSize;

    ::glm::vec4 *m_satBuffer;

    ::Ogre::TexturePtr m_sourceBuffer, m_targetBuffer;

    ::Ogre::TexturePtr m_gpuSummedAreaTable;

    ::Ogre::SceneManager *m_sceneManager;

    int m_passOrientation;

    int m_sliceIndex;

    int m_readOffset;

    float m_currentSliceDepth;

    const int m_nbTextReads = 32;

    ::glm::vec4 applyTf(fwData::TransferFunction::sptr _tf, int16_t imgValue);

    ::glm::vec4 getSatValue(int x, int y, int z);

    void setSatValue(::glm::vec4 value, int x, int y, int z);

};

//-----------------------------------------------------------------------------

inline Ogre::TexturePtr SummedAreaTable::getTexture() const
{
    return m_gpuSummedAreaTable;
}

//-----------------------------------------------------------------------------

}

#endif

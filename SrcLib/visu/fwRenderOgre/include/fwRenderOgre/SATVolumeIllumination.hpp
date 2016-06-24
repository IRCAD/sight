#ifndef __FWRENDEROGRE_SATVOLUMEILLUMINATION_HPP__
#define __FWRENDEROGRE_SATVOLUMEILLUMINATION_HPP__

#include "fwRenderOgre/config.hpp"
#include "fwRenderOgre/SummedAreaTable.hpp"


namespace fwRenderOgre
{

/**
 * @brief Computes the illumination volume for extinction based shading.
 */
class FWRENDEROGRE_CLASS_API SATVolumeIllumination
{
public:

    /// Initializes the illumination volume with the give dimensions.
    FWRENDEROGRE_API SATVolumeIllumination(std::string parentId, ::Ogre::SceneManager *sceneManager, int width, int height, int depth);

    /// Destructor, does nothing.
    FWRENDEROGRE_API ~SATVolumeIllumination();

    /// Recomputes the illumination volume, can be called when the image or the TF changed.
    FWRENDEROGRE_API void updateVolIllum(::Ogre::TexturePtr _img, ::Ogre::TexturePtr _tf);

private:

    /// SAT used to compute the illumination volume.
    ::fwRenderOgre::SummedAreaTable m_sat;

    /// Texture holding the illumination volume.
    ::Ogre::TexturePtr m_illuminationVolume;

    /// The index of the slice to which we currently render.
    int m_currentSliceIndex;
};

}

#endif // __FWRENDEROGRE_SATVOLUMEILLUMINATION_HPP__

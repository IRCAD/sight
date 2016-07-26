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
    FWRENDEROGRE_API SATVolumeIllumination(std::string parentId, ::Ogre::SceneManager *sceneManager,
                                           float satSizeRatio = 0.25f, int nbShells = 3, int shellRadius = 3);

    /// Destructor, does nothing.
    FWRENDEROGRE_API ~SATVolumeIllumination();

    /// Computes a new SAT with a different resolution given by the size ratio.
    FWRENDEROGRE_API void updateSAT(float _satSizeRatio);

    /// Recomputes the illumination volume, can be called when the image or the TF changed.
    FWRENDEROGRE_API void updateVolIllum(::Ogre::TexturePtr _img, ::Ogre::TexturePtr _tf);

    /// Illumination volume texture getter
    FWRENDEROGRE_API ::Ogre::TexturePtr getIlluminationVolume();

private:

    /// SAT used to compute the illumination volume.
    ::fwRenderOgre::SummedAreaTable m_sat;

    /// Texture holding the illumination volume.
    ::Ogre::TexturePtr m_illuminationVolume;

    /// The index of the slice to which we currently render.
    int m_currentSliceIndex;

    /// Number of shells used to compute the volume illumination from the SAT.
    int m_nbShells;

    /// Radius of the shells used to compute the volume illumination from the SAT.
    int m_shellRadius;
};

//-----------------------------------------------------------------------------
// Inline method(s)

inline ::Ogre::TexturePtr SATVolumeIllumination::getIlluminationVolume()
{
    return m_illuminationVolume;
}

//-----------------------------------------------------------------------------

}

#endif // __FWRENDEROGRE_SATVOLUMEILLUMINATION_HPP__

#ifndef __PREINTEGRATIONTABLE_HPP__
#define __PREINTEGRATIONTABLE_HPP__

#include <fwData/Image.hpp>
#include <fwData/TransferFunction.hpp>

#include <glm/glm.hpp>

#include <OGRE/OgreTexture.h>

#include "fwRenderOgre/config.hpp"

namespace fwRenderOgre
{

/**
 * @brief Class used to compute the table for pre-integrated rendering.
 * @see Engel 2001 : High-Quality Pre-Integrated Volume Rendering Using Hardware-Accelerated Pixel Shading
 */
class PreIntegrationTable
{
public:

    /// Constructor, does nothing.
    FWRENDEROGRE_API PreIntegrationTable();

    /// Destructor, does nothing.
    FWRENDEROGRE_API ~PreIntegrationTable();

    /// Initializes the texture resource.
    FWRENDEROGRE_API void createTexture(const std::string& _parentId);

    /// Sets the table resolution based on the new image and recomputes it.
    FWRENDEROGRE_API void imageUpdate(const ::fwData::Image::sptr& _img, const ::fwData::TransferFunction::sptr& _tf, unsigned _nbSamples);

    /// Computes the table based on the given TF and slice distance.
    FWRENDEROGRE_API void tfUpdate(const ::fwData::TransferFunction::sptr& _tf, unsigned _nbSamples);

    /// Get the texture holding the pre-integration table.
    FWRENDEROGRE_API inline ::Ogre::TexturePtr getTexture() const;

    /// Returns the image value interval.
    FWRENDEROGRE_API inline std::pair<int, int> getMinMax() const;

private:

    /// Defines a texture pixel.
    struct TablePixel
    {
        uint8_t b;
        uint8_t g;
        uint8_t r;
        uint8_t a;
    };

    /// Pixel structure used when computing the table.
    typedef ::glm::dvec4 IntegralPixel;

    /// Array storing table values.
    TablePixel *m_table;

    /// Transfer function integral.
    IntegralPixel *m_integralTable;

    /// Table's GPU texture.
    ::Ogre::TexturePtr m_tableTexture;

    /// Size of the longest view ray inside the volume.
    double m_volumeDiagonalSize;

    /// Image value interval.
    std::pair<int, int> m_valueInterval;

    /// Texture resolution.
    unsigned m_textureSize;

};

//-----------------------------------------------------------------------------

::Ogre::TexturePtr PreIntegrationTable::getTexture() const
{
    return m_tableTexture;
}

//-----------------------------------------------------------------------------

 std::pair<int, int> PreIntegrationTable::getMinMax() const
 {
    return m_valueInterval;
 }

//-----------------------------------------------------------------------------

} // namespace fwRenderOgre

#endif // __PREINTEGRATIONTABLE_HPP__

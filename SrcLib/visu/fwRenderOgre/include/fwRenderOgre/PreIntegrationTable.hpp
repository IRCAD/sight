#ifndef __PREINTEGRATIONTABLE_HPP__
#define __PREINTEGRATIONTABLE_HPP__

#include <fwData/Image.hpp>
#include <fwData/TransferFunction.hpp>

#include <OGRE/OgreTexture.h>

#include "fwRenderOgre/config.hpp"

namespace fwRenderOgre
{

class PreIntegrationTable
{
public:

    FWRENDEROGRE_API PreIntegrationTable();

    FWRENDEROGRE_API ~PreIntegrationTable();

    FWRENDEROGRE_API void createTexture(const std::string& _parentId);

    FWRENDEROGRE_API void imageUpdate(const ::fwData::Image::sptr& _img, const ::fwData::TransferFunction::sptr& _tf, unsigned _nbSamples);

    FWRENDEROGRE_API void tfUpdate(const ::fwData::TransferFunction::sptr& _tf, unsigned _nbSamples);

    FWRENDEROGRE_API inline ::Ogre::TexturePtr getTexture() const;

    FWRENDEROGRE_API inline std::pair<int, int> getMinMax() const;

private:

    struct TablePixel
    {
        uint8_t b;
        uint8_t g;
        uint8_t r;
        uint8_t a;
    };

    struct IntegralPixel
    {
        double a;
        double r;
        double g;
        double b;
    };

    TablePixel *m_table;

    IntegralPixel *m_integralTable;

    ::Ogre::TexturePtr m_tableTexture;

    double m_volumeDiagonalSize;

    std::pair<int, int> m_valueInterval;

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

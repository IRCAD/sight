#include "fwRenderOgre/PreIntegrationTable.hpp"

#include <algorithm>

#include <boost/algorithm/clamp.hpp>

#include <fwComEd/helper/Image.hpp>
#include <fwMemory/BufferManager.hpp>
#include <fwMemory/BufferObject.hpp>
#include <fwRenderOgre/Utils.hpp>

#include <OGRE/OgreHardwarePixelBuffer.h>
#include <OGRE/OgreTextureManager.h>

namespace fwRenderOgre
{

//-----------------------------------------------------------------------------

PreIntegrationTable::PreIntegrationTable() :
    m_table(nullptr),
    m_integralTable(nullptr),
    m_textureSize(0)
{

}

//-----------------------------------------------------------------------------

PreIntegrationTable::~PreIntegrationTable()
{

}

//-----------------------------------------------------------------------------

void PreIntegrationTable::createTexture(const std::string &_parentId)
{
    m_tableTexture = ::Ogre::TextureManager::getSingletonPtr()->create(
        _parentId + "_PreIntTableTexture",
        ::Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
        true);
}

//-----------------------------------------------------------------------------

void PreIntegrationTable::imageUpdate(const fwData::Image::sptr& _img, const ::fwData::TransferFunction::sptr& _tf, unsigned _nbSamples)
{
    ::Ogre::PixelFormat pixelFormat = ::fwRenderOgre::Utils::getPixelFormatOgre(_img);

    ::fwComEd::helper::Image imgHelper(_img);

    size_t nbPixels = _img->getDataArray()->getNumberOfElements();

    switch(pixelFormat)
    {
    case ::Ogre::PF_L8: //uint8
    {
        uint8_t *ucharImgBuffer = static_cast<uint8_t *>(imgHelper.getBuffer());
        auto minMax = std::minmax_element(ucharImgBuffer, ucharImgBuffer + nbPixels);

        m_valueInterval.first  = *minMax.first;
        m_valueInterval.second = *minMax.second;

        break;
    }
    case ::Ogre::PF_L16: //int16
    {
        int16_t *ushortImgBuffer = static_cast<int16_t *>(imgHelper.getBuffer());
        auto minMax = std::minmax_element(ushortImgBuffer, ushortImgBuffer + nbPixels);

        m_valueInterval.first  = *minMax.first;
        m_valueInterval.second = *minMax.second;

        break;
    }
    default:
        OSLM_FATAL("Invalid pixel format for pre-integration, pixels must be integers");
    }

    ::fwData::Image::SpacingType imgSpacing;
    ::fwData::Image::SizeType imgSize;

    imgSpacing = _img->getSpacing();
    imgSize = _img->getSize();

    double imgDimensions[3];

    for(int i = 0; i < 3; ++ i)
    {
        imgDimensions[i] = static_cast<double>(imgSize[i]) * imgSpacing[i];
    }

    m_volumeDiagonalSize = std::sqrt(imgDimensions[0] * imgDimensions[0] +
                                     imgDimensions[1] * imgDimensions[1] +
                                     imgDimensions[2] * imgDimensions[2]);

    unsigned textureSize = m_valueInterval.second - m_valueInterval.first;

    if(textureSize != m_textureSize)
    {
        m_textureSize = textureSize;

        if(m_table)
        {
            delete m_table;
            delete m_integralTable;
        }

        m_table = new TablePixel[m_textureSize * m_textureSize];
        m_integralTable = new IntegralPixel[m_textureSize];

        Utils::allocateTexture(m_tableTexture.get(), m_textureSize, m_textureSize, 1, ::Ogre::PF_A8R8G8B8, ::Ogre::TEX_TYPE_2D, true);

        tfUpdate(_tf, _nbSamples);
    }
}

//-----------------------------------------------------------------------------

void PreIntegrationTable::tfUpdate(const fwData::TransferFunction::sptr& _tf, unsigned _nbSamples)
{

    if(!m_table)
    {
        return;
    }

    const ::fwData::TransferFunction::TFValuePairType intensityMinMax = _tf->getWLMinMax();

    const ::fwData::TransferFunction::TFValuePairType tfMinMax = _tf->getMinMaxTFValues();

    const double invWindow = 1./_tf->getWindow();

    IntegralPixel tmp;
    std::memset(&tmp, 0, sizeof tmp);

    _tf->setIsClamped(false);

    for(int k = 0; k < (int)m_textureSize; ++ k)
    {
        ::fwData::TransferFunction::TFValueType value = k + m_valueInterval.first;

        value = (value - intensityMinMax.first) * (tfMinMax.second - tfMinMax.first) * invWindow + tfMinMax.first;

        ::fwData::TransferFunction::TFColor interpolatedColor = _tf->getInterpolatedColor(value);

        double alpha = interpolatedColor.a;

        tmp.r += interpolatedColor.r;
        tmp.g += interpolatedColor.g;
        tmp.b += interpolatedColor.b;
        tmp.a += alpha;

        m_integralTable[k] = tmp;
    }

    const double distanceBetweenSamples = m_volumeDiagonalSize / static_cast<double>(_nbSamples);

    for(int sb = 0; sb < (int)m_textureSize; ++ sb)
    {
#pragma omp parallel for
        for(int sf = 0; sf < (int)m_textureSize; ++ sf)
        {

            IntegralPixel res;
            std::memset(&res, 0, sizeof res);

            const double d =  distanceBetweenSamples  / static_cast<double>(sb - sf);

            if(sb != sf)
            {
                res.a = 1. - std::exp( -d * (m_integralTable[sb].a - m_integralTable[sf].a));

                res.r = d * (m_integralTable[sb].r - m_integralTable[sf].r);
                res.g = d * (m_integralTable[sb].g - m_integralTable[sf].g);
                res.b = d * (m_integralTable[sb].b - m_integralTable[sf].b);
            }
            else
            {
                ::fwData::TransferFunction::TFValueType value = sb + m_valueInterval.first;

                // intensity --> transfer function
                value = (value - intensityMinMax.first) * (tfMinMax.second - tfMinMax.first) * invWindow + tfMinMax.first;

                ::fwData::TransferFunction::TFColor interpolatedColor = _tf->getInterpolatedColor(value);

                res.a = interpolatedColor.a;
                res.r = interpolatedColor.r;
                res.g = interpolatedColor.g;
                res.b = interpolatedColor.b;
            }

            res.r = boost::algorithm::clamp(res.r, 0., 1.);
            res.g = boost::algorithm::clamp(res.g, 0., 1.);
            res.b = boost::algorithm::clamp(res.b, 0., 1.);
            res.a = boost::algorithm::clamp(res.a, 0., 1.);

            m_table[sb * m_textureSize + sf] = (TablePixel) {
                    static_cast<uint8_t>(res.b * 255.),
                    static_cast<uint8_t>(res.g * 255.),
                    static_cast<uint8_t>(res.r * 255.),
                    static_cast<uint8_t>(res.a * 255.)
            };
        }
    }

    // Store table in texture buffer.
    ::Ogre::HardwarePixelBufferSharedPtr pixBuffer = m_tableTexture->getBuffer();

    // Discards the entire buffer while locking so that we can easily refill it from scratch
    pixBuffer->lock(::Ogre::HardwareBuffer::HBL_DISCARD);
    ::Ogre::PixelBox pixBox = pixBuffer->getCurrentLock();
    std::uint8_t* pDest = static_cast<std::uint8_t*>(pixBox.data);

    std::memcpy(pDest, m_table, m_textureSize * m_textureSize * sizeof(TablePixel));

    pixBuffer->unlock();
}

//-----------------------------------------------------------------------------

} // namespace fwRenderOgre

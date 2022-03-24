/************************************************************************
 *
 * Copyright (C) 2018-2022 IRCAD France
 * Copyright (C) 2018-2021 IHU Strasbourg
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

#include "filter/image/MinMaxPropagation.hpp"

#include <core/tools/Dispatcher.hpp>

#include <data/helper/MedicalImage.hpp>
#include <data/Image.hpp>

#include <io/itk/itk.hpp>

#include <itkFloodFilledImageFunctionConditionalIterator.h>
#include <itkImage.h>
#include <itkImageFunction.hxx>

#include <algorithm>
#include <numeric>

namespace sight::filter::image
{

template<typename TImage>
class MinMaxPropagCriterion : public itk::ImageFunction<TImage,
                                                        bool>
{
public:

    typedef MinMaxPropagCriterion Self;
    typedef itk::ImageFunction<TImage, bool> Superclass;
    typedef itk::SmartPointer<Self> Pointer;
    typedef itk::SmartPointer<Self const> ConstPointer;

    itkNewMacro(Self);
    itkTypeMacro(MinMaxPropagCriterion, ImageFunction);

    typedef typename Superclass::InputPixelType PixelType;
    typedef typename TImage::IndexType IndexType;
    typedef typename TImage::SpacingType SpacingType;

    //-----------------------------------------------------------------------------

    MinMaxPropagCriterion() :
        m_min(itk::NumericTraits<PixelType>::max()),
        m_max(itk::NumericTraits<PixelType>::min()),
        m_useRadius(false),
        m_radius(std::numeric_limits<double>::infinity()),
        m_overwrite(false)
    {
    }

    //-----------------------------------------------------------------------------

    void setParams(
        data::Image::csptr roi,
        const std::vector<IndexType>& seeds,
        double radius,
        bool overwrite,
        MinMaxPropagation::Mode mode
)
    {
        std::vector<PixelType> seedValues(seeds.size());
        std::transform(
            seeds.begin(),
            seeds.end(),
            seedValues.begin(),
            [this](const IndexType index){return this->GetInputImage()->GetPixel(index);});

        if(!seedValues.empty())
        {
            if(mode == MinMaxPropagation::MIN || mode == MinMaxPropagation::MINMAX)
            {
                m_min = *std::min_element(seedValues.begin(), seedValues.end());
            }
            else
            {
                m_min = itk::NumericTraits<PixelType>::min();
            }

            if(mode == MinMaxPropagation::MAX || mode == MinMaxPropagation::MINMAX)
            {
                m_max = *std::max_element(seedValues.begin(), seedValues.end());
            }
            else
            {
                m_max = itk::NumericTraits<PixelType>::max();
            }
        }

        // Radius is irrelevant if it's greater than the image diagonal.
        const SpacingType& imgSpacing           = this->GetInputImage()->GetSpacing();
        const typename TImage::SizeType imgSize = this->GetInputImage()->GetLargestPossibleRegion().GetSize();

        const double r2 = radius * radius;

        double distance2 = 0;
        for(typename TImage::IndexValueType i = 0 ; i < IndexType::Dimension ; ++i)
        {
            const double realDim = imgSize[i] * imgSpacing[i];
            distance2 += realDim * realDim;
        }

        m_useRadius = (r2 <= distance2);

        m_roi       = roi;
        m_radius    = radius;
        m_seeds     = seeds;
        m_overwrite = overwrite;
    }

    //-----------------------------------------------------------------------------

    bool Evaluate(const typename Superclass::PointType& point) const override
    {
        IndexType index;
        this->ConvertPointToNearestIndex(point, index);
        return EvaluateAtIndex(index);
    }

    //-----------------------------------------------------------------------------

    bool EvaluateAtIndex(const IndexType& index) const override
    {
        const PixelType& currentValue = this->GetInputImage()->GetPixel(index);

        // Check if the value is in the range.
        if(currentValue < m_min || currentValue > m_max)
        {
            return false;
        }

        // Check if writing is allowed.
        if(!m_overwrite && currentValue != itk::NumericTraits<PixelType>::Zero)
        {
            return false;
        }

        // Check if the pixel is inside the roi.
        if(m_roi != nullptr && !isInROI(index))
        {
            return false;
        }

        // Check if the max distance was reached
        if(m_useRadius && !isInsideRadius(index))
        {
            return false;
        }

        return true;
    }

    //-----------------------------------------------------------------------------

    bool EvaluateAtContinuousIndex(const typename Superclass::ContinuousIndexType& contIndex) const override
    {
        IndexType index;
        this->ConvertContinuousIndexToNearestIndex(contIndex, index);
        return EvaluateAtIndex(index);
    }

    //-----------------------------------------------------------------------------

    bool isInsideRadius(const IndexType& index) const
    {
        const SpacingType& imgSpacing = this->GetInputImage()->GetSpacing();

        const double r2 = m_radius * m_radius;

        for(const auto& seed : m_seeds)
        {
            double distance2 = 0.;

            for(typename TImage::IndexValueType i = 0 ; i < IndexType::Dimension ; ++i)
            {
                const double distTmp = (index[i] - seed[i]) * imgSpacing[i];
                distance2 += distTmp * distTmp;
            }

            if(distance2 < r2)
            {
                return true;
            }
        }

        return false;
    }

    //-----------------------------------------------------------------------------

    bool isInROI(const IndexType& index) const
    {
        const auto dumpLock = m_roi->dump_lock();
        const auto size     = m_roi->getSize();

        const data::Image::BufferType* roiVal =
            reinterpret_cast<const data::Image::BufferType*>(
                m_roi->getPixel(index[0] + index[1] * size[0] + index[2] * size[0] * size[1]));

        return !data::helper::MedicalImage::isBufNull(roiVal, m_roi->getType().size());
    }

private:

    data::Image::csptr m_roi;

    PixelType m_min;

    PixelType m_max;

    std::vector<IndexType> m_seeds;

    bool m_useRadius;

    double m_radius;

    bool m_overwrite;
};

//-----------------------------------------------------------------------------

struct MinMaxPropagator
{
    struct Parameters
    {
        data::Image::csptr inputImage;
        data::Image::sptr outputImage;
        data::Image::csptr roi;
        ImageDiff diff;
        data::Image::BufferType* value;
        MinMaxPropagation::SeedsType seeds;
        double radius;
        bool overwrite;
        MinMaxPropagation::Mode mode;
    };

    //------------------------------------------------------------------------------

    template<class PIXELTYPE>
    void operator()(Parameters& params)
    {
        typedef typename itk::Image<PIXELTYPE, 3> ImageType;
        typedef MinMaxPropagCriterion<ImageType> CriterionType;

        const typename ImageType::Pointer itkImage = io::itk::moveToItk<ImageType>(params.inputImage);

        std::vector<typename ImageType::IndexType> itkSeeds;
        for(const auto& seed : params.seeds)
        {
            typename ImageType::IndexType index;

            for(typename ImageType::IndexValueType i = 0 ; i < ImageType::IndexType::Dimension ; ++i)
            {
                index[i] = static_cast<typename ImageType::IndexValueType>(seed[i]);
            }

            itkSeeds.push_back(index);
        }

        typename CriterionType::Pointer criterion = CriterionType::New();
        criterion->SetInputImage(itkImage);
        criterion->setParams(params.roi, itkSeeds, params.radius, params.overwrite, params.mode);

        itk::FloodFilledImageFunctionConditionalIterator<ImageType, CriterionType> iter(
            itkImage, criterion, itkSeeds);

        const auto dumpLock = params.outputImage->dump_lock();

        const std::uint8_t outImgPixelSize = params.outputImage->getType().size()
                                             * static_cast<std::uint8_t>(params.outputImage->numComponents());

        for( ; !iter.IsAtEnd() ; ++iter)
        {
            const typename ImageType::IndexType currentIndex = iter.GetIndex();

            const std::size_t bufferIndex = static_cast<std::size_t>(itkImage->ComputeOffset(currentIndex));

            const data::Image::BufferType* pixBuf =
                reinterpret_cast<data::Image::BufferType*>(params.outputImage->getPixel(bufferIndex));

            if(!std::equal(pixBuf, pixBuf + outImgPixelSize, params.value))
            {
                params.diff.addDiff(bufferIndex, pixBuf, params.value);
                params.outputImage->setPixel(bufferIndex, params.value);
            }
        }
    }
};

//-----------------------------------------------------------------------------

MinMaxPropagation::MinMaxPropagation(
    const data::Image::csptr& inImage,
    const data::Image::sptr& outImage,
    const data::Image::csptr& roi
) :
    m_inImage(inImage),
    m_roi(roi),
    m_outImage(outImage)
{
}

//-----------------------------------------------------------------------------

ImageDiff MinMaxPropagation::propagate(
    SeedsType& seeds,
    data::Image::BufferType* value,
    const double radius,
    const bool overwrite,
    const Mode mode
)
{
    const core::Type type               = m_inImage->getType();
    const std::size_t outImagePixelSize = m_outImage->getType().size() * m_outImage->numComponents();

    MinMaxPropagator::Parameters params;
    params.inputImage  = m_inImage;
    params.outputImage = m_outImage;
    params.roi         = m_roi;
    params.diff        = ImageDiff(outImagePixelSize);
    params.seeds       = seeds;
    params.value       = value;
    params.overwrite   = overwrite;
    params.mode        = mode;
    params.radius      = radius;

    core::tools::Dispatcher<core::tools::SupportedDispatcherTypes, MinMaxPropagator>::invoke(type, params);

    m_seeds.clear();

    return params.diff;
}

//-----------------------------------------------------------------------------

} // namespace sight::filter::image.

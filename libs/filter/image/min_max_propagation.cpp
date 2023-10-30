/************************************************************************
 *
 * Copyright (C) 2018-2023 IRCAD France
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

// cspell:ignore NOLINT NOLINTNEXTLINE

#include "filter/image/min_max_propagation.hpp"

#include <core/tools/dispatcher.hpp>

#include <data/helper/medical_image.hpp>
#include <data/image.hpp>

#include <io/itk/itk.hpp>

#include <itkFloodFilledImageFunctionConditionalIterator.h>
#include <itkImage.h>
#include <itkImageFunction.hxx>

#include <algorithm>
#include <numeric>

namespace sight::filter::image
{

template<typename TImage>
class min_max_propag_criterion : public itk::ImageFunction<TImage,
                                                           bool>
{
public:

    using self_t       = min_max_propag_criterion;
    using superclass_t = itk::ImageFunction<TImage, bool>;
    using Pointer      = itk::SmartPointer<self_t>;

    itkNewMacro(self_t);
    itkTypeMacro(MinMaxPropagCriterion, ImageFunction);

    using pixel_t   = typename superclass_t::InputPixelType;
    using index_t   = typename TImage::IndexType;
    using spacing_t = typename TImage::SpacingType;

    //-----------------------------------------------------------------------------

    min_max_propag_criterion() :
        m_min(itk::NumericTraits<pixel_t>::max()),
        m_max(itk::NumericTraits<pixel_t>::min()),
        m_radius(std::numeric_limits<double>::infinity())
    {
    }

    //-----------------------------------------------------------------------------

    void set_params(
        data::image::csptr _roi,
        const std::vector<index_t>& _seeds,
        double _radius,
        bool _overwrite,
        min_max_propagation::mode _mode
)
    {
        std::vector<pixel_t> seed_values(_seeds.size());
        std::transform(
            _seeds.begin(),
            _seeds.end(),
            seed_values.begin(),
            [this](const index_t _index){return this->GetInputImage()->GetPixel(_index);});

        if(!seed_values.empty())
        {
            if(_mode == min_max_propagation::min || _mode == min_max_propagation::minmax)
            {
                m_min = *std::min_element(seed_values.begin(), seed_values.end());
            }
            else
            {
                m_min = itk::NumericTraits<pixel_t>::min();
            }

            if(_mode == min_max_propagation::max || _mode == min_max_propagation::minmax)
            {
                m_max = *std::max_element(seed_values.begin(), seed_values.end());
            }
            else
            {
                m_max = itk::NumericTraits<pixel_t>::max();
            }
        }

        // Radius is irrelevant if it's greater than the image diagonal.
        const spacing_t& img_spacing = this->GetInputImage()->GetSpacing();
        const auto img_size          = this->GetInputImage()->GetLargestPossibleRegion().GetSize();

        const double r2 = _radius * _radius;

        double distance2 = 0;
        for(typename TImage::IndexValueType i = 0 ; i < index_t::Dimension ; ++i)
        {
            const double real_dim = double(img_size[std::uint32_t(i)]) * img_spacing[i];
            distance2 += real_dim * real_dim;
        }

        m_use_radius = (r2 <= distance2);

        m_roi       = _roi;
        m_radius    = _radius;
        m_seeds     = _seeds;
        m_overwrite = _overwrite;
    }

    //-----------------------------------------------------------------------------

    bool Evaluate(const typename superclass_t::PointType& point) const override // NOLINT(readability-identifier-naming)
    {
        index_t index;
        this->ConvertPointToNearestIndex(point, index);
        return EvaluateAtIndex(index);
    }

    //-----------------------------------------------------------------------------

    bool EvaluateAtIndex(const index_t& index) const override // NOLINT(readability-identifier-naming)
    {
        const pixel_t& current_value = this->GetInputImage()->GetPixel(index);

        // Check if the value is in the range.
        if(current_value < m_min || current_value > m_max)
        {
            return false;
        }

        // Check if writing is allowed.
        if(!m_overwrite && current_value != itk::NumericTraits<pixel_t>::Zero)
        {
            return false;
        }

        // Check if the pixel is inside the roi.
        if(m_roi != nullptr && !is_in_roi(index))
        {
            return false;
        }

        // Check if the max distance was reached
        return !(m_use_radius && !is_inside_radius(index));
    }

    //-----------------------------------------------------------------------------

    // NOLINTNEXTLINE(readability-identifier-naming)
    bool EvaluateAtContinuousIndex(const typename superclass_t::ContinuousIndexType& contIndex) const override
    {
        index_t index;
        this->ConvertContinuousIndexToNearestIndex(contIndex, index);
        return EvaluateAtIndex(index);
    }

    //-----------------------------------------------------------------------------

    bool is_inside_radius(const index_t& _index) const
    {
        const spacing_t& img_spacing = this->GetInputImage()->GetSpacing();

        const double r2 = m_radius * m_radius;

        for(const auto& seed : m_seeds)
        {
            double distance2 = 0.;

            for(typename TImage::IndexValueType i = 0 ; i < index_t::Dimension ; ++i)
            {
                const double dist_tmp = double(_index[std::uint32_t(i)] - seed[std::uint32_t(i)])
                                        * img_spacing[std::uint32_t(i)];
                distance2 += dist_tmp * dist_tmp;
            }

            if(distance2 < r2)
            {
                return true;
            }
        }

        return false;
    }

    //-----------------------------------------------------------------------------

    bool is_in_roi(const index_t& _index) const
    {
        const auto dump_lock = m_roi->dump_lock();
        const auto size      = m_roi->size();

        const auto* roi_val =
            reinterpret_cast<const data::image::buffer_t*>(
                m_roi->get_pixel(
                    std::size_t(_index[0]) + std::size_t(_index[1]) * size[0] + std::size_t(_index[2])
                    * size[0] * size[1]
                )
            );

        return !data::helper::medical_image::is_buf_null(roi_val, std::uint32_t(m_roi->type().size()));
    }

private:

    data::image::csptr m_roi;

    pixel_t m_min;

    pixel_t m_max;

    std::vector<index_t> m_seeds;

    bool m_use_radius {false};

    double m_radius;

    bool m_overwrite {false};
};

//-----------------------------------------------------------------------------

struct min_max_propagator
{
    struct parameters
    {
        data::image::csptr input_image;
        data::image::sptr output_image;
        data::image::csptr roi;
        image_diff diff;
        data::image::buffer_t* value {};
        min_max_propagation::seeds_t seeds;
        double radius {};
        bool overwrite {};
        min_max_propagation::mode mode {min_max_propagation::mode::minmax};
    };

    //------------------------------------------------------------------------------

    template<class PIXELTYPE>
    void operator()(parameters& _params)
    {
        using image_t     = typename itk::Image<PIXELTYPE, 3>;
        using criterion_t = min_max_propag_criterion<image_t>;

        const typename image_t::Pointer itk_image = io::itk::move_to_itk<image_t>(_params.input_image);

        std::vector<typename image_t::IndexType> itk_seeds;
        for(const auto& seed : _params.seeds)
        {
            typename image_t::IndexType index;

            for(typename image_t::IndexValueType i = 0 ; i < image_t::IndexType::Dimension ; ++i)
            {
                index[std::uint32_t(i)] = typename image_t::IndexValueType(seed[std::size_t(i)]);
            }

            itk_seeds.push_back(index);
        }

        typename criterion_t::Pointer criterion = criterion_t::New();
        criterion->SetInputImage(itk_image);
        criterion->set_params(_params.roi, itk_seeds, _params.radius, _params.overwrite, _params.mode);

        itk::FloodFilledImageFunctionConditionalIterator<image_t, criterion_t> iter(
            itk_image, criterion, itk_seeds);

        const auto dump_lock = _params.output_image->dump_lock();

        const std::uint8_t out_img_pixel_size = std::uint8_t(
            _params.output_image->type().size()
            * _params.output_image->num_components()
        );

        for( ; !iter.IsAtEnd() ; ++iter)
        {
            const typename image_t::IndexType current_index = iter.GetIndex();

            const auto buffer_index = static_cast<std::size_t>(itk_image->ComputeOffset(current_index));

            const data::image::buffer_t* pix_buf =
                reinterpret_cast<data::image::buffer_t*>(_params.output_image->get_pixel(buffer_index));

            if(!std::equal(pix_buf, pix_buf + out_img_pixel_size, _params.value))
            {
                _params.diff.add_diff(buffer_index, pix_buf, _params.value);
                _params.output_image->set_pixel(buffer_index, _params.value);
            }
        }
    }
};

//-----------------------------------------------------------------------------

min_max_propagation::min_max_propagation(
    data::image::csptr _in_image,
    data::image::sptr _out_image,
    data::image::csptr _roi
) :
    m_in_image(std::move(_in_image)),
    m_roi(std::move(_roi)),
    m_out_image(std::move(_out_image))
{
}

//-----------------------------------------------------------------------------

image_diff min_max_propagation::propagate(
    seeds_t& _seeds,
    data::image::buffer_t* _value,
    const double _radius,
    const bool _overwrite,
    const mode _mode
)
{
    const core::type type                  = m_in_image->type();
    const std::size_t out_image_pixel_size = m_out_image->type().size() * m_out_image->num_components();

    min_max_propagator::parameters params;
    params.input_image  = m_in_image;
    params.output_image = m_out_image;
    params.roi          = m_roi;
    params.diff         = image_diff(out_image_pixel_size);
    params.seeds        = _seeds;
    params.value        = _value;
    params.overwrite    = _overwrite;
    params.mode         = _mode;
    params.radius       = _radius;

    core::tools::dispatcher<core::tools::supported_dispatcher_types, min_max_propagator>::invoke(type, params);

    m_seeds.clear();

    return params.diff;
}

//-----------------------------------------------------------------------------

} // namespace sight::filter::image.

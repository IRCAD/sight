/************************************************************************
 *
 * Copyright (C) 2018-2024 IRCAD France
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

#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/mean.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/variance.hpp>

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
        m_sqr_radius(std::numeric_limits<double>::infinity())
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
        std::ranges::transform(
            _seeds,
            seed_values.begin(),
            [this](const index_t& _index){return this->GetInputImage()->GetPixel(_index);});

        if(!seed_values.empty())
        {
            if(_mode == min_max_propagation::stddev)
            {
                namespace boost_acc = boost::accumulators;
                boost_acc::accumulator_set<double,
                                           boost_acc::stats<boost_acc::tag::variance, boost_acc::tag::mean> > var_mean;

                std::ranges::for_each(seed_values, [&](const auto& _x){var_mean(static_cast<double>(_x));});

                const auto mean               = boost_acc::mean(var_mean);
                const auto standard_deviation = std::sqrt(boost_acc::variance(var_mean));
                m_min = static_cast<pixel_t>(mean - standard_deviation);
                m_max = static_cast<pixel_t>(mean + standard_deviation);
            }
            else
            {
                if(_mode == min_max_propagation::min || _mode == min_max_propagation::minmax)
                {
                    m_min = *std::ranges::min_element(seed_values);
                }
                else
                {
                    m_min = itk::NumericTraits<pixel_t>::min();
                }

                if(_mode == min_max_propagation::max || _mode == min_max_propagation::minmax)
                {
                    m_max = *std::ranges::max_element(seed_values);
                }
                else
                {
                    m_max = itk::NumericTraits<pixel_t>::max();
                }
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

        m_roi        = _roi;
        m_sqr_radius = r2;
        m_seeds      = _seeds;
        m_overwrite  = _overwrite;
        m_spacing    = img_spacing;
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
        for(const auto& seed : m_seeds)
        {
            double distance2 = 0.;

            for(typename TImage::IndexValueType i = 0 ; i < index_t::Dimension ; ++i)
            {
                const double dist_tmp = double(_index[std::uint32_t(i)] - seed[std::uint32_t(i)])
                                        * m_spacing[std::uint32_t(i)];
                distance2 += dist_tmp * dist_tmp;
            }

            if(distance2 < m_sqr_radius)
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

    double m_sqr_radius;
    bool m_use_radius {false};
    bool m_overwrite {false};
    spacing_t m_spacing;
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
        image_diff out_diff;
        std::uint8_t value {};
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

        itk::FloodFilledImageFunctionConditionalIterator<image_t, criterion_t> iter(itk_image, criterion, itk_seeds);

        const auto dump_lock = _params.output_image->dump_lock();

        {
            const auto* in_buf          = static_cast<const data::image::buffer_t*>(_params.input_image->buffer());
            const std::size_t in_stride = _params.input_image->type().size() * _params.input_image->num_components();

            auto* out_buf                = static_cast<uint8_t*>(_params.output_image->buffer());
            const std::size_t out_stride = _params.output_image->num_components();

            for( ; !iter.IsAtEnd() ; ++iter)
            {
                const typename image_t::IndexType current_index = iter.GetIndex();

                const auto buffer_index = static_cast<std::size_t>(itk_image->ComputeOffset(current_index));

                auto* out_pix_buf = out_buf + buffer_index * out_stride;
                if(*out_pix_buf != _params.value)
                {
                    _params.out_diff.add_diff(buffer_index, out_pix_buf, &_params.value);

                    const auto* pix_buf = in_buf + in_stride * buffer_index;
                    _params.diff.add_diff(buffer_index, pix_buf, &_params.value);

                    *out_pix_buf = _params.value;
                }
            }
        }
    }
};

//-----------------------------------------------------------------------------

image_diff min_max_propagation::process(
    data::image::csptr _in_image,
    data::image::sptr _out_image,
    data::image::csptr _roi,
    seeds_t& _seeds,
    std::uint8_t _value,
    const double _radius,
    const bool _overwrite,
    const mode _mode
)
{
    const core::type type                  = _in_image->type();
    const std::size_t in_image_pixel_size  = _in_image->type().size() * _in_image->num_components();
    const std::size_t out_image_pixel_size = _out_image->type().size() * _out_image->num_components();

    min_max_propagator::parameters params;
    params.input_image  = _in_image;
    params.output_image = _out_image;
    params.roi          = _roi;
    params.diff         = image_diff(in_image_pixel_size);
    params.out_diff     = image_diff(out_image_pixel_size);
    params.seeds        = _seeds;
    params.value        = _value;
    params.overwrite    = _overwrite;
    params.mode         = _mode;
    params.radius       = _radius;

    core::tools::dispatcher<core::tools::supported_dispatcher_types, min_max_propagator>::invoke(type, params);

    return params.diff;
}

//-----------------------------------------------------------------------------

} // namespace sight::filter::image.

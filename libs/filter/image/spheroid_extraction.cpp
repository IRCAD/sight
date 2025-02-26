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

#if HAS_ITK_REVIEW_MODULE

#include "filter/image/spheroid_extraction.hpp"

#include <core/tools/dispatcher.hpp>

#include <io/itk/itk.hpp>

#include <itkBinaryThresholdImageFilter.h>
#include <itkConnectedComponentImageFilter.h>
#include <itkImage.h>
#include <itkLabelGeometryImageFilter.h>

namespace sight::filter::image
{

struct spheroid_extractor
{
    struct parameters
    {
        data::image::csptr input_image;
        data::point_list::sptr output_point_list;
        double threshold {};
        double radius_min {};
        double radius_max {};
        double elongation_min {};
        double elongation_max {};
    };

    //------------------------------------------------------------------------------

    template<class PIXELTYPE>
    void operator()(parameters& _params)
    {
        using image_t        = typename itk::Image<PIXELTYPE, 3>;
        using binary_image_t = typename itk::Image<std::uint16_t, 3>;

        typename image_t::Pointer input_image = io::itk::move_to_itk<image_t>(_params.input_image);

        typename itk::BinaryThresholdImageFilter<image_t, binary_image_t>::Pointer threshold_filter =
            itk::BinaryThresholdImageFilter<image_t, binary_image_t>::New();

        auto threshold = PIXELTYPE(_params.threshold);
        threshold_filter->SetLowerThreshold(threshold);
        threshold_filter->SetUpperThreshold(threshold * PIXELTYPE(10));
        threshold_filter->SetInsideValue(255);
        threshold_filter->SetOutsideValue(0);
        threshold_filter->SetInput(input_image);

        threshold_filter->Update();

        binary_image_t::Pointer binary_image =
            threshold_filter->GetOutput();
        itk::ConnectedComponentImageFilter<binary_image_t, binary_image_t>::Pointer cc =
            itk::ConnectedComponentImageFilter<binary_image_t, binary_image_t>::New();

        cc->SetInput(threshold_filter->GetOutput());
        cc->FullyConnectedOn();
        cc->Update();

        using label_stats_filter_t = itk::LabelGeometryImageFilter<binary_image_t, image_t>;
        typename label_stats_filter_t::Pointer label_geometry_filter = label_stats_filter_t::New();

        label_geometry_filter->SetInput(cc->GetOutput());
        label_geometry_filter->SetIntensityInput(input_image);
        label_geometry_filter->Update();

        auto labels = label_geometry_filter->GetLabels();

        SIGHT_DEBUG("Number of labels : " << label_geometry_filter->GetNumberOfLabels());

        for(auto labels_it = labels.begin() ; labels_it != labels.end() ; ++labels_it)
        {
            auto label_value = *labels_it;

            const double radius_mean     = (_params.radius_max + _params.radius_min) / 2.;
            const double mean_elongation = (_params.elongation_max + _params.elongation_min) / 2.;
            const double radius_std      = (_params.radius_max - _params.radius_min) / 6.;
            const double elongation_std  = (_params.elongation_max - _params.elongation_min) / 6.;

            const double e         = label_geometry_filter->GetElongation(label_value);
            const auto axes_length = label_geometry_filter->GetAxesLength(label_value);

            SIGHT_DEBUG("Elongation" << e);
            SIGHT_DEBUG("Axes length" << axes_length);

            const double ge  = std::pow(e - mean_elongation, 2.) / std::pow(elongation_std, 2.);
            const double gr0 = std::pow(axes_length[0] - radius_mean, 2.) / std::pow(radius_std, 2.);
            const double gr1 = std::pow(axes_length[1] - radius_mean, 2.) / std::pow(radius_std, 2.);
            const double gr2 = std::pow(axes_length[2] - radius_mean, 2.) / std::pow(radius_std, 2.);

            const double mahalanobis_distance = std::sqrt(ge + gr0 + gr1 + gr2);

            SIGHT_DEBUG("Mahalanobis distance : " << mahalanobis_distance);

            // Empirical value. You shouldn't have to change it.
            // If you believe the detection wrongly failed/succeeded, please consider changing your radius
            // and elongation ranges before modifying this threshold.
            const double criteria_threshold = 15.;

            if(mahalanobis_distance < criteria_threshold)
            {
                const auto centroid_point = label_geometry_filter->GetCentroid(label_value);

                std::array<double, 3> real_point_coords {};
                for(std::uint8_t i = 0 ; i < 3 ; ++i)
                {
                    real_point_coords[i] = double(centroid_point[i]) * input_image->GetSpacing()[i]
                                           + input_image->GetOrigin()[i];
                }

                data::point::sptr point = std::make_shared<data::point>(real_point_coords);
                _params.output_point_list->get_points().push_back(point);
            }
        }
    }
};

//------------------------------------------------------------------------------

data::point_list::sptr spheroid_extraction::extract(
    const data::image::csptr& _image,
    const double _threshold,
    const double _radius_min,
    const double _radius_max,
    const double _elongation_min,
    const double _elongation_max
)
{
    data::point_list::sptr output_point_list = std::make_shared<data::point_list>();

    spheroid_extractor::parameters params;
    params.input_image       = _image;
    params.output_point_list = output_point_list;
    params.threshold         = _threshold;
    params.radius_min        = _radius_min;
    params.radius_max        = _radius_max;
    params.elongation_min    = _elongation_min;
    params.elongation_max    = _elongation_max;

    core::tools::dispatcher<core::tools::supported_dispatcher_types, spheroid_extractor>::invoke(
        _image->type(),
        params
    );

    return output_point_list;
}

} // namespace sight::filter::image.

#endif // ITK_VERSION_MAJOR == 4 || (ITK_VERSION_MAJOR >= 5 && ITK_VERSION_MINOR >= 3)

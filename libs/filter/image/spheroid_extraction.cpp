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

struct SpheroidExtractor
{
    struct Parameters
    {
        data::image::csptr inputImage;
        data::point_list::sptr outputPointList;
        double threshold {};
        double radiusMin {};
        double radiusMax {};
        double elongationMin {};
        double elongationMax {};
    };

    //------------------------------------------------------------------------------

    template<class PIXELTYPE>
    void operator()(Parameters& params)
    {
        using image_t        = typename itk::Image<PIXELTYPE, 3>;
        using binary_image_t = typename itk::Image<std::uint16_t, 3>;

        typename image_t::Pointer inputImage = io::itk::move_to_itk<image_t>(params.inputImage);

        typename itk::BinaryThresholdImageFilter<image_t, binary_image_t>::Pointer thresholdFilter =
            itk::BinaryThresholdImageFilter<image_t, binary_image_t>::New();

        auto threshold = PIXELTYPE(params.threshold);
        thresholdFilter->SetLowerThreshold(threshold);
        thresholdFilter->SetUpperThreshold(threshold * PIXELTYPE(10));
        thresholdFilter->SetInsideValue(255);
        thresholdFilter->SetOutsideValue(0);
        thresholdFilter->SetInput(inputImage);

        thresholdFilter->Update();

        binary_image_t::Pointer binaryImage =
            thresholdFilter->GetOutput();
        itk::ConnectedComponentImageFilter<binary_image_t, binary_image_t>::Pointer cc =
            itk::ConnectedComponentImageFilter<binary_image_t, binary_image_t>::New();

        cc->set_input(thresholdFilter->GetOutput());
        cc->FullyConnectedOn();
        cc->Update();

        using label_stats_filter_t = itk::LabelGeometryImageFilter<binary_image_t, image_t>;
        typename label_stats_filter_t::Pointer labelGeometryFilter = label_stats_filter_t::New();

        labelGeometryFilter->SetInput(cc->GetOutput());
        labelGeometryFilter->SetIntensityInput(inputImage);
        labelGeometryFilter->Update();

        typename label_stats_filter_t::labels_t labels = labelGeometryFilter->GetLabels();

        SIGHT_DEBUG("Number of labels : " << labelGeometryFilter->GetNumberOfLabels());

        typename label_stats_filter_t::labels_t::iterator labelsIt;
        for(labelsIt = labels.begin() ; labelsIt != labels.end() ; ++labelsIt)
        {
            typename label_stats_filter_t::label_pixel_t labelValue = *labelsIt;

            const double radiusMean     = (params.radiusMax + params.radiusMin) / 2.;
            const double meanElongation = (params.elongationMax + params.elongationMin) / 2.;
            const double radiusSTD      = (params.radiusMax - params.radiusMin) / 6.;
            const double elongationSTD  = (params.elongationMax - params.elongationMin) / 6.;

            const double e        = labelGeometryFilter->GetElongation(labelValue);
            const auto axesLength = labelGeometryFilter->GetAxesLength(labelValue);

            SIGHT_DEBUG("Elongation" << e);
            SIGHT_DEBUG("Axes length" << axesLength);

            const double ge  = std::pow(e - meanElongation, 2.) / std::pow(elongationSTD, 2.);
            const double gr0 = std::pow(axesLength[0] - radiusMean, 2.) / std::pow(radiusSTD, 2.);
            const double gr1 = std::pow(axesLength[1] - radiusMean, 2.) / std::pow(radiusSTD, 2.);
            const double gr2 = std::pow(axesLength[2] - radiusMean, 2.) / std::pow(radiusSTD, 2.);

            const double mahalanobisDistance = std::sqrt(ge + gr0 + gr1 + gr2);

            SIGHT_DEBUG("Mahalanobis distance : " << mahalanobisDistance);

            // Empirical value. You shouldn't have to change it.
            // If you believe the detection wrongly failed/succeeded, please consider changing your radius
            // and elongation ranges before modifying this threshold.
            const double criteriaThreshold = 15.;

            if(mahalanobisDistance < criteriaThreshold)
            {
                const auto centroidPoint = labelGeometryFilter->GetCentroid(labelValue);

                std::array<double, 3> realPointCoords {};
                for(std::uint8_t i = 0 ; i < 3 ; ++i)
                {
                    realPointCoords[i] = double(centroidPoint[i]) * inputImage->GetSpacing()[i]
                                         + inputImage->GetOrigin()[i];
                }

                data::point::sptr point = std::make_shared<data::point>(realPointCoords);
                params.outputPointList->getPoints().push_back(point);
            }
        }
    }
};

//------------------------------------------------------------------------------

data::point_list::sptr spheroid_extraction::extract(
    const data::image::csptr& _image,
    const double _threshold,
    const double _radiusMin,
    const double _radiusMax,
    const double _elongationMin,
    const double _elongationMax
)
{
    data::point_list::sptr outputPointList = std::make_shared<data::point_list>();

    SpheroidExtractor::Parameters params;
    params.inputImage      = _image;
    params.outputPointList = outputPointList;
    params.threshold       = _threshold;
    params.radiusMin       = _radiusMin;
    params.radiusMax       = _radiusMax;
    params.elongationMin   = _elongationMin;
    params.elongationMax   = _elongationMax;

    core::tools::dispatcher<core::tools::supported_dispatcher_types, SpheroidExtractor>::invoke(
        _image->getType(),
        params
    );

    return outputPointList;
}

} // namespace sight::filter::image.

#endif // ITK_VERSION_MAJOR == 4 || (ITK_VERSION_MAJOR >= 5 && ITK_VERSION_MINOR >= 3)

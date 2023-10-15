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

#include "filter/image/labeling.hpp"

#include "filter/image/filters.hpp"
#include "filter/image/filters.hxx"

#include <core/com/signal.hpp>
#include <core/com/signal.hxx>
#include <core/com/signals.hpp>
#include <core/tools/dispatcher.hpp>

#include <data/boolean.hpp>
#include <data/helper/MedicalImage.hpp>
#include <data/image.hpp>
#include <data/string.hpp>

#include <itkLabelImageToShapeLabelMapFilter.h>

namespace sight::filter::image
{

//------------------------------------------------------------------------------

struct labelingFilter
{
    struct Parameters
    {
        data::image::sptr m_inputImage;
        data::image::sptr m_outputImage;
        unsigned int m_numLabels {};
    };

    //------------------------------------------------------------------------------

    template<class PIXELTYPE>
    void operator()(Parameters& params)
    {
        using ImageType       = itk::Image<PIXELTYPE, 3>;
        using BinaryImageType = itk::Image<std::uint8_t, 3>;
        typename ImageType::Pointer itkImage;
        itkImage = io::itk::moveToItk<ImageType>(params.m_inputImage);

        BinaryImageType::Pointer out;
        out = filter::image::labeling<PIXELTYPE, 3>(itkImage, params.m_numLabels);

        io::itk::moveFromItk<BinaryImageType>(out, params.m_outputImage);
    }
};

//------------------------------------------------------------------------------

data::image::sptr labeling(data::image::sptr image, unsigned int numLabels)
{
    data::image::sptr outputImage = std::make_shared<data::image>();

    labelingFilter::Parameters params;
    params.m_inputImage  = image;
    params.m_outputImage = outputImage;
    params.m_numLabels   = numLabels;

    const core::type type = image->getType();
    core::tools::dispatcher<core::tools::supported_dispatcher_types, labelingFilter>::invoke(type, params);

    // Notify image
    data::object::ModifiedSignalType::sptr sig;
    sig = outputImage->signal<data::object::ModifiedSignalType>(data::object::MODIFIED_SIG);
    sig->async_emit();

    return outputImage;
}

//------------------------------------------------------------------------------

// Gets centroids of shapes from a labelled image and adds them as a 'landmark' field to it.
struct LabelImageFilter
{
    struct Parameters
    {
        data::image::sptr i_image;
        std::vector<data::point_list::sptr> i_lPointListCentroids;
        std::vector<std::vector<std::size_t> > i_lPointListLabels;
    };
    //------------------------------------------------------------------------------

    template<class PIXELTYPE>
    void operator()(Parameters& params)
    {
        data::image::sptr image      = params.i_image;
        const unsigned int dimension = 3;
        SIGHT_ASSERT("Only image dimension 3 managed.", image->numDimensions() == dimension);
        using InputImageType = typename itk::Image<PIXELTYPE, dimension>;
        typename InputImageType::Pointer itkInputImage = io::itk::moveToItk<InputImageType>(image);

        using LabelType            = PIXELTYPE;
        using OutputImageType      = itk::Image<LabelType, dimension>;
        using ShapeLabelObjectType = itk::ShapeLabelObject<LabelType, dimension>;
        using LabelMapType         = itk::LabelMap<ShapeLabelObjectType>;

        // Extract shapes
        using I2LType = typename itk::LabelImageToShapeLabelMapFilter<OutputImageType, LabelMapType>;

        typename I2LType::Pointer i2l = I2LType::New();
        i2l->SetInput(itkInputImage);
        i2l->SetComputePerimeter(true);
        i2l->SetBackgroundValue(0);
        i2l->Update();

        // If we have clusters in XML file
        if(!params.i_lPointListCentroids.empty() && !params.i_lPointListLabels.empty())
        {
            LabelMapType* labelMap = i2l->GetOutput();
            data::point::sptr newPoint;

            for(unsigned int n = 1 ; n <= labelMap->GetNumberOfLabelObjects() ; ++n)
            {
                std::vector<std::size_t> findPlanes;
                std::size_t plane = 0;
                for(plane = 0 ; plane < params.i_lPointListLabels.size() ; ++plane)
                {
                    std::vector<std::size_t> currentPlane = params.i_lPointListLabels.at(plane);

                    for(std::size_t labelInPlane : currentPlane)
                    {
                        if(labelInPlane == n)
                        {
                            findPlanes.push_back(plane);
                        }
                    }
                }

                if(!findPlanes.empty())
                {
                    // We need to get the 'n-1'th object because of the '0' background value (1st object = '1' label
                    // value)
                    ShapeLabelObjectType* labelObject = labelMap->GetNthLabelObject(n - 1);

                    // append to landmark
                    const typename ShapeLabelObjectType::CentroidType centroid = labelObject->GetCentroid();

                    newPoint = std::make_shared<data::point>(centroid[0], centroid[1], centroid[2]);

                    for(std::size_t findPlane : findPlanes)
                    {
                        data::point_list::sptr planePointList =
                            params.i_lPointListCentroids.at(findPlane);

                        // append to point the label
                        std::stringstream labelName;
                        labelName << n;
                        data::string::sptr label = std::make_shared<data::string>(labelName.str());

                        planePointList->getPoints().push_back(newPoint);
                    }
                }
            }
        }
        // Default behaviour (put point in field of the image)
        else
        {
            //get landmarks
            data::point_list::sptr landmarks = data::helper::MedicalImage::getLandmarks(*image);

            SIGHT_ASSERT("landmarks not instanced", landmarks);
            landmarks->getPoints().clear();

            LabelMapType* labelMap = i2l->GetOutput();
            data::point::sptr newPoint;
            for(unsigned int n = 0 ; n < labelMap->GetNumberOfLabelObjects() ; ++n)
            {
                ShapeLabelObjectType* labelObject = labelMap->GetNthLabelObject(n);

                // append to landmark
                const typename ShapeLabelObjectType::CentroidType centroid = labelObject->GetCentroid();

                newPoint = std::make_shared<data::point>(centroid[0], centroid[1], centroid[2]);
                landmarks->getPoints().push_back(newPoint);

                // append to point the label
                std::stringstream labelName;
                labelName << n;
                newPoint->setLabel(labelName.str());
            }

            data::helper::MedicalImage::setLandmarksVisibility(*image, true);
        }
    }
};

//------------------------------------------------------------------------------

void computeCentroids(
    data::image::sptr image,
    std::vector<data::point_list::sptr> pointListCentroids,
    std::vector<std::vector<std::size_t> > pointListLabels
)
{
    // Preparing the parameters for ITK
    LabelImageFilter::Parameters params;
    params.i_image = image;

    if(!pointListCentroids.empty())
    {
        params.i_lPointListCentroids = pointListCentroids;
        params.i_lPointListLabels    = pointListLabels;
    }

    // Call the ITK operator
    const core::type type = image->getType();
    core::tools::dispatcher<core::tools::supported_dispatcher_types, LabelImageFilter>::invoke(type, params);
}

} // namespace sight::filter::image.

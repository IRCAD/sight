/************************************************************************
 *
 * Copyright (C) 2018-2021 IRCAD France
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

#include "filter/image/Labeling.hpp"

#include "filter/image/filters.hpp"
#include "filter/image/filters.hxx"

#include <core/com/Signal.hpp>
#include <core/com/Signal.hxx>
#include <core/com/Signals.hpp>
#include <core/tools/Dispatcher.hpp>
#include <core/tools/TypeKeyTypeMapping.hpp>

#include <data/Boolean.hpp>
#include <data/fieldHelper/Image.hpp>
#include <data/fieldHelper/MedicalImageHelpers.hpp>
#include <data/Image.hpp>
#include <data/String.hpp>

#include <itkLabelImageToShapeLabelMapFilter.h>

namespace sight::filter::image
{

//------------------------------------------------------------------------------

struct LabelingFilter
{
    struct Parameters
    {
        data::Image::sptr m_inputImage;
        data::Image::sptr m_outputImage;
        unsigned int m_numLabels;
    };

    //------------------------------------------------------------------------------

    template<class PIXELTYPE>
    void operator()(Parameters& params)
    {
        typedef itk::Image<PIXELTYPE, 3> ImageType;
        typedef itk::Image<std::uint8_t, 3> BinaryImageType;
        typename ImageType::Pointer itkImage;
        itkImage = io::itk::moveToItk<ImageType>(params.m_inputImage);

        BinaryImageType::Pointer out;
        out = filter::image::labeling<PIXELTYPE, 3>(itkImage, params.m_numLabels);

        io::itk::moveFromItk<BinaryImageType>(out, params.m_outputImage);
    }
};

//------------------------------------------------------------------------------

data::Image::sptr labeling(data::Image::sptr image, unsigned int numLabels)
{
    data::Image::sptr outputImage = data::Image::New();

    LabelingFilter::Parameters params;
    params.m_inputImage  = image;
    params.m_outputImage = outputImage;
    params.m_numLabels   = numLabels;

    const core::tools::Type type = image->getType();
    core::tools::Dispatcher<core::tools::SupportedDispatcherTypes, LabelingFilter>::invoke(type, params);

    // Notify image
    data::Object::ModifiedSignalType::sptr sig;
    sig = outputImage->signal<data::Object::ModifiedSignalType>(data::Object::s_MODIFIED_SIG);
    sig->asyncEmit();

    return outputImage;
}

//------------------------------------------------------------------------------

// Gets centroids of shapes from a labelised image and adds them as a 'landmark' field to it.
struct LabelImageFilter
{
    struct Parameters
    {
        data::Image::sptr i_image;
        std::vector<data::PointList::sptr> i_lPointListCentroids;
        std::vector<std::vector<size_t> > i_lPointListLabels;
    };
    //------------------------------------------------------------------------------

    template<class PIXELTYPE>
    void operator()(Parameters& params)
    {
        data::Image::sptr image      = params.i_image;
        const unsigned int dimension = 3;
        SIGHT_ASSERT("Only image dimension 3 managed.", image->getNumberOfDimensions() == dimension);
        typedef typename ::itk::Image<PIXELTYPE, dimension> InputImageType;
        typename InputImageType::Pointer itkInputImage = io::itk::moveToItk<InputImageType>(image);

        typedef PIXELTYPE LabelType;
        typedef ::itk::Image<LabelType, dimension> OutputImageType;
        typedef ::itk::ShapeLabelObject<LabelType, dimension> ShapeLabelObjectType;
        typedef ::itk::LabelMap<ShapeLabelObjectType> LabelMapType;

        // Extract shapes
        typedef typename ::itk::LabelImageToShapeLabelMapFilter<OutputImageType, LabelMapType> I2LType;

        typename I2LType::Pointer i2l = I2LType::New();
        i2l->SetInput(itkInputImage);
        i2l->SetComputePerimeter(true);
        i2l->SetBackgroundValue(0);
        i2l->Update();

        // If we have clusters in XML file
        if(!params.i_lPointListCentroids.empty() && !params.i_lPointListLabels.empty())
        {
            LabelMapType* labelMap = i2l->GetOutput();
            data::Point::sptr newPoint;

            for(unsigned int n = 1 ; n <= labelMap->GetNumberOfLabelObjects() ; ++n)
            {
                std::vector<size_t> findPlanes;
                size_t plane;
                for(plane = 0 ; plane < params.i_lPointListLabels.size() ; ++plane)
                {
                    std::vector<size_t> currentPlane = params.i_lPointListLabels.at(plane);

                    for(size_t labelInPlane = 0 ; labelInPlane < currentPlane.size() ; ++labelInPlane)
                    {
                        if(currentPlane.at(labelInPlane) == n)
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

                    newPoint = data::Point::New(centroid[0], centroid[1], centroid[2]);

                    for(size_t idFindPlanes = 0 ; idFindPlanes < findPlanes.size() ; ++idFindPlanes)
                    {
                        data::PointList::sptr planePointList =
                            params.i_lPointListCentroids.at(findPlanes.at(idFindPlanes));

                        // append to point the label
                        std::stringstream labelName;
                        labelName << n;
                        data::String::sptr label = data::String::New(labelName.str());
                        //newPoint->setField( data::fieldHelper::Image::m_labelId, label );

                        planePointList->getPoints().push_back(newPoint);
                    }
                }
            }
        }
        // Default behaviour (put point in field of the image)
        else
        {
            //get landmarks
            data::fieldHelper::MedicalImageHelpers::checkLandmarks(image);
            data::PointList::sptr landmarks =
                image->getField<data::PointList>(data::fieldHelper::Image::m_imageLandmarksId);

            SIGHT_ASSERT("landmarks not instanced", landmarks);
            landmarks->getPoints().clear();

            LabelMapType* labelMap = i2l->GetOutput();
            data::Point::sptr newPoint;
            for(unsigned int n = 0 ; n < labelMap->GetNumberOfLabelObjects() ; ++n)
            {
                ShapeLabelObjectType* labelObject = labelMap->GetNthLabelObject(n);

                // append to landmark
                const typename ShapeLabelObjectType::CentroidType centroid = labelObject->GetCentroid();

                newPoint = data::Point::New(centroid[0], centroid[1], centroid[2]);
                landmarks->getPoints().push_back(newPoint);

                // append to point the label
                std::stringstream labelName;
                labelName << n;
                data::String::sptr label = data::String::New(labelName.str());
                newPoint->setField(data::fieldHelper::Image::m_labelId, label);
            }

            image->setField("ShowLandmarks", data::Boolean::New(true));
        }
    }
};

//------------------------------------------------------------------------------

void computeCentroids(
    data::Image::sptr image,
    std::vector<data::PointList::sptr> pointListCentroids,
    std::vector<std::vector<size_t> > pointListLabels
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
    const core::tools::Type type = image->getType();
    core::tools::Dispatcher<core::tools::SupportedDispatcherTypes, LabelImageFilter>::invoke(type, params);
}

} // namespace sight::filter::image.

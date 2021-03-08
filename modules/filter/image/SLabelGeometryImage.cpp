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

#include "modules/filter/image/SLabelGeometryImage.hpp"

#include <core/com/Signal.hxx>
#include <core/com/Slot.hpp>
#include <core/com/Slots.hxx>
#include <core/tools/Dispatcher.hpp>
#include <core/tools/DynamicTypeKeyTypeMapping.hpp>
#include <core/tools/IntrinsicTypes.hpp>

#include <data/fieldHelper/Image.hpp>
#include <data/fieldHelper/MedicalImageHelpers.hpp>
#include <data/helper/Image.hpp>
#include <data/Image.hpp>
#include <data/String.hpp>

#include <filter/image/Labeling.hpp>

#include <service/macros.hpp>

#include <boost/tokenizer.hpp>

namespace sight::module::filter::image
{

const core::com::Slots::SlotKeyType s_UPDATE_SELECTED_POINT_LIST = "updateSelectedPointList";

//-----------------------------------------------------------------------------

SLabelGeometryImage::SLabelGeometryImage()
{
    newSlot(s_UPDATE_SELECTED_POINT_LIST, &SLabelGeometryImage::updateSelectedPointList, this);
}

//-----------------------------------------------------------------------------

SLabelGeometryImage::~SLabelGeometryImage()
{
}

//-----------------------------------------------------------------------------

void SLabelGeometryImage::configuring()
{
    const core::runtime::ConfigurationElement::sptr clusters = m_configuration->findConfigurationElement("clusters");
    if(clusters)
    {
        SIGHT_ASSERT("pointList is needed in output key", m_configuration->findConfigurationElement("out"));
        std::vector< core::runtime::ConfigurationElement::sptr> clusterVect = clusters->find("cluster");

        SIGHT_ASSERT("Clusters must have cluster tag.", clusterVect.size() > 0);

        for(size_t i = 0; i < clusterVect.size(); ++i)
        {
            std::string clusterStr = clusterVect[i]->getValue();
            std::vector<size_t> clusterLabels;
            const :: boost::char_separator<char> separator(",");
            const ::boost::tokenizer< ::boost::char_separator<char> > tok {clusterStr, separator};

            for(const auto& t : tok)
            {
                clusterLabels.push_back(std::stoi(t));
            }

            m_lPointListLabels.push_back(clusterLabels);
            m_lPointListCentroids.push_back(data::PointList::New());
        }
    }
}

//-----------------------------------------------------------------------------

void SLabelGeometryImage::starting()
{
}

//-----------------------------------------------------------------------------

void SLabelGeometryImage::updating()
{
    const auto image = this->getLockedInOut< data::Image >("image");

    data::helper::Image imageHelper(image.get_shared());
    if (!imageHelper.getBuffer())
    {
        SIGHT_INFO("Image is not set.");
        return;
    }

    // Call the ITK operator
    sight::filter::image::computeCentroids(image.get_shared(), m_lPointListCentroids, m_lPointListLabels);

    if(m_lPointListCentroids.empty())
    {
        //get landmarks
        data::fieldHelper::MedicalImageHelpers::checkLandmarks( image.get_shared() );
        data::PointList::sptr landmarks =
            image->getField< data::PointList >( data::fieldHelper::Image::m_imageLandmarksId);
        SIGHT_ASSERT("landmarks not instanced", landmarks);

        for(const auto& point : landmarks->getPoints())
        {
            // notify
            auto sig = image->signal< data::Image::LandmarkAddedSignalType >(data::Image::s_LANDMARK_ADDED_SIG);
            sig->asyncEmit(point);
        }
    }
    else
    {
        this->updateSelectedPointList("1", "");
    }

    m_sigComputed->asyncEmit();
}

//-----------------------------------------------------------------------------

void SLabelGeometryImage::stopping()
{
}

//-----------------------------------------------------------------------------

void SLabelGeometryImage::updateSelectedPointList(std::string value, std::string key)
{
    SIGHT_ASSERT("value: " << value << "should end by a number between 0 and 9",
                 value.back() >= '0' && value.back() <= '9');
    int indexPlane = std::stoi(value);
    // if the XML enum is between 1 and n, instead of 0 and n-1
    if(indexPlane > 0)
    {
        indexPlane--;
    }
    data::PointList::sptr selectedPointList = m_lPointListCentroids.at(indexPlane);

    for(size_t idPoint = 0; idPoint < selectedPointList->getPoints().size(); ++idPoint)
    {
        data::String::sptr label = data::String::New(std::to_string(idPoint));
        selectedPointList->getPoints().at(idPoint)->setField( data::fieldHelper::Image::m_labelId, label );
    }

    this->setOutput("pointList", m_lPointListCentroids.at(indexPlane));
}

//-----------------------------------------------------------------------------

} // namespace sight::module::filter::image.

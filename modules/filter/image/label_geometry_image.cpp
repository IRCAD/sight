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

#include "modules/filter/image/label_geometry_image.hpp"

#include <core/com/signal.hxx>
#include <core/com/slots.hxx>

#include <data/helper/MedicalImage.hpp>

#include <filter/image/labeling.hpp>

#include <boost/tokenizer.hpp>

namespace sight::module::filter::image
{

const core::com::slots::key_t UPDATE_SELECTED_POINT_LIST = "updateSelectedPointList";

//-----------------------------------------------------------------------------

label_geometry_image::label_geometry_image()
{
    new_slot(UPDATE_SELECTED_POINT_LIST, &label_geometry_image::updateSelectedPointList, this);
}

//-----------------------------------------------------------------------------

label_geometry_image::~label_geometry_image()
= default;

//-----------------------------------------------------------------------------

void label_geometry_image::configuring()
{
    const auto& config = this->get_config();

    const auto& clusters = config.get_child("clusters");
    for(const auto& elt : boost::make_iterator_range(clusters.equal_range("cluster")))
    {
        const auto clusterStr = elt.second.get_value<std::string>();
        std::vector<std::size_t> clusterLabels;
        const boost::char_separator<char> separator(",");
        const boost::tokenizer<boost::char_separator<char> > tok {clusterStr, separator};

        for(const auto& t : tok)
        {
            clusterLabels.push_back(std::stoul(t));
        }

        m_lPointListLabels.push_back(clusterLabels);
        m_lPointListCentroids.push_back(std::make_shared<data::point_list>());
    }
}

//-----------------------------------------------------------------------------

void label_geometry_image::starting()
{
}

//-----------------------------------------------------------------------------

void label_geometry_image::updating()
{
    const auto image = m_image.lock();

    // Call the ITK operator
    sight::filter::image::computeCentroids(image.get_shared(), m_lPointListCentroids, m_lPointListLabels);

    if(m_lPointListCentroids.empty())
    {
        data::point_list::sptr landmarks = data::helper::MedicalImage::getLandmarks(*image);

        SIGHT_ASSERT("landmarks not instanced", landmarks);

        for(const auto& point : landmarks->getPoints())
        {
            auto sig = image->signal<data::image::LandmarkAddedSignalType>(data::image::LANDMARK_ADDED_SIG);
            sig->async_emit(point);
        }
    }
    else
    {
        this->updateSelectedPointList("1", "");
    }

    m_sigComputed->async_emit();
}

//-----------------------------------------------------------------------------

void label_geometry_image::stopping()
{
}

//-----------------------------------------------------------------------------

void label_geometry_image::updateSelectedPointList(std::string value, std::string /*key*/)
{
    SIGHT_ASSERT(
        "value: " << value << "should end by a number between 0 and 9",
        value.back() >= '0' && value.back() <= '9'
    );
    std::size_t indexPlane = std::stoul(value);
    // if the XML enum is between 1 and n, instead of 0 and n-1
    if(indexPlane > 0)
    {
        indexPlane--;
    }

    data::point_list::sptr selectedPointList = m_lPointListCentroids.at(indexPlane);

    for(std::size_t idPoint = 0 ; idPoint < selectedPointList->getPoints().size() ; ++idPoint)
    {
        selectedPointList->getPoints().at(idPoint)->setLabel(std::to_string(idPoint));
    }

    this->set_output("pointList", m_lPointListCentroids.at(indexPlane));
}

//-----------------------------------------------------------------------------

} // namespace sight::module::filter::image.

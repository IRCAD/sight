/************************************************************************
 *
 * Copyright (C) 2020-2021 IRCAD France
 * Copyright (C) 2017 IHU Strasbourg
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

#include "io/patch/semantic/V10/V09ALA/data/Image.hpp"

#include <atoms/Boolean.hpp>
#include <atoms/Map.hpp>
#include <atoms/Numeric.hpp>
#include <atoms/Numeric.hxx>
#include <atoms/Object.hpp>
#include <atoms/Object.hxx>
#include <atoms/String.hpp>

#include <core/tools/UUID.hpp>

#include <io/atoms/patch/helper/functions.hpp>
#include <io/atoms/patch/StructuralCreatorDB.hpp>

#include <boost/algorithm/string.hpp>

namespace sight::io::patch::semantic
{
namespace V10
{
namespace V09ALA
{
namespace data
{

Image::Image() :
    io::atoms::patch::ISemanticPatch()
{
    m_originClassname = "::sight::data::Image";
    m_originVersion   = "2";
    this->addContext("MedicalData", "V10", "V09ALA");
}

// ----------------------------------------------------------------------------

Image::~Image()
{
}

// ----------------------------------------------------------------------------

Image::Image( const Image& cpy ) :
    io::atoms::patch::ISemanticPatch(cpy)
{
}

// ----------------------------------------------------------------------------

void Image::apply(
    const sight::atoms::Object::sptr& previous,
    const sight::atoms::Object::sptr& current,
    io::atoms::patch::IPatch::NewVersionsType& newVersions)
{
    ISemanticPatch::apply(previous, current, newVersions);
    io::atoms::patch::helper::cleanFields( current );
    io::atoms::patch::helper::Object helper( current );

    sight::atoms::Map::csptr previousFieldMap = sight::atoms::Map::dynamicCast(previous->getAttribute("fields"));
    SIGHT_ASSERT("Image does not have field map", previousFieldMap);

    const auto& iter = previousFieldMap->find("m_landmarksId");
    if (iter != previousFieldMap->end())
    {
        // create new Landmarks structure
        io::atoms::patch::StructuralCreatorDB::sptr creators = io::atoms::patch::StructuralCreatorDB::getDefault();

        sight::atoms::Object::sptr currentPL = creators->create( "::sight::data::PointList", "1");
        io::atoms::patch::helper::Object helperPL( currentPL );

        sight::atoms::Map::sptr currentFieldMap = sight::atoms::Map::dynamicCast(current->getAttribute("fields"));
        currentFieldMap->insert("m_imageLandmarksId", currentPL);

        sight::atoms::Sequence::sptr plSeq = sight::atoms::Sequence::dynamicCast(currentPL->getAttribute("points"));

        // Convert previous Landmarks
        sight::atoms::Object::sptr previousLandmarks = sight::atoms::Object::dynamicCast(iter->second);

        sight::atoms::Map::sptr previousLandmarksMap =
            sight::atoms::Map::dynamicCast(previousLandmarks->getAttribute("landmarks"));

        for (const auto& elt : previousLandmarksMap->getValue())
        {
            sight::atoms::Object::csptr atomGroup = sight::atoms::Object::dynamicCast(elt.second);
            sight::atoms::Sequence::csptr points  = sight::atoms::Sequence::dynamicCast(atomGroup->getAttribute(
                                                                                            "points"));

            size_t count = 0;
            for (const auto& ptObj : points->getValue())
            {
                sight::atoms::String::csptr previousPt = sight::atoms::String::dynamicCast(ptObj);
                std::vector<std::string> res;
                const std::string coords = previousPt->getValue();
                ::boost::split(res, coords, ::boost::is_any_of(";"));

                sight::atoms::Object::sptr point = sight::atoms::Object::New();
                io::atoms::patch::helper::setClassname(point, "::sight::data::Point");
                io::atoms::patch::helper::setVersion(point, "1");
                io::atoms::patch::helper::generateID(point);

                plSeq->push_back(point);

                io::atoms::patch::helper::Object helperPt( point );
                sight::atoms::Sequence::sptr pointCoord = sight::atoms::Sequence::New();
                helperPt.addAttribute("coord", pointCoord);
                pointCoord->push_back(sight::atoms::Numeric::New(std::stod(res[0])));
                pointCoord->push_back(sight::atoms::Numeric::New(std::stod(res[1])));
                pointCoord->push_back(sight::atoms::Numeric::New(std::stod(res[2])));

                // set point label
                sight::atoms::Map::sptr pointFieldMap = sight::atoms::Map::New();
                helperPt.addAttribute("fields", pointFieldMap);

                const std::string label = elt.first + "_" + std::to_string(count);

                pointFieldMap->insert("m_labelId", sight::atoms::String::New(label));
                ++count;
            }
        }
    }
}

// ----------------------------------------------------------------------------

} // namespace data
} // namespace V09ALA
} // namespace V10
} // namespace sight::io::patch::semantic

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

#include "fwMDSemanticPatch/V10/V09ALA/data/Image.hpp"

#include <atoms/Boolean.hpp>
#include <atoms/Map.hpp>
#include <atoms/Numeric.hpp>
#include <atoms/Numeric.hxx>
#include <atoms/Object.hpp>
#include <atoms/Object.hxx>
#include <atoms/String.hpp>

#include <core/tools/UUID.hpp>

#include <fwAtomsPatch/helper/functions.hpp>
#include <fwAtomsPatch/StructuralCreatorDB.hpp>

#include <boost/algorithm/string.hpp>

namespace fwMDSemanticPatch
{
namespace V10
{
namespace V09ALA
{
namespace sight::data
{

Image::Image() :
    ::fwAtomsPatch::ISemanticPatch()
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
    ::fwAtomsPatch::ISemanticPatch(cpy)
{
}

// ----------------------------------------------------------------------------

void Image::apply(
    const atoms::Object::sptr& previous,
    const atoms::Object::sptr& current,
    ::fwAtomsPatch::IPatch::NewVersionsType& newVersions)
{
    ISemanticPatch::apply(previous, current, newVersions);
    ::fwAtomsPatch::helper::cleanFields( current );
    ::fwAtomsPatch::helper::Object helper( current );

    atoms::Map::csptr previousFieldMap = atoms::Map::dynamicCast(previous->getAttribute("fields"));
    SLM_ASSERT("Image does not have field map", previousFieldMap);

    const auto& iter = previousFieldMap->find("m_landmarksId");
    if (iter != previousFieldMap->end())
    {
        // create new Landmarks structure
        ::fwAtomsPatch::StructuralCreatorDB::sptr creators = ::fwAtomsPatch::StructuralCreatorDB::getDefault();

        atoms::Object::sptr currentPL = creators->create( "::sight::data::PointList", "1");
        ::fwAtomsPatch::helper::Object helperPL( currentPL );

        atoms::Map::sptr currentFieldMap = atoms::Map::dynamicCast(current->getAttribute("fields"));
        currentFieldMap->insert("m_imageLandmarksId", currentPL);

        atoms::Sequence::sptr plSeq = atoms::Sequence::dynamicCast(currentPL->getAttribute("points"));

        // Convert previous Landmarks
        atoms::Object::sptr previousLandmarks = atoms::Object::dynamicCast(iter->second);

        atoms::Map::sptr previousLandmarksMap =
            atoms::Map::dynamicCast(previousLandmarks->getAttribute("landmarks"));

        for (const auto& elt : previousLandmarksMap->getValue())
        {
            atoms::Object::csptr atomGroup = atoms::Object::dynamicCast(elt.second);
            atoms::Sequence::csptr points  = atoms::Sequence::dynamicCast(atomGroup->getAttribute("points"));

            size_t count = 0;
            for (const auto& ptObj : points->getValue())
            {
                atoms::String::csptr previousPt = atoms::String::dynamicCast(ptObj);
                std::vector<std::string> res;
                const std::string coords = previousPt->getValue();
                ::boost::split(res, coords, ::boost::is_any_of(";"));

                atoms::Object::sptr point = atoms::Object::New();
                ::fwAtomsPatch::helper::setClassname(point, "::sight::data::Point");
                ::fwAtomsPatch::helper::setVersion(point, "1");
                ::fwAtomsPatch::helper::generateID(point);

                plSeq->push_back(point);

                ::fwAtomsPatch::helper::Object helperPt( point );
                atoms::Sequence::sptr pointCoord = atoms::Sequence::New();
                helperPt.addAttribute("coord", pointCoord);
                pointCoord->push_back(atoms::Numeric::New(std::stod(res[0])));
                pointCoord->push_back(atoms::Numeric::New(std::stod(res[1])));
                pointCoord->push_back(atoms::Numeric::New(std::stod(res[2])));

                // set point label
                atoms::Map::sptr pointFieldMap = atoms::Map::New();
                helperPt.addAttribute("fields", pointFieldMap);

                const std::string label = elt.first + "_" + std::to_string(count);

                pointFieldMap->insert("m_labelId", atoms::String::New(label));
                ++count;
            }
        }
    }
}

// ----------------------------------------------------------------------------

} // namespace sight::data
} // namespace V09ALA
} // namespace V10
} // namespace fwMDSemanticPatch

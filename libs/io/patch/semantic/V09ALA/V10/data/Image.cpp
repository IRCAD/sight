/************************************************************************
 *
 * Copyright (C) 2020-2021 IRCAD France
 * Copyright (C) 2020-2021 IHU Strasbourg
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

#include "io/patch/semantic/V09ALA/V10/data/Image.hpp"

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

namespace sight::io::patch::semantic
{
namespace V09ALA
{
namespace V10
{
namespace data
{

Image::Image() :
    io::atoms::patch::ISemanticPatch()
{
    m_originClassname = "::sight::data::Image";
    m_originVersion   = "2";
    this->addContext("MedicalData", "V09ALA", "V10");
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

    const auto& iter = previousFieldMap->find("m_imageLandmarksId");
    if (iter != previousFieldMap->end())
    {
        // create new Landmarks structure
        io::atoms::patch::StructuralCreatorDB::sptr creators = io::atoms::patch::StructuralCreatorDB::getDefault();

        sight::atoms::Object::sptr currentLandmarks = creators->create( "::sight::data::Landmarks", "1");
        io::atoms::patch::helper::Object helperLandmarks( currentLandmarks );

        sight::atoms::Map::sptr currentFieldMap = sight::atoms::Map::dynamicCast(current->getAttribute("fields"));
        currentFieldMap->insert("m_landmarksId", currentLandmarks);

        sight::atoms::Map::sptr landmarksMap = sight::atoms::Map::dynamicCast(currentLandmarks->getAttribute(
                                                                                  "landmarks"));

        // Convert previous PointList
        sight::atoms::Object::sptr previousPL = sight::atoms::Object::dynamicCast(iter->second);

        sight::atoms::Sequence::sptr previousPLSeq =
            sight::atoms::Sequence::dynamicCast(previousPL->getAttribute("points"));

        size_t count = 0;
        for (const auto& obj : previousPLSeq->getValue())
        {
            // get point coordinates
            sight::atoms::Object::csptr point         = sight::atoms::Object::dynamicCast(obj);
            sight::atoms::Sequence::csptr pointCoords =
                sight::atoms::Sequence::dynamicCast(point->getAttribute("coord"));
            sight::atoms::Numeric::csptr coordX = sight::atoms::Numeric::dynamicCast(pointCoords->getValue()[0]);
            sight::atoms::Numeric::csptr coordY = sight::atoms::Numeric::dynamicCast(pointCoords->getValue()[1]);
            sight::atoms::Numeric::csptr coordZ = sight::atoms::Numeric::dynamicCast(pointCoords->getValue()[2]);
            const std::string coords            = coordX->getString() + ";" + coordY->getString() + ";" +
                                                  coordZ->getString();

            // get point label
            sight::atoms::Map::csptr pointFieldMap = sight::atoms::Map::dynamicCast(point->getAttribute("fields"));

            std::string label;
            const auto& it = pointFieldMap->find("m_labelId");
            if (it != pointFieldMap->end())
            {
                sight::atoms::Object::csptr labelObj = sight::atoms::Object::dynamicCast(it->second);
                sight::atoms::String::csptr labelStr =
                    sight::atoms::String::dynamicCast(labelObj->getAttribute("value"));
                label = labelStr->getValue();
            }
            if (label.empty())
            {
                label = "label_" + std::to_string(count++);
            }

            // create one landmark group per point
            sight::atoms::Object::sptr atomGroup = sight::atoms::Object::New();
            atomGroup->setMetaInfo("ID_METAINFO", core::tools::UUID::generateUUID());

            atomGroup->setAttribute("color", sight::atoms::String::New("1;1;1;1"));
            atomGroup->setAttribute("size", sight::atoms::Numeric::New(1));
            atomGroup->setAttribute("shape", sight::atoms::String::New("SPHERE"));
            atomGroup->setAttribute("visibility", sight::atoms::Boolean::New(true));

            sight::atoms::Sequence::sptr seq = sight::atoms::Sequence::New();

            seq->push_back(sight::atoms::String::New(coords));
            atomGroup->setAttribute("points", seq);
            landmarksMap->insert(label, atomGroup);
        }
    }
}

// ----------------------------------------------------------------------------

} // namespace data
} // namespace V10
} // namespace V09ALA
} // namespace sight::io::patch::semantic

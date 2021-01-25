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

#include "fwMDSemanticPatch/V09ALA/V10/data/Image.hpp"

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

namespace fwMDSemanticPatch
{
namespace V09ALA
{
namespace V10
{
namespace sight::data
{

Image::Image() :
    ::fwAtomsPatch::ISemanticPatch()
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

    const auto& iter = previousFieldMap->find("m_imageLandmarksId");
    if (iter != previousFieldMap->end())
    {
        // create new Landmarks structure
        ::fwAtomsPatch::StructuralCreatorDB::sptr creators = ::fwAtomsPatch::StructuralCreatorDB::getDefault();

        atoms::Object::sptr currentLandmarks = creators->create( "::sight::data::Landmarks", "1");
        ::fwAtomsPatch::helper::Object helperLandmarks( currentLandmarks );

        atoms::Map::sptr currentFieldMap = atoms::Map::dynamicCast(current->getAttribute("fields"));
        currentFieldMap->insert("m_landmarksId", currentLandmarks);

        atoms::Map::sptr landmarksMap = atoms::Map::dynamicCast(currentLandmarks->getAttribute("landmarks"));

        // Convert previous PointList
        atoms::Object::sptr previousPL = atoms::Object::dynamicCast(iter->second);

        atoms::Sequence::sptr previousPLSeq = atoms::Sequence::dynamicCast(previousPL->getAttribute("points"));

        size_t count = 0;
        for (const auto& obj : previousPLSeq->getValue())
        {
            // get point coordinates
            atoms::Object::csptr point         = atoms::Object::dynamicCast(obj);
            atoms::Sequence::csptr pointCoords = atoms::Sequence::dynamicCast(point->getAttribute("coord"));
            atoms::Numeric::csptr coordX       = atoms::Numeric::dynamicCast(pointCoords->getValue()[0]);
            atoms::Numeric::csptr coordY       = atoms::Numeric::dynamicCast(pointCoords->getValue()[1]);
            atoms::Numeric::csptr coordZ       = atoms::Numeric::dynamicCast(pointCoords->getValue()[2]);
            const std::string coords           = coordX->getString() + ";" + coordY->getString() + ";" +
                                                 coordZ->getString();

            // get point label
            atoms::Map::csptr pointFieldMap = atoms::Map::dynamicCast(point->getAttribute("fields"));

            std::string label;
            const auto& it = pointFieldMap->find("m_labelId");
            if (it != pointFieldMap->end())
            {
                atoms::Object::csptr labelObj = atoms::Object::dynamicCast(it->second);
                atoms::String::csptr labelStr = atoms::String::dynamicCast(labelObj->getAttribute("value"));
                label = labelStr->getValue();
            }
            if (label.empty())
            {
                label = "label_" + std::to_string(count++);
            }

            // create one landmark group per point
            atoms::Object::sptr atomGroup = atoms::Object::New();
            atomGroup->setMetaInfo("ID_METAINFO", core::tools::UUID::generateUUID());

            atomGroup->setAttribute("color", atoms::String::New("1;1;1;1"));
            atomGroup->setAttribute("size", atoms::Numeric::New(1));
            atomGroup->setAttribute("shape", atoms::String::New("SPHERE"));
            atomGroup->setAttribute("visibility", atoms::Boolean::New(true));

            atoms::Sequence::sptr seq = atoms::Sequence::New();

            seq->push_back(atoms::String::New(coords));
            atomGroup->setAttribute("points", seq);
            landmarksMap->insert(label, atomGroup);
        }
    }
}

// ----------------------------------------------------------------------------

} // namespace sight::data
} // namespace V10
} // namespace V09ALA
} // namespace fwMDSemanticPatch

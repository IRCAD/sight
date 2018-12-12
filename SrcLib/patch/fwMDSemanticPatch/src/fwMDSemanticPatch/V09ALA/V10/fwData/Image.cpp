/************************************************************************
 *
 * Copyright (C) 2017 IRCAD France
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

#include "fwMDSemanticPatch/V09ALA/V10/fwData/Image.hpp"

#include <fwAtoms/Boolean.hpp>
#include <fwAtoms/Map.hpp>
#include <fwAtoms/Numeric.hpp>
#include <fwAtoms/Numeric.hxx>
#include <fwAtoms/Object.hpp>
#include <fwAtoms/Object.hxx>
#include <fwAtoms/String.hpp>

#include <fwAtomsPatch/helper/functions.hpp>
#include <fwAtomsPatch/StructuralCreatorDB.hpp>

#include <fwTools/UUID.hpp>

namespace fwMDSemanticPatch
{
namespace V09ALA
{
namespace V10
{
namespace fwData
{

Image::Image() :
    ::fwAtomsPatch::ISemanticPatch()
{
    m_originClassname = "::fwData::Image";
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
    const ::fwAtoms::Object::sptr& previous,
    const ::fwAtoms::Object::sptr& current,
    ::fwAtomsPatch::IPatch::NewVersionsType& newVersions)
{
    ISemanticPatch::apply(previous, current, newVersions);
    ::fwAtomsPatch::helper::cleanFields( current );
    ::fwAtomsPatch::helper::Object helper( current );

    ::fwAtoms::Map::csptr previousFieldMap = ::fwAtoms::Map::dynamicCast(previous->getAttribute("fields"));
    SLM_ASSERT("Image does not have field map", previousFieldMap);

    const auto& iter = previousFieldMap->find("m_imageLandmarksId");
    if (iter != previousFieldMap->end())
    {
        // create new Landmarks structure
        ::fwAtomsPatch::StructuralCreatorDB::sptr creators = ::fwAtomsPatch::StructuralCreatorDB::getDefault();

        ::fwAtoms::Object::sptr currentLandmarks = creators->create( "::fwData::Landmarks", "1");
        ::fwAtomsPatch::helper::Object helperLandmarks( currentLandmarks );

        ::fwAtoms::Map::sptr currentFieldMap = ::fwAtoms::Map::dynamicCast(current->getAttribute("fields"));
        currentFieldMap->insert("m_landmarksId", currentLandmarks);

        ::fwAtoms::Map::sptr landmarksMap = ::fwAtoms::Map::dynamicCast(currentLandmarks->getAttribute("landmarks"));

        // Convert previous PointList
        ::fwAtoms::Object::sptr previousPL = ::fwAtoms::Object::dynamicCast(iter->second);

        ::fwAtoms::Sequence::sptr previousPLSeq = ::fwAtoms::Sequence::dynamicCast(previousPL->getAttribute("points"));

        size_t count = 0;
        for (const auto& obj : previousPLSeq->getValue())
        {
            // get point coordinates
            ::fwAtoms::Object::csptr point         = ::fwAtoms::Object::dynamicCast(obj);
            ::fwAtoms::Sequence::csptr pointCoords = ::fwAtoms::Sequence::dynamicCast(point->getAttribute("coord"));
            ::fwAtoms::Numeric::csptr coordX       = ::fwAtoms::Numeric::dynamicCast(pointCoords->getValue()[0]);
            ::fwAtoms::Numeric::csptr coordY       = ::fwAtoms::Numeric::dynamicCast(pointCoords->getValue()[1]);
            ::fwAtoms::Numeric::csptr coordZ       = ::fwAtoms::Numeric::dynamicCast(pointCoords->getValue()[2]);
            const std::string coords = coordX->getString() + ";" + coordY->getString() + ";" + coordZ->getString();

            // get point label
            ::fwAtoms::Map::csptr pointFieldMap = ::fwAtoms::Map::dynamicCast(point->getAttribute("fields"));

            std::string label;
            const auto& it = pointFieldMap->find("m_labelId");
            if (it != pointFieldMap->end())
            {
                ::fwAtoms::Object::csptr labelObj = ::fwAtoms::Object::dynamicCast(it->second);
                ::fwAtoms::String::csptr labelStr = ::fwAtoms::String::dynamicCast(labelObj->getAttribute("value"));
                label                             = labelStr->getValue();
            }
            if (label.empty())
            {
                label = "label_" + std::to_string(count++);
            }

            // create one landmark group per point
            ::fwAtoms::Object::sptr atomGroup = ::fwAtoms::Object::New();
            atomGroup->setMetaInfo("ID_METAINFO", ::fwTools::UUID::generateUUID());

            atomGroup->setAttribute("color", ::fwAtoms::String::New("1;1;1;1"));
            atomGroup->setAttribute("size", ::fwAtoms::Numeric::New(1));
            atomGroup->setAttribute("shape", ::fwAtoms::String::New("SPHERE"));
            atomGroup->setAttribute("visibility", ::fwAtoms::Boolean::New(true));

            ::fwAtoms::Sequence::sptr seq = ::fwAtoms::Sequence::New();

            seq->push_back(::fwAtoms::String::New(coords));
            atomGroup->setAttribute("points", seq);
            landmarksMap->insert(label, atomGroup);
        }
    }
}

// ----------------------------------------------------------------------------

} // namespace fwData
} // namespace V10
} // namespace V09ALA
} // namespace fwMDSemanticPatch


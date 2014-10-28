/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <string>
#include <sstream>

#include <boost/date_time/posix_time/posix_time.hpp>

#include <fwTools/dateAndTime.hpp>

#include <fwAtoms/String.hpp>

#include "fwStructuralPatch/fwData/Patient/V1ToFwMedDataPatientV1.hpp"

namespace fwStructuralPatch
{

namespace fwData
{

namespace Patient
{

V1ToFwMedDataPatientV1::V1ToFwMedDataPatientV1() : ::fwAtomsPatch::IStructuralPatch()
{
    m_originClassname = "::fwData::Patient";
    m_targetClassname = "::fwMedData::Patient";
    m_originVersion = "1";
    m_targetVersion = "1";
}

// ----------------------------------------------------------------------------

V1ToFwMedDataPatientV1::~V1ToFwMedDataPatientV1()
{
}

// ----------------------------------------------------------------------------

V1ToFwMedDataPatientV1::V1ToFwMedDataPatientV1(
        const V1ToFwMedDataPatientV1 &cpy ) : ::fwAtomsPatch::IStructuralPatch(cpy)
{
}

// ----------------------------------------------------------------------------

void V1ToFwMedDataPatientV1::apply(const ::fwAtoms::Object::sptr& previous,
        const ::fwAtoms::Object::sptr& current,
        ::fwAtomsPatch::IPatch::NewVersionsType& newVersions)
{
    IStructuralPatch::apply(previous, current, newVersions);

    // Update object version
    this->updateVersion(current);

    // Create helper
    ::fwAtomsPatch::helper::Object helper(current);

    helper.removeAttribute("firstname");
    helper.removeAttribute("db_id");
    helper.removeAttribute("studies");

    helper.renameAttribute("id_dicom", "patient_id");
    helper.renameAttribute("birthdate", "birth_date");
    helper.renameAttribute("is_male", "sex");

    // Update Name
    std::string nameSeperation = "^";
    if( previous->getAttribute("name")->getString().empty() ||
        previous->getAttribute("firstname")->getString().empty() )
    {
        nameSeperation = "";
    }

    std::string newName = previous->getAttribute("name")->getString()
            + nameSeperation
            + previous->getAttribute("firstname")->getString();
    helper.replaceAttribute( "name", ::fwAtoms::String::New(newName) );

    // Update Birthdate
    ::boost::posix_time::ptime bithdate =
            ::boost::posix_time::time_from_string(previous->getAttribute("birthdate")->getString());
    helper.replaceAttribute("birth_date", ::fwAtoms::String::New(::fwTools::getDate(bithdate)));

    // Update Sex
    helper.replaceAttribute("sex", ::fwAtoms::String::New(
            !(previous->getAttribute("is_male")->getString().compare("true"))?"M":"F"));
}

} // namespace Patient

} // namespace fwData

} // namespace fwStructuralPatch


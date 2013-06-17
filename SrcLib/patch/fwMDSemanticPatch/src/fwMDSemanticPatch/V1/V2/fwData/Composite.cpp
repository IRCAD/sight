/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <string>

#include <boost/foreach.hpp>

#include <fwTools/UUID.hpp>

#include <fwAtoms/Object.hpp>
#include <fwAtoms/Object.hxx>
#include <fwAtoms/Numeric.hpp>
#include <fwAtoms/String.hpp>
#include <fwAtoms/Boolean.hpp>
#include <fwAtoms/Sequence.hpp>
#include <fwAtoms/Map.hpp>

#include <fwAtomsPatch/StructuralCreatorDB.hpp>
#include <fwAtomsPatch/helper/functions.hpp>

#include "fwMDSemanticPatch/V1/V2/fwData/Composite.hpp"

namespace fwMDSemanticPatch
{
namespace V1
{
namespace V2
{
namespace fwData
{

Composite::Composite() : ::fwAtomsPatch::ISemanticPatch()
{
    m_originClassname = "::fwData::Composite";
    m_originVersion = "1";
    this->addContext("MedicalData", "V1", "V2");
}

// ----------------------------------------------------------------------------

Composite::~Composite()
{
}

// ----------------------------------------------------------------------------

Composite::Composite( const Composite &cpy ) : ::fwAtomsPatch::ISemanticPatch(cpy)
{
}

// ----------------------------------------------------------------------------

void Composite::apply(
        const ::fwAtoms::Object::sptr& previous,
        const ::fwAtoms::Object::sptr& current,
        ::fwAtomsPatch::IPatch::NewVersionsType& newVersions)
{
    ISemanticPatch::apply(previous, current, newVersions);

    // Create helper
    ::fwAtomsPatch::helper::Object helper(current);

    // If fwData::Composite is a MedicalWorkspace
    if ( previous->getMetaInfo("compositeType") == "MedicalWorkspace" )
    {
        // Update type/version
        ::fwAtomsPatch::helper::setClassname( current, "::fwMedData::SeriesDB" );
        ::fwAtomsPatch::helper::setVersion( current, "1" );

        // Update Attributes
        ::fwAtomsPatch::helper::cleanFields( current );
        // Replace fwAtoms::Map by fwAtoms::Sequence ( have the same key )
        ::fwAtoms::Sequence::sptr series = ::fwAtoms::Sequence::New();
        helper.replaceAttribute( "values", series );

        // Add ImageSeries and create ModelSeries if they are necessary
        ::fwAtoms::Map::sptr oldCompositeMap = previous->getAttribute< ::fwAtoms::Map >("values");
        ::fwAtoms::Object::sptr oldPatientDB =
                ::fwAtoms::Object::dynamicCast( (*oldCompositeMap)["patientDB"] );
        ::fwAtoms::Sequence::sptr oldPatients = oldPatientDB->getAttribute< ::fwAtoms::Sequence >("patients");

        BOOST_FOREACH( ::fwAtoms::Base::sptr oldPatientAtom, oldPatients->getValue() )
        {
            ::fwAtoms::Object::sptr oldPatient = ::fwAtoms::Object::dynamicCast( oldPatientAtom );
            ::fwAtoms::Object::sptr newPatient = newVersions[oldPatient];

            ::fwAtoms::Sequence::sptr oldStudies = oldPatient->getAttribute< ::fwAtoms::Sequence >("studies");

            BOOST_FOREACH( ::fwAtoms::Base::sptr oldStudyAtom, oldStudies->getValue() )
            {
                ::fwAtoms::Object::sptr oldStudy = ::fwAtoms::Object::dynamicCast( oldStudyAtom );
                ::fwAtoms::Object::sptr newStudy = newVersions[oldStudy];

                ::fwAtoms::Sequence::sptr oldAcquisitions =
                        oldStudy->getAttribute< ::fwAtoms::Sequence >("acquisitions");

                BOOST_FOREACH( ::fwAtoms::Base::sptr oldAcqAtom, oldAcquisitions->getValue() )
                {
                    ::fwAtoms::Object::sptr oldAcq = ::fwAtoms::Object::dynamicCast( oldAcqAtom );

                    // finalize and push newImgSeries
                    ::fwAtoms::Object::sptr newImgSeries = newVersions[oldAcq];
                    ::fwAtomsPatch::helper::Object imgSeriesHelper (newImgSeries);
                    ::fwAtoms::Object::sptr clonedPatient = ::fwAtoms::Object::dynamicCast(newPatient->clone());
                    ::fwAtomsPatch::helper::changeUID(clonedPatient);

                    imgSeriesHelper.replaceAttribute("patient", clonedPatient );
                    ::fwAtoms::Object::sptr clonedStudy = ::fwAtoms::Object::dynamicCast(newStudy->clone());
                    ::fwAtomsPatch::helper::changeUID(clonedStudy);

                    imgSeriesHelper.replaceAttribute("study", clonedStudy );
                    imgSeriesHelper.replaceAttribute("modality", oldStudy->getAttribute("modality")->clone() );

                    ::fwAtoms::String::sptr institution;
                    institution = ::fwAtoms::String::dynamicCast( oldStudy->getAttribute("hospital")->clone() );
                    ::fwAtoms::Object::sptr equipment = newImgSeries->getAttribute< ::fwAtoms::Object >("equipment");
                    ::fwAtomsPatch::helper::Object equipmentHelper(equipment);
                    equipmentHelper.replaceAttribute("institution_name", institution);


                    // Set study date and time if necessary
                    ::fwAtoms::String::sptr studyDate = newStudy->getAttribute< ::fwAtoms::String >("date");
                    ::fwAtomsPatch::helper::Object studyHelper(clonedStudy);

                    if ( studyDate->getValue().empty() )
                    {
                        studyHelper.replaceAttribute("date", newImgSeries->getAttribute("date")->clone());
                    }

                    ::fwAtoms::String::sptr studyTime = newStudy->getAttribute< ::fwAtoms::String >("time");
                    if ( studyTime->getValue().empty() )
                    {
                        studyHelper.replaceAttribute("time", newImgSeries->getAttribute("time")->clone());
                    }

                    series->push_back( newImgSeries );

                    // finalize and push newModelSeries
                    ::fwAtoms::Sequence::sptr oldReconstructions =
                            oldAcq->getAttribute< ::fwAtoms::Sequence >("reconstructions");

                    if ( oldReconstructions->size() > 0 )
                    {
                        ::fwAtomsPatch::StructuralCreatorDB::sptr creators =
                                ::fwAtomsPatch::StructuralCreatorDB::getDefault();
                        // Create new model series
                        ::fwAtoms::Object::sptr newModelSeries =
                                creators->create( "::fwMedData::ModelSeries", "1");

                        ::fwAtomsPatch::helper::Object msHelper (newModelSeries);
                        ::fwAtoms::Object::sptr msPatient = ::fwAtoms::Object::dynamicCast(clonedPatient->clone());
                        ::fwAtomsPatch::helper::changeUID(msPatient);
                        msHelper.replaceAttribute("patient", msPatient );

                        ::fwAtoms::Object::sptr msStudy = ::fwAtoms::Object::dynamicCast(clonedStudy->clone());
                        ::fwAtomsPatch::helper::changeUID(msStudy);
                        msHelper.replaceAttribute("study", msStudy );

                        ::fwAtoms::Object::sptr msEquipment = ::fwAtoms::Object::dynamicCast(equipment->clone());
                                                ::fwAtomsPatch::helper::changeUID(msEquipment);
                        msHelper.replaceAttribute("equipment", msEquipment );

                        msHelper.replaceAttribute("modality", ::fwAtoms::String::New("OT") );
                        msHelper.replaceAttribute("instance_uid" ,
                                ::fwAtoms::String::New(::fwTools::UUID::generateUUID()) );
                        msHelper.replaceAttribute("date" , newImgSeries->getAttribute("date")->clone() );
                        msHelper.replaceAttribute("time", newImgSeries->getAttribute("time")->clone() );
                        msHelper.replaceAttribute("performing_physicians_name" ,
                                newImgSeries->getAttribute("performing_physicians_name")->clone() );
                        msHelper.replaceAttribute("description" , newImgSeries->getAttribute("description")->clone() );

                        ::fwAtoms::Sequence::sptr newReconstructions =
                                newModelSeries->getAttribute< ::fwAtoms::Sequence >("reconstruction_db");
                        BOOST_FOREACH( ::fwAtoms::Base::sptr oldRecAtom, oldReconstructions->getValue() )
                        {
                            ::fwAtoms::Object::sptr oldRec = ::fwAtoms::Object::dynamicCast( oldRecAtom );
                            newReconstructions->push_back( newVersions[oldRec] );
                        }

                        series->push_back( newModelSeries );
                    }
                }
            }
        }
    } // End "MedicalWorkspace"
}

} // namespace fwData
} // namespace V2
} // namespace V1
} // namespace fwMDSemanticPatch

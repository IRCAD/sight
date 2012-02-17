/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <map>
#include <iomanip>
#include <cstdlib>
#include <ctime>

#include <boost/foreach.hpp>
#include <boost/algorithm/string/trim.hpp>

#include "fwDataTools/Patient.hpp"
#include "fwDataTools/Image.hpp"


namespace fwDataTools
{

//------------------------------------------------------------------------------

Patient::Patient()
{}

//------------------------------------------------------------------------------

Patient::~Patient()
{}

//------------------------------------------------------------------------------

void Patient::generatePatient(::fwData::Patient::sptr patient,
                              const unsigned char nbStudy,
                              const unsigned char nbAcquisition,
                              const unsigned char nbReconstruction)
{
    const std::string PATIENT_NAME          = "NomPatient1" ;
    const std::string PATIENT_FIRSTNAME     = "PrenomPatient1" ;
    const std::string PATIENT_IDDICOM       = "IDDICOM" ;
    const std::string PATIENT_BIRTHDATE     = "2000-Feb-21 15:00:00";
    const bool PATIENT_SEX                  = true;
    const ::boost::int32_t  PATIENT_DBID    = 457;

    patient->setCRefName( PATIENT_NAME );
    patient->setCRefFirstname( PATIENT_FIRSTNAME );
    patient->setCRefIDDicom(   PATIENT_IDDICOM );
    patient->setCRefBirthdate( ::boost::posix_time::time_from_string( PATIENT_BIRTHDATE ));
    patient->setIsMale( PATIENT_SEX );
    patient->setDbID( PATIENT_DBID );

    for (unsigned char nb=0 ; nb<nbStudy ; ++nb)
    {
        ::fwData::Study::NewSptr study;

        Patient::generateStudy(study, nbAcquisition, nbReconstruction);

        patient->addStudy(study);
    }
}

//------------------------------------------------------------------------------

bool Patient::comparePatient(::fwData::Patient::sptr patient1, ::fwData::Patient::sptr patient2)
{
    bool compare = true;

    compare &= (patient1 && patient2);
    OSLM_ERROR_IF("Patients not initialized", !patient1 || !patient2);

    compare &= patient1->getName() == patient2->getName();
    OSLM_ERROR_IF("Patient have not same name : " << patient1->getName() << " != " << patient2->getName(),
            patient1->getName() != patient2->getName());

    compare &= (patient1->getFirstname() == patient2->getFirstname());
    OSLM_ERROR_IF("Patient have not same first name : " << patient1->getFirstname() << " != " << patient2->getFirstname(),
            patient1->getFirstname() != patient2->getFirstname());

    compare &= (patient1->getBirthdate() == patient2->getBirthdate());
    OSLM_ERROR_IF("Patient have not same birthdate : " << patient1->getBirthdate() << " != " << patient2->getBirthdate(),
            patient1->getBirthdate() != patient2->getBirthdate());

    compare &= (patient1->getIDDicom() == patient2->getIDDicom());
    OSLM_ERROR_IF("Patient have not same Dicom ID : " << patient1->getIDDicom() << " != " << patient2->getIDDicom(),
                  patient1->getIDDicom() != patient2->getIDDicom());

    compare &= (patient1->getIsMale() == patient2->getIsMale());
    OSLM_ERROR_IF("Patient have not same sex : " << patient1->getIsMale() << " != " << patient2->getIsMale(),
            patient1->getIsMale() != patient2->getIsMale());

    compare &= (patient1->getDbID() == patient2->getDbID());
    OSLM_ERROR_IF("Patient have not same DbID : " << patient1->getDbID() << " != " << patient2->getDbID(),
                patient1->getDbID() != patient2->getDbID());

    compare &= (patient1->getStudySize() == patient2->getStudySize());
    OSLM_ERROR_IF("Patient have not same study size : " << patient1->getStudySize() << " != " << patient2->getStudySize(),
                patient1->getStudySize() != patient2->getStudySize());

    std::pair< ::fwData::Patient::StudyIterator, ::fwData::Patient::StudyIterator > pairPatient1 = patient1->getStudies();
    ::fwData::Patient::StudyIterator iter1 = pairPatient1.first;
    ::fwData::Patient::StudyIterator iter2 = patient2->getStudies().first;
    while( iter1 != pairPatient1.second )
    {
        compare &= Patient::compareStudy(*iter1, *iter2);
        iter1++;
        iter2++;
    }


    return compare;
}


//------------------------------------------------------------------------------

void Patient::generateStudy(::fwData::Study::sptr study,
                            const unsigned char nbAcquisition,
                            const unsigned char nbReconstruction)
{
    // studies informations
    const std::string STUDY_HOSPITAL       = "hopital" ;
    const std::string STUDY_MODALITY       = "modality" ;
    const std::string STUDY_ZONE           = "IDDICOM" ;
    const std::string STUDY_RISID          = "risid569" ;
    const std::string STUDY_UID            = "UID569" ;
    const ::boost::int32_t  STUDY_DBID     = 156 ;

    study->setCRefHospital( STUDY_HOSPITAL );
    study->setCRefModality( STUDY_MODALITY );
    study->setCRefAcquisitionZone( STUDY_ZONE );
    study->setCRefRISId( STUDY_RISID );
    study->setCRefUID( STUDY_UID );
    study->setDbID( STUDY_DBID );

    for (unsigned char nb=0 ; nb<nbAcquisition ; ++nb)
    {
        ::fwData::Acquisition::NewSptr acq;

        Patient::generateAcquisition(acq, nbReconstruction);

        study->addAcquisition(acq);
    }
}

//------------------------------------------------------------------------------

bool Patient::compareStudy(::fwData::Study::sptr study1, ::fwData::Study::sptr study2)
{
    bool compare = true;

    compare &= (study1 && study2);
    OSLM_ERROR_IF("Studies not initialized", !study1 || !study2);

    compare &= (study1->getHospital() == study2->getHospital());
    OSLM_ERROR_IF("Studies have not same hopital  : " << study1->getHospital() << " != " << study2->getHospital(),
                  study1->getHospital() != study2->getHospital());


    compare &= (study1->getModality() == study2->getModality());
    OSLM_ERROR_IF("Studies have not same modality  : " << study1->getModality() << " != " << study2->getModality(),
            study1->getModality() != study2->getModality());

    compare &= (study1->getAcquisitionZone() == study2->getAcquisitionZone());
    OSLM_ERROR_IF("Studies have not same acquisition zone  : '" << study1->getAcquisitionZone() << "' != '" << study2->getAcquisitionZone()<<"'",
            study1->getAcquisitionZone() != study2->getAcquisitionZone());

    compare &= (study1->getRefRISId() == study2->getRefRISId());
    OSLM_ERROR_IF("Studies have not same RISId  : " << study1->getRefRISId() << " != " << study2->getRefRISId(),
            study1->getRefRISId() != study2->getRefRISId());

    compare &= (study1->getUID() == study2->getUID());
    OSLM_ERROR_IF("Studies have not same UID  : " << study1->getUID() << " != " << study2->getUID(),
            study1->getUID() != study2->getUID());

    compare &= (study1->getDbID() == study2->getDbID());
    OSLM_ERROR_IF("Studies have not same DbID  : " << study1->getDbID() << " != " << study2->getDbID(),
            study1->getDbID() != study2->getDbID());

    compare &= (study1->getAcquisitionSize() == study2->getAcquisitionSize());
    OSLM_ERROR_IF("Studies have not same size  : " << study1->getAcquisitionSize() << " != " << study2->getAcquisitionSize(),
            study1->getAcquisitionSize() != study2->getAcquisitionSize());

    std::pair< ::fwData::Study::AcquisitionIterator, ::fwData::Study::AcquisitionIterator > pairStudy1 = study1->getAcquisitions();
    ::fwData::Study::AcquisitionIterator iter1 = pairStudy1.first;
    ::fwData::Study::AcquisitionIterator iter2 = study2->getAcquisitions().first;
    while( iter1 != pairStudy1.second )
    {
        compare &= Patient::compareAcquisition(*iter1, *iter2);
        iter1++;
        iter2++;
    }

    return compare;
}

//------------------------------------------------------------------------------

void Patient::generateAcquisition(::fwData::Acquisition::sptr acq,
                                  const unsigned char nbReconstruction)
{
    // aquisitions informations
    const ::boost::uint8_t  ACQ_AXE            = rand()%3 ;
    const bool ACQ_UNSIGNEDFLAG                = false ;
    const ::boost::uint32_t ACQ_INDEX          = 0 ;
    const std::string ACQ_IMAGETYPE            = "refimageType" ;
    const std::string ACQ_IMAGEFORMAT          = "refimageFormat" ;
    const std::string ACQ_CREATIONDATE         = "2007-Feb-26 15:00:00";
    const bool ACQ_ISMAIN                      = true ;
    const bool ACQ_ISNORMALDIR                 = true ;
    const std::string ACQ_UID                  = "uid324" ;
    const ::boost::int32_t ACQ_DBID            = rand()%1000 ;
    const ::boost::uint32_t ACQ_LABOID         = rand()%1000 ;
    const ::boost::uint32_t ACQ_NETID          = rand()%1000 ;
    const std::string ACQ_DATESENDTOLABOAT     = "2007-Feb-26 10:05:00";
    const std::string ACQ_DATERECEIVEFROMLABOAT= "2007-Feb-27 10:05:00";
    const std::string ACQ_DATESENDTOBDDAT      = "2007-Feb-27 10:05:00";
    const std::string ACQ_DATEDISPONIBILITYAT  = "2007-Feb-28 10:05:00";
    const double ACQ_PATIENTSIZE               = rand()%1000 / 100. ;
    const double ACQ_PATIENTWEIGHT             = rand()%1000 / 100. ;
    const double ACQ_RADIATIONS                = rand()%1000 / 100. ;
    const std::string ACQ_MEDICALPRINTER       = "MedicalPrinter" ;
    const std::string ACQ_MEDICALPRINTERCORP   = "MedicalPrinterCorp" ;
    const std::string ACQ_PATIENTPOSITION      = "PatientPosition" ;


    ::fwData::Image::NewSptr img;
    Image::generateRandomImage(img, ::fwTools::Type::create("int16"));
    acq->setImage(img);

    acq->setBitsPerPixel(img->getType().sizeOf()*8);
    acq->setSliceThickness(img->getSpacing()[2]);
    acq->setAxe(ACQ_AXE);
    acq->setUnsignedFlag(ACQ_UNSIGNEDFLAG);
    acq->setAcquisitionIndex(ACQ_INDEX);
    acq->setCRefImageType(ACQ_IMAGETYPE);
    acq->setCRefImageFormat(ACQ_IMAGEFORMAT);
    acq->setCRefCreationDate( ::boost::posix_time::time_from_string(ACQ_CREATIONDATE) );
    acq->setIsMain(ACQ_ISMAIN);
    acq->setIsNormalDir(ACQ_ISNORMALDIR);
    acq->setCRefUID(ACQ_UID);
    acq->setDbID(ACQ_DBID);
    acq->setLaboID(ACQ_LABOID);
    acq->setNetID(ACQ_NETID);
    acq->setCRefDateSendToLaboAt( ::boost::posix_time::time_from_string(ACQ_DATESENDTOLABOAT) );
    acq->setCRefDateReceiveFromLaboAt(::boost::posix_time::time_from_string(ACQ_DATERECEIVEFROMLABOAT));
    acq->setCRefDateSendToBDDAt(::boost::posix_time::time_from_string(ACQ_DATESENDTOBDDAT));
    acq->setCRefDateDisponibilityAt(::boost::posix_time::time_from_string(ACQ_DATEDISPONIBILITYAT));
    acq->setPatientSize(ACQ_PATIENTSIZE);
    acq->setPatientWeight(ACQ_PATIENTWEIGHT);
    acq->setRadiations(ACQ_RADIATIONS);
    acq->setCRefMedicalPrinter(ACQ_MEDICALPRINTER);
    acq->setCRefMedicalPrinterCorp(ACQ_MEDICALPRINTERCORP);
    acq->setCRefPatientPosition(ACQ_PATIENTPOSITION);

    for (unsigned char nb=0 ; nb<nbReconstruction ; ++nb)
    {
        ::fwData::Reconstruction::NewSptr rec;

        Patient::generateReconstruction(rec);

        acq->addReconstruction(rec);
    }
}

//------------------------------------------------------------------------------

bool Patient::compareAcquisition(::fwData::Acquisition::sptr acquisition1, ::fwData::Acquisition::sptr acquisition2)
{
    bool compare = true;

    compare &= (acquisition1 && acquisition2);
    OSLM_ERROR_IF("Acquisitions not initialized", !acquisition1 || !acquisition2);

    compare &= (acquisition1->getBitsPerPixel() == acquisition2->getBitsPerPixel());
    OSLM_ERROR_IF("Acquisitions have not same bit per pixel  : " << (int)acquisition1->getBitsPerPixel() << " != " << (int)acquisition2->getBitsPerPixel(),
            acquisition1->getBitsPerPixel() != acquisition2->getBitsPerPixel());

    compare &= (acquisition1->getSliceThickness() == acquisition2->getSliceThickness());
    OSLM_ERROR_IF("Acquisitions have not same slice thickness  : " << acquisition1->getSliceThickness() << " != " << acquisition2->getSliceThickness(),
            acquisition1->getSliceThickness() != acquisition2->getSliceThickness());

    compare &= (acquisition1->getAxe() == acquisition2->getAxe());
    OSLM_ERROR_IF("Acquisitions have not same axe  : " << static_cast<int> (acquisition1->getAxe()) << " != " << static_cast<int> (acquisition2->getAxe()),
            acquisition1->getAxe() != acquisition2->getAxe());

    compare &= (acquisition1->getUnsignedFlag() == acquisition2->getUnsignedFlag());
    OSLM_ERROR_IF("Acquisitions have not same UnsignedFlag : " << acquisition1->getUnsignedFlag() << " != " << acquisition2->getUnsignedFlag(),
            acquisition1->getUnsignedFlag() != acquisition2->getUnsignedFlag());

    compare &= (acquisition1->getAcquisitionIndex() == acquisition2->getAcquisitionIndex());
    OSLM_ERROR_IF("Acquisitions have not same index : " << acquisition1->getAcquisitionIndex() << " != " << acquisition2->getAcquisitionIndex(),
            acquisition1->getAcquisitionIndex() != acquisition2->getAcquisitionIndex());

    compare &= (acquisition1->getImageType() == acquisition2->getImageType());
    OSLM_ERROR_IF("Acquisitions have not same image type : '" << acquisition1->getImageType() << "' != '" << acquisition2->getImageType()<< "'",
            acquisition1->getImageType() != acquisition2->getImageType());

    compare &= (acquisition1->getImageFormat() == acquisition2->getImageFormat());
    OSLM_ERROR_IF("Acquisitions have not same image format : '" << acquisition1->getImageFormat() << "' != '" << acquisition2->getImageFormat()<<"'",
            acquisition1->getImageFormat() != acquisition2->getImageFormat());

    compare &= (acquisition1->getIsMain() == acquisition2->getIsMain());
    OSLM_ERROR_IF("Acquisitions have not same IsMain : " << acquisition1->getIsMain() << " != " << acquisition2->getIsMain(),
            acquisition1->getIsMain() != acquisition2->getIsMain());

    compare &= (acquisition1->getIsNormalDir() == acquisition2->getIsNormalDir());
    OSLM_ERROR_IF("Acquisitions have not same normal dir : " << acquisition1->getIsNormalDir() << " != " << acquisition2->getIsNormalDir(),
            acquisition1->getIsNormalDir() != acquisition2->getIsNormalDir());

    compare &= (acquisition1->getUID() == acquisition2->getUID());
    OSLM_ERROR_IF("Acquisitions have not same UID : " << acquisition1->getUID() << " != " << acquisition2->getUID(),
            acquisition1->getUID() != acquisition2->getUID());

    compare &= (acquisition1->getDbID() == acquisition2->getDbID());
    OSLM_ERROR_IF("Acquisitions have not same dbID : " << acquisition1->getDbID() << " != " << acquisition2->getDbID(),
            acquisition1->getDbID() != acquisition2->getDbID());

    compare &= (acquisition1->getLaboID() == acquisition2->getLaboID());
    OSLM_ERROR_IF("Acquisitions have not same laboID : " << acquisition1->getLaboID() << " != " << acquisition2->getLaboID(),
            acquisition1->getLaboID() != acquisition2->getLaboID());

    compare &= (acquisition1->getNetID() == acquisition2->getNetID());
    OSLM_ERROR_IF("Acquisitions have not same NetID : " << acquisition1->getNetID() << " != " << acquisition2->getNetID(),
            acquisition1->getNetID() != acquisition2->getNetID());

    compare &= (acquisition1->getDateSendToLaboAt() == acquisition2->getDateSendToLaboAt());
    OSLM_ERROR_IF("Acquisitions have not same DateSendToLaboAt : " << acquisition1->getDateSendToLaboAt() << " != " << acquisition2->getDateSendToLaboAt(),
            acquisition1->getDateSendToLaboAt() != acquisition2->getDateSendToLaboAt());

    compare &= (acquisition1->getDateReceiveFromLaboAt() == acquisition2->getDateReceiveFromLaboAt());
    OSLM_ERROR_IF("Acquisitions have not same DateReceiveFromLaboAt : " << acquisition1->getDateReceiveFromLaboAt() << " != " << acquisition2->getDateReceiveFromLaboAt(),
            acquisition1->getDateReceiveFromLaboAt() != acquisition2->getDateReceiveFromLaboAt());

    compare &= (acquisition1->getDateSendToBDDAt() == acquisition2->getDateSendToBDDAt());
    OSLM_ERROR_IF("Acquisitions have not same DateSendToBDDAt : " << acquisition1->getDateSendToBDDAt() << " != " << acquisition2->getDateSendToBDDAt(),
            acquisition1->getDateSendToBDDAt() != acquisition2->getDateSendToBDDAt());

    compare &= (acquisition1->getDateDisponibilityAt() == acquisition2->getDateDisponibilityAt());
    OSLM_ERROR_IF("Acquisitions have not same DateDisponibilityAt : " << acquisition1->getDateDisponibilityAt() << " != " << acquisition2->getDateDisponibilityAt(),
            acquisition1->getDateDisponibilityAt() != acquisition2->getDateDisponibilityAt());

    compare &= (acquisition1->getPatientSize() == acquisition2->getPatientSize());
    OSLM_ERROR_IF("Acquisitions have not same patient size : " << acquisition1->getPatientSize() << " != " << acquisition2->getPatientSize(),
            acquisition1->getPatientSize() != acquisition2->getPatientSize());

    compare &= (acquisition1->getPatientWeight() == acquisition2->getPatientWeight());
    OSLM_ERROR_IF("Acquisitions have not same patient weight : " << acquisition1->getPatientWeight() << " != " << acquisition2->getPatientWeight(),
            acquisition1->getPatientWeight() != acquisition2->getPatientWeight());

    compare &= (acquisition1->getRadiations() == acquisition2->getRadiations());
    OSLM_ERROR_IF("Acquisitions have not same radiation : " << acquisition1->getRadiations() << " != " << acquisition2->getRadiations(),
            acquisition1->getRadiations() != acquisition2->getRadiations());

    compare &= (acquisition1->getMedicalPrinter() == acquisition2->getMedicalPrinter());
    OSLM_ERROR_IF("Acquisitions have not same Medical Printer : '" << acquisition1->getMedicalPrinter() << "' != '" << acquisition2->getMedicalPrinter()<<"'",
            acquisition1->getMedicalPrinter() != acquisition2->getMedicalPrinter());

    compare &= (acquisition1->getMedicalPrinterCorp() == acquisition2->getMedicalPrinterCorp());
    OSLM_ERROR_IF("Acquisitions have not same MedicalPrinterCorp : '" << acquisition1->getMedicalPrinterCorp() << "' != '" << acquisition2->getMedicalPrinterCorp() <<"'",
            acquisition1->getMedicalPrinterCorp() != acquisition2->getMedicalPrinterCorp());

    compare &= (acquisition1->getPatientPosition() == acquisition2->getPatientPosition());
    OSLM_ERROR_IF("Acquisitions have not same patient position : '" << acquisition1->getPatientPosition() << "' != '" << acquisition2->getPatientPosition()<<"'",
            acquisition1->getPatientPosition() != acquisition2->getPatientPosition());

    compare &= (acquisition1->getReconstructionSize() == acquisition2->getReconstructionSize());
    OSLM_ERROR_IF("Acquisitions have not same reconstruction size : " << acquisition1->getReconstructionSize() << " != " << acquisition2->getReconstructionSize(),
            acquisition1->getReconstructionSize() != acquisition2->getReconstructionSize());

    std::pair< ::fwData::Acquisition::ReconstructionIterator, ::fwData::Acquisition::ReconstructionIterator > pairAcquisition1 = acquisition1->getReconstructions();
    ::fwData::Acquisition::ReconstructionIterator iter1 = pairAcquisition1.first;
    ::fwData::Acquisition::ReconstructionIterator iter2 = acquisition2->getReconstructions().first;
    while( iter1 != pairAcquisition1.second )
    {
        compare &= Patient::compareReconstruction(*iter1, *iter2);
        iter1++;
        iter2++;
    }

    compare &= Image::compareImage(acquisition1->getImage(), acquisition2->getImage());

    return compare;
}

//------------------------------------------------------------------------------

void Patient::generateReconstruction(::fwData::Reconstruction::sptr rec)
{
    ::fwData::Image::NewSptr img;
    Image::generateRandomImage(img, ::fwTools::Type::create("uint16"));
    rec->setImage(img);

    ::fwData::Material::NewSptr material;
    rec->setMaterial(material);

    ::fwData::Mesh::NewSptr mesh;
    rec->setMesh(mesh);
}

//------------------------------------------------------------------------------

bool Patient::compareReconstruction(::fwData::Reconstruction::sptr reconstruction1, ::fwData::Reconstruction::sptr reconstruction2)
{
    bool compare = true;

    compare &= (reconstruction1 && reconstruction2);
    OSLM_ERROR_IF("Reconstructions not initialized", !reconstruction1 || !reconstruction2);

    compare &= (reconstruction1->getIsVisible() == reconstruction2->getIsVisible());
    OSLM_ERROR_IF("Reconstructions have not same visibility : " << reconstruction1->getIsVisible() << " != " << reconstruction2->getIsVisible(),
            reconstruction1->getIsVisible() != reconstruction2->getIsVisible());

    compare &= (reconstruction1->getOrganName() == reconstruction2->getOrganName());
    OSLM_ERROR_IF("Reconstructions have not same organ name : " << reconstruction1->getOrganName() << " != " << reconstruction2->getOrganName(),
            reconstruction1->getOrganName() != reconstruction2->getOrganName());

    compare &= (reconstruction1->getStructureType() == reconstruction2->getStructureType());
    OSLM_ERROR_IF("Reconstructions have not same structure type : " << reconstruction1->getStructureType() << " != " << reconstruction2->getStructureType(),
            reconstruction1->getStructureType() != reconstruction2->getStructureType());

    compare &= (reconstruction1->getIsClosed() == reconstruction2->getIsClosed());
    OSLM_ERROR_IF("Reconstructions have not same closing : " << reconstruction1->getIsClosed() << " != " << reconstruction2->getIsClosed(),
            reconstruction1->getIsClosed() != reconstruction2->getIsClosed());

    compare &= (reconstruction1->getIsAutomatic() == reconstruction2->getIsAutomatic());
    OSLM_ERROR_IF("Reconstructions have not same IsAutomatic : " << reconstruction1->getIsAutomatic() << " != " << reconstruction2->getIsAutomatic(),
            reconstruction1->getIsAutomatic() != reconstruction2->getIsAutomatic());

    compare &= (reconstruction1->getAvgVolume() == reconstruction2->getAvgVolume());
    OSLM_ERROR_IF("Reconstructions have not same volume : " << reconstruction1->getAvgVolume() << " != " << reconstruction2->getAvgVolume(),
            reconstruction1->getAvgVolume() != reconstruction2->getAvgVolume());

    compare &= (reconstruction1->getVolStdDeviation() == reconstruction2->getVolStdDeviation());
    OSLM_ERROR_IF("Reconstructions have not same VolStdDeviation : " << reconstruction1->getVolStdDeviation() << " != " << reconstruction2->getVolStdDeviation(),
            reconstruction1->getVolStdDeviation() != reconstruction2->getVolStdDeviation());

    compare &= (reconstruction1->getVolPctConfidence() == reconstruction2->getVolPctConfidence());
    OSLM_ERROR_IF("Reconstructions have not same VolPctConfidence : " << reconstruction1->getVolPctConfidence() << " != " << reconstruction2->getVolPctConfidence(),
            reconstruction1->getVolPctConfidence() != reconstruction2->getVolPctConfidence());

    compare &= (reconstruction1->getReconstructionTime() == reconstruction2->getReconstructionTime());
    OSLM_ERROR_IF("Reconstructions have not same time : " << reconstruction1->getReconstructionTime() << " != " << reconstruction2->getReconstructionTime(),
            reconstruction1->getReconstructionTime() != reconstruction2->getReconstructionTime());

    compare &= (reconstruction1->getMaskGenerated() == reconstruction2->getMaskGenerated());
    OSLM_ERROR_IF("Reconstructions have not same MaskGenerated : " << reconstruction1->getMaskGenerated() << " != " << reconstruction2->getMaskGenerated(),
            reconstruction1->getMaskGenerated() != reconstruction2->getMaskGenerated());

    compare &= (reconstruction1->getLevel() == reconstruction2->getLevel());
    OSLM_ERROR_IF("Reconstructions have not same level : " << reconstruction1->getLevel() << " != " << reconstruction2->getLevel(),
            reconstruction1->getLevel() != reconstruction2->getLevel());

    compare &= (reconstruction1->getLabel() == reconstruction2->getLabel());
    OSLM_ERROR_IF("Reconstructions have not same label : " << reconstruction1->getLabel() << " != " << reconstruction2->getLabel(),
            reconstruction1->getLabel() != reconstruction2->getLabel());

    compare &= (reconstruction1->getGenerated3D() == reconstruction2->getGenerated3D());
    OSLM_ERROR_IF("Reconstructions have not same Generated3D : " << reconstruction1->getGenerated3D() << " != " << reconstruction2->getGenerated3D(),
            reconstruction1->getGenerated3D() != reconstruction2->getGenerated3D());

    compare &= (reconstruction1->getType3D() == reconstruction2->getType3D());
    OSLM_ERROR_IF("Reconstructions have not same Type3D : " << reconstruction1->getType3D() << " != " << reconstruction2->getType3D(),
            reconstruction1->getType3D() != reconstruction2->getType3D());

    compare &= (reconstruction1->getPath() == reconstruction2->getPath());
    OSLM_ERROR_IF("Reconstructions have not same path : " << reconstruction1->getPath() << " != " << reconstruction2->getPath(),
            reconstruction1->getPath() != reconstruction2->getPath());

    compare &= (reconstruction1->getDbID() == reconstruction2->getDbID());
    OSLM_ERROR_IF("Reconstructions have not same dbID : " << reconstruction1->getDbID() << " != " << reconstruction2->getDbID(),
            reconstruction1->getDbID() != reconstruction2->getDbID());

    compare &= Image::compareImage(reconstruction1->getImage(), reconstruction2->getImage());
    //compareMaterial(reconstruction1->getMaterial(), reconstruction2->getMaterial());
    //compareMesh(reconstruction1->getMesh(), reconstruction2->getMesh());

    return compare;
}

//------------------------------------------------------------------------------

} // namespace fwDataTools

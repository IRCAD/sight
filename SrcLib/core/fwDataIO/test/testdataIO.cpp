/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/filesystem/operations.hpp>

#include <mfo/Patient.h>
#include <fwCore/base.hpp>
#include <xmlmfo/xmlmfo.h>

#include <mfo/mfoToData.hpp>
#include <mfo/dataToMfo.hpp>
#include <fwData/Patient.hpp>
#include <fwData/Study.hpp>
#include <fwData/DictionaryOrgan.hpp>
#include <fwData/Dictionary.hpp>

#include <fwDataIO/Serializer.hpp>

#include <fwDataIO/ObjectToStream.hpp>

#include <fwDataIO/policy/ListManagedPolicy.hpp>
#include <fwDataIO/policy/PatientFolderPathPolicy.hpp>

//------------------------------------------------------------------------------

mfo::CPatient * loadMfoPatient( const boost::filesystem::path & patientXml, const boost::filesystem::path & dicoXml )
{
    mfo::CPatient * mfoPatient = NULL;
    mfoPatient = xmlmfo::CXmlMfo::GetPatientAndDictionary( patientXml.string()/*native_file_string()*/, dicoXml.string()/*native_file_string()*/ );
    SLM_ASSERT("mfoPatient not instanced", mfoPatient);

    return mfoPatient;
}

//------------------------------------------------------------------------------

void test_1(const boost::filesystem::path & patientXml, const boost::filesystem::path & dicoXml)
{
    SLM_TRACE("test_1");

    // Load mfo patient
    mfo::CPatient * mfoPatient = loadMfoPatient( patientXml, dicoXml );

    // Convert mfo patient 2 data patient
    ::boost::shared_ptr< ::fwData::Patient > dataPatient = ::mfo::convertToDataPatient( mfoPatient );

    // Print patient
    OSLM_INFO ( "Patient print format (v1) = \n" << ::fwDataIO::ObjectToStream::toString( dataPatient, 0) );
    OSLM_INFO ( "Patient print format (v2) = \n" << ::fwDataIO::ObjectToStream::toString( dataPatient, 1) );
    OSLM_INFO ( "Patient print format (v3) = \n" << ::fwDataIO::ObjectToStream::toString( dataPatient, 2) );

    // MFO TO DATA : Dictionary convertion
    // initialization of dictionary (singleton)
    ::boost::shared_ptr< ::fwData::Dictionary > dataDictionary = ::mfo::convertToDataDictionary();

    // Print Dico
    OSLM_INFO ( "Patient print format (v3) = \n" << ::fwDataIO::ObjectToStream::toString( dataDictionary, 2) );
}

//------------------------------------------------------------------------------

void test_2( const boost::filesystem::path & patientXml,
             const boost::filesystem::path & dicoXml,
             const boost::filesystem::path & saveDirectoryForDataPatient )
{
    SLM_TRACE("test_2");

    // Load mfo patient
    mfo::CPatient * mfoPatient = loadMfoPatient( patientXml, dicoXml );

    // Convert mfo 2 data
    ::boost::shared_ptr< ::fwData::Patient > dataPatient = ::mfo::convertToDataPatient( mfoPatient );
    ::boost::shared_ptr< ::fwData::Dictionary > dataDictionary = ::mfo::convertToDataDictionary();

    // choose saving Policy
    ::boost::shared_ptr< ::fwDataIO::ListManagedPolicy > spolicy( new ::fwDataIO::ListManagedPolicy );
    ::boost::shared_ptr< ::fwDataIO::PatientFolderPathPolicy >  fpolicy( new ::fwDataIO::PatientFolderPathPolicy);
    spolicy->addSplitClass("Study");
    spolicy->addSplitClass("Acquisition");
    spolicy->addSplitClass("Reconstruction");




    // Save patient
    ::fwDataIO::Serializer serializer;
    serializer.rootFolder() = saveDirectoryForDataPatient;
    serializer.setSplitPolicy( spolicy );
    serializer.setPathPolicy( fpolicy );
    serializer.serialize(dataPatient);

    // Save dico
    spolicy->addSplitClass("DictionaryOrgan");
    serializer.serialize(dataDictionary);
}

//------------------------------------------------------------------------------

void test_3( const boost::filesystem::path & patientXml )
{
    SLM_TRACE("test_3");

    ::fwDataIO::Serializer serializer;
    ::boost::shared_ptr< fwTools::Object>  dataPatient = serializer.deSerialize( patientXml );
    OSLM_INFO ( "Patient loaded = \n" << patientXml.string() << ::fwDataIO::ObjectToStream::toString( dataPatient, 2) );
}

//------------------------------------------------------------------------------

void test_4( const boost::filesystem::path & patientXml,
             const boost::filesystem::path & saveDirectoryForDataPatient )
{
    SLM_TRACE("test_4");

    // Load
    ::fwDataIO::Serializer serializer;
    ::boost::shared_ptr< fwTools::Object>  dataPatient = serializer.deSerialize( patientXml );

    // Save
    serializer.rootFolder() = saveDirectoryForDataPatient;
    serializer.serialize(dataPatient);
}

//------------------------------------------------------------------------------

void test_5( const boost::filesystem::path & patientXml,
             const boost::filesystem::path & dicoXml,
             const boost::filesystem::path & saveDirectoryForDataPatient )
{
    SLM_TRACE("test_5");

    // Load
    ::fwDataIO::Serializer serializer;
    ::boost::shared_ptr< fwTools::Object>  dataPatient = serializer.deSerialize( patientXml );
    ::boost::shared_ptr< fwTools::Object>  dataDictionary = serializer.deSerialize( dicoXml );

    // Save
    serializer.rootFolder() = saveDirectoryForDataPatient;
    serializer.serialize(dataPatient);
    serializer.serialize(dataDictionary);
}

//------------------------------------------------------------------------------

void test_6( const boost::filesystem::path & patientXml,
             const boost::filesystem::path & dicoXml,
             const boost::filesystem::path & saveDirectoryForMfoPatient )
{
    SLM_TRACE("test_6");

    // Load
    ::fwDataIO::Serializer serializer;
    ::boost::shared_ptr< fwTools::Object>  dataPatient = serializer.deSerialize( patientXml );
    ::boost::shared_ptr< fwTools::Object>  dataDictionary = serializer.deSerialize( dicoXml );

    // DATA TO MFO
    ::mfo::convertToMfoDictionary( boost::dynamic_pointer_cast< ::fwData::Dictionary >(dataDictionary) ); // update the singleton mfo::DictionaryOrgan
    std::cout << "SIZE TEMP = " << ::mfo::DictionaryOrgan::getAllOrgans().size() << std::endl;
    mfo::CPatient* newMfoPatient = ::mfo::convertToMfoPatient( boost::dynamic_pointer_cast< ::fwData::Patient >(dataPatient) );
    std::cout << "SIZE TEMP = " << ::mfo::DictionaryOrgan::getAllOrgans().size() << std::endl;
    xmlmfo::CXmlMfo::WritePatientAndDictionary(saveDirectoryForMfoPatient.string(),newMfoPatient);
}
//------------------------------------------------------------------------------

void test_7( std::string filename)
{
    SLM_TRACE("test_7");
    FILE *fichier;
    char buf[1001];

    if((fichier = fopen(filename.c_str(), "r")) != NULL)
    {
        while (!feof(fichier))
        {
         fscanf(fichier,"%s",buf);
         OSLM_INFO("ligne lue  "<<buf);
        }
        fclose(fichier);
    }
}

//------------------------------------------------------------------------------

void test_8( std::string filename)
{
    SLM_TRACE("test_8");

    std::string newFilename;
    ::boost::filesystem::path mypath (filename);

    OSLM_INFO("nom apres conversion boost  "<<mypath);

    FILE *fichier;
    char buf[1001];
    if((fichier = fopen(filename.c_str(), "r")) != NULL)
    {
        while (!feof(fichier))
        {
         fscanf(fichier,"%s",buf);
         OSLM_INFO("ligne lue  "<<buf);
        }
        fclose(fichier);
    }
}

//------------------------------------------------------------------------------

int main(int argc, char **argv)
{

    if (argc <= 2)
    {
        SLM_INFO("Syntax helper =>")
        SLM_INFO("./testdataIO -t1 mfoPatient.xml mfoDico.xml : load an mfo patient, convert and print out new data structures.");
        SLM_INFO("./testdataIO -t2 mfoPatient.xml mfoDico.xml saveDirectoryForDataPatient : load an mfo patient, convert and save new data structures.");
        SLM_INFO("./testdataIO -t3 dataObject.xml : load an data object and print out his structure.");
        SLM_INFO("./testdataIO -t4 dataObject.xml saveDirectoryForDataPatient : load an data object and save it in another folder");
        SLM_INFO("./testdataIO -t5 dataPatient.xml dataDico.xml saveDirectoryForDataPatient : load an patient and dico data and save it in another folder");
        SLM_INFO("./testdataIO -t6 dataPatient.xml dataDico.xml saveDirectoryForMfoPatient : load a patient object and a dico object save it in another folder with xmlmfo");
        SLM_INFO("./testdataIO -t7 fichier.txt : test window and linux pathes with fopen method");
        SLM_INFO("./testdataIO -t8 fichier.txt : test window and linux pathes with fopen method and boost conversion");
        return 0;
    }

    std::string options = argv[1];

    if ( options == "-t1" )
    {
         boost::filesystem::path patientXml (argv[2]);
         boost::filesystem::path dicoXml (argv[3]);
         test_1(patientXml,dicoXml);
    }
    else if ( options == "-t2" )
    {
         boost::filesystem::path patientXml (argv[2]);
         boost::filesystem::path dicoXml (argv[3]);
         boost::filesystem::path saveDirectoryForDataPatient (argv[4]);
         test_2( patientXml, dicoXml, saveDirectoryForDataPatient );
    }
    else if ( options == "-t3" )
    {
         boost::filesystem::path patientXml (argv[2]);
         test_3( patientXml );
    }
    else if ( options == "-t4" )
    {
        boost::filesystem::path patientXml (argv[2]);
        boost::filesystem::path saveDirectoryForDataPatient (argv[3]);
        test_4( patientXml, saveDirectoryForDataPatient );
    }
    else if ( options == "-t5" )
    {
        boost::filesystem::path patientXml (argv[2]);
        boost::filesystem::path dicoXml (argv[3]);
        boost::filesystem::path saveDirectoryForDataPatient (argv[4]);
        test_5( patientXml, dicoXml, saveDirectoryForDataPatient );
    }
    else if ( options == "-t6" )
    {
        boost::filesystem::path patientXml (argv[2]);
        boost::filesystem::path dicoXml (argv[3]);
        boost::filesystem::path saveDirectoryForMfoPatient (argv[4]);
        test_6( patientXml, dicoXml, saveDirectoryForMfoPatient );
    }
    else if ( options == "-t7" )
    {
        std::string filename(argv[2]);
        test_7(filename);
    }
    else if ( options == "-t8" )
    {
        std::string filename(argv[2]);
        test_8(filename);
    }

}

/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwDicomIOFilter/helper/Filter.hpp>
#include <fwMedData/SeriesDB.hpp>
#include <fwDcmtkIO/SeriesDBReader.hpp>

#include <boost/program_options.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

#include <stdlib.h>
#include <string>
#include <iostream>
#include <fstream>

/** DicomMetadataJsonGenerator/src/main.cpp
 *
 *********************
 * Software : DicomMetadataJsonGenerator
 *********************
 * Generate Json files used for ::fwDcmtkIO::ut::SeriesDBReaderTest unit test.
 * HELP  : DicomMetadataJsonGenerator.exe --help
 * USE :   DicomMetadataJsonGenerator.exe <options>
 * Allowed options:
 *   -h [ --help ]           produce help message
 *   -i [ --input ] arg      set the input folder
 */
int main(int argc, char** argv)
{
    // Declare the supported options.
    ::boost::program_options::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "produce help message")
        ("input,i", ::boost::program_options::value< std::string >(), "set input folder")
    ;

    // Manage the options
    ::boost::program_options::variables_map vm;
    ::boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
    ::boost::program_options::notify(vm);

    if (vm.count("help"))
    {
        std::cout << desc << "\n";
        return EXIT_SUCCESS;
    }
    else if(!vm.count("input"))
    {
        std::cout << "You must specify an input folder containing the Dicom folders." << "\n";
        return EXIT_FAILURE;
    }

    // Get paths
    const ::boost::filesystem::path input(vm["input"].as< std::string >());
    const ::boost::filesystem::path output(input.string() + "/DICOMFILES");

    if(!::boost::filesystem::exists(input) || !::boost::filesystem::is_directory(input))
    {
        std::cout << "The specified input folder " << input << " is not a directory." << "\n";
        return EXIT_FAILURE;
    }

    //Delete old DICOMFILES
    if(::boost::filesystem::exists(output))
    {
        ::boost::filesystem::remove(output);
    }

    // Create a map to store Dicom files and meta
    typedef std::vector< std::string > DicomVectorType;
    DicomVectorType dicomVector;

    // List of files used for unit testing
    dicomVector.push_back("01-CT-DICOM_LIVER");
    dicomVector.push_back("08-CT-PACS");
    dicomVector.push_back("09-CT-PACS");
    dicomVector.push_back("42-OT-BARRE-MONO2-8-colon");
    dicomVector.push_back("46-MR-BARRE-MONO2-12-shoulder");
    dicomVector.push_back("71-CT-DICOM_SEG");
    dicomVector.push_back("71-CT-DICOM_SF");
    dicomVector.push_back("71-CT-DICOM_SR");
    dicomVector.push_back("71-CT-DICOM_3DSR");
    dicomVector.push_back("82-MR-SAGITTAL-KNEE");

    // Add other Dicom test images

    // Read series
    for(std::string file :  dicomVector)
    {
        OSLM_WARN(input.string() << "/" << file);
        ::fwDcmtkIO::SeriesDBReader::sptr myLoader = ::fwDcmtkIO::SeriesDBReader::New();
        ::fwMedData::SeriesDB::sptr dummy          = ::fwMedData::SeriesDB::New();
        myLoader->setObject(dummy);
        myLoader->setFolder(input.string() + "/" + file);
        myLoader->readDicomSeries();

        std::vector< ::fwMedData::DicomSeries::sptr > dicomSeriesContainer = myLoader->getDicomSeries();
        ::fwDicomIOFilter::IFilter::sptr filter = ::fwDicomIOFilter::factory::New(
            "::fwDicomIOFilter::custom::DefaultDicomFilter");
        ::fwDicomIOFilter::helper::Filter::applyFilter(dicomSeriesContainer,filter, true);

        std::ofstream outputFile;
        outputFile.open(output.c_str(), std::ios::out | std::ios::app);

        // Write first instance of each series to the DICOMFILES file
        for(::fwMedData::DicomSeries::sptr series :  dicomSeriesContainer)
        {
            outputFile << file << "/" << series->getLocalDicomPaths().begin()->second.filename().c_str() << "\n";
        }

        outputFile.close();

    }

    // Execute shellscript to create json files
    const std::string script = "share/fwDcmtkIO_0-1/test/DicomMetadataJsonGenerator.sh " + input.string();
    system(script.c_str());

    return EXIT_SUCCESS;
}


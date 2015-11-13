/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <stdlib.h>
#include <string>
#include <iostream>
#include <fstream>

#include <boost/program_options.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

#include <fwMedData/SeriesDB.hpp>
#include <fwDicomIOExt/dcmtk/SeriesDBReader.hpp>


/** \file main.cpp
 *********************
 * Software : DicomMetadataJsonGenerator
 *********************
 * Generate Json files used for ::fwDicomIOExt::ut::SeriesDBReaderTest unit test.
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
    dicomVector.push_back("01-CT-DICOM_LIVER");
    dicomVector.push_back("02-CT-DICOM_POUMON");
    dicomVector.push_back("03-CT-DICOM_GENOU");
    dicomVector.push_back("04-CT-DICOM_SCRAT");
    dicomVector.push_back("05-CT-DICOM_DEFORMATION_1");
    dicomVector.push_back("06-CT-DICOM_DEFORMATION_2");
    dicomVector.push_back("07-CT-DVD");
    dicomVector.push_back("08-CT-PACS");  // Multi-Volumes
    dicomVector.push_back("09-CT-PACS");
    dicomVector.push_back("10-CT-PACS");
    dicomVector.push_back("11-CT-PACS");
    dicomVector.push_back("12-CT-PACS");
    dicomVector.push_back("13-CT-PACS");
    dicomVector.push_back("14-CT-PACS");
    dicomVector.push_back("15-CT-PACS");
    dicomVector.push_back("16-CT-PACS");
    dicomVector.push_back("17-CT-PACS");
    dicomVector.push_back("18-CT-PACS");
    dicomVector.push_back("19-CT-PACS");
    dicomVector.push_back("20-CT-PACS");
    dicomVector.push_back("21-CT-PACS");
    dicomVector.push_back("22-CT-PACS");
    dicomVector.push_back("23-CT-BARRE-MONO2-16-brain");
    dicomVector.push_back("24-CT-BARRE-MONO2-16-chest");
    dicomVector.push_back("25-CT-BARRE-MONO2-16-ort");
    dicomVector.push_back("26-CT-OSIRIX-BREBIX");
    dicomVector.push_back("27-CT-OSIRIX-MAGIX/");
    dicomVector.push_back("28-CT-OSIRIX-MANIX");
    dicomVector.push_back("29-CT-OSIRIX-OBELIX");
    dicomVector.push_back("30-CT-OSIRIX-PHENIX");
    dicomVector.push_back("31-CT-JOHAN-DICOM");
    dicomVector.push_back("32-CT-JOHAN-GATING-TESTS");
    dicomVector.push_back("33-CT-JOHAN-GATING-TESTS");
    dicomVector.push_back("34-CT-JOHAN-DICOM");
    dicomVector.push_back("35-CT-JOHAN-DICOM");
    dicomVector.push_back("36-CT-JOHAN-DICOM");

    // Add other Dicom test images

    // Read series
    for(std::string file :  dicomVector)
    {
        OSLM_WARN(input.string() << "/" << file);
        ::fwDicomIOExt::dcmtk::SeriesDBReader::sptr myLoader = ::fwDicomIOExt::dcmtk::SeriesDBReader::New();
        ::fwMedData::SeriesDB::sptr dummy                    = ::fwMedData::SeriesDB::New();
        myLoader->setObject(dummy);
        myLoader->setFolder(input.string() + "/" + file);

        myLoader->read();

        std::ofstream outputFile;
        outputFile.open(output.c_str(), std::ios::out | std::ios::app);

        // Write first instance of each series to the DICOMFILES file
        for(::fwDicomData::DicomSeries::sptr series :  myLoader->getDicomSeries())
        {
            outputFile << file << "/" << series->getLocalDicomPaths().begin()->second.filename().c_str() << "\n";
        }

        outputFile.close();

    }

    // Execute shellscript to create json files
    const std::string script = "share/fwDicomIOExt_0-1/test/DicomMetadataJsonGenerator.sh " + input.string();
    system(script.c_str());

    return EXIT_SUCCESS;
}


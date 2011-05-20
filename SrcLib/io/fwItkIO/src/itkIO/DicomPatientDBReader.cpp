/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <sstream>

#include <itkOrientedImage.h>
#include <itkGDCMImageIO.h>
#include <itkGDCMSeriesFileNames.h>
#include <itkImageSeriesReader.h>
#include <itkImageFileWriter.h>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>

#include <fwCore/base.hpp>

#include <fwData/PatientDB.hpp>
#include <fwData/Patient.hpp>
#include <fwData/Study.hpp>
#include <fwData/Acquisition.hpp>
#include <fwData/Image.hpp>

#include <fwTools/Factory.hpp>
#include <fwTools/ClassRegistrar.hpp>
#include <fwTools/dateAndTime.hpp>

#include "itkIO/DicomPatientDBReader.hpp"
#include "itkIO/itk.hpp"
#include "itkIO/types.hpp"
#include "itkIO/helper/ProgressItkToFw.hpp"

REGISTER_BINDING_BYCLASSNAME( ::fwDataIO::reader::IObjectReader , ::itkIO::DicomPatientDBReader, ::itkIO::DicomPatientDBReader );

namespace itkIO
{

//------------------------------------------------------------------------------

DicomPatientDBReader::DicomPatientDBReader() :
::fwData::location::enableFolder< IObjectReader >(this)
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

DicomPatientDBReader::~DicomPatientDBReader()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void DicomPatientDBReader::searchRecursivelyDirectories
( const ::boost::filesystem::path & path, ::std::vector< ::boost::filesystem::path > & dirs )
{
    // Add directory
    dirs.push_back(path);
    for( ::boost::filesystem::directory_iterator it(path); it != boost::filesystem::directory_iterator(); ++it)
    {
        if( ::boost::filesystem::is_directory(*it) )
        {
            searchRecursivelyDirectories( *it, dirs );
        }
    }
}

//------------------------------------------------------------------------------

::fwData::PatientDB::sptr DicomPatientDBReader::createPatientDB( const ::boost::filesystem::path dicomDir )
{
    SLM_TRACE_FUNC();
    ::fwData::PatientDB::sptr  patientDB   = this->getConcreteObject();

    ::std::vector< ::boost::filesystem::path > dirs;
    searchRecursivelyDirectories( dicomDir, dirs );

    BOOST_FOREACH(::boost::filesystem::path dir, dirs)
    {
        addPatients( patientDB, dir );
    }

    return patientDB;
}

//------------------------------------------------------------------------------

std::string DicomPatientDBReader::getDicomValue( ::itk::GDCMImageIO::Pointer dicomIO , const std::string entryId )
{
    std::string value = "";

    typedef itk::MetaDataDictionary   DictionaryType;
    const  DictionaryType & dictionary = dicomIO->GetMetaDataDictionary();

    // In this example, we are only interested in the DICOM tags that can be
    // represented as strings. We declare therefore a \doxygen{MetaDataObject} of
    // string type in order to receive those particular values.
    typedef itk::MetaDataObject< std::string > MetaDataStringType;

    // The Meta data dictionary is organized as a container with its corresponding
    // iterators. We can therefore visit all its entries by first getting access to
    // its \code{Begin()} and \code{End()} methods.
    DictionaryType::ConstIterator itr = dictionary.Begin();
    DictionaryType::ConstIterator end = dictionary.End();

    //  It is also possible to query for specific entries instead of reading all of
    //  them as we did above. In this case, the user must provide the tag
    //  identifier using the standard DICOM encoding. The identifier is stored in a
    //  string and used as key on the dictionary.

    DictionaryType::ConstIterator tagItr = dictionary.Find( entryId );

    if( tagItr == end )
    {
        OSLM_WARN( "Tag " << entryId << " not found in the DICOM header" );
        return "";
    }

    // Since the entry may or may not be of string type we must again use a
    // \code{dynamic\_cast} in order to attempt to convert it to a string dictionary
    // entry. If the conversion is successful, then we can print out its content.

    MetaDataStringType::ConstPointer entryvalue = dynamic_cast<const MetaDataStringType *>( tagItr->second.GetPointer() );

    if( entryvalue )
    {
        value = entryvalue->GetMetaDataObjectValue();
    }
    else
    {
        SLM_WARN( "Entry was not of string type" );
        return "";
    }

    return value;
}

//------------------------------------------------------------------------------

void DicomPatientDBReader::addPatients
( ::fwData::PatientDB::sptr patientDB, const ::boost::filesystem::path dirPath )
{
    SLM_TRACE_FUNC();

    // Type def
    typedef signed short    PixelType;
    const unsigned int      Dimension = 3;
    typedef itk::OrientedImage< PixelType, Dimension >         ImageType;

    // We use the image type for instantiating the type of the series reader and
    // for constructing one object of its type.
    typedef itk::ImageSeriesReader< ImageType >        ReaderType;
    ReaderType::Pointer reader = ReaderType::New();

    // A GDCMImageIO object is created and connected to the reader. This object is
    // the one that is aware of the internal intricacies of the DICOM format.
    typedef itk::GDCMImageIO       ImageIOType;
    ImageIOType::Pointer dicomIO = ImageIOType::New();
    reader->SetImageIO( dicomIO );

    // Now we face one of the main challenges of the process of reading a DICOM
    // series. That is, to identify from a given directory the set of filenames
    // that belong together to the same volumetric image. Fortunately for us, GDCM
    // offers functionalities for solving this problem and we just need to invoke
    // those functionalities through an ITK class that encapsulates a communication
    // with GDCM classes. This ITK object is the GDCMSeriesFileNames. Conveniently
    // for us, we only need to pass to this class the name of the directory where
    // the DICOM slices are stored. This is done with the \code{SetDirectory()}
    // method. The GDCMSeriesFileNames object will explore the directory and will
    // generate a sequence of filenames for DICOM files for one study/series.
    // In this example, we also call the \code{SetUseSeriesDetails(true)} function
    // that tells the GDCMSereiesFileNames object to use additional DICOM
    // information to distinguish unique volumes within the directory.  This is
    // useful, for example, if a DICOM device assigns the same SeriesID to
    // a scout scan and its 3D volume; by using additional DICOM information
    // the scout scan will not be included as part of the 3D volume.  Note that
    // \code{SetUseSeriesDetails(true)} must be called prior to calling
    // \code{SetDirectory()}. By default \code{SetUseSeriesDetails(true)} will use
    // the following DICOM tags to sub-refine a set of files into multiple series:
    // * 0020 0011 Series Number
    // * 0018 0024 Sequence Name
    // * 0018 0050 Slice Thickness
    // * 0028 0010 Rows
    // * 0028 0011 Columns
    // If this is not enough for your specific case you can always add some more
    // restrictions using the \code{AddSeriesRestriction()} method. In this example we will use
    // the DICOM Tag: 0008 0021 DA 1 Series Date, to sub-refine each series. The format
    // for passing the argument is a string containing first the group then the element
    // of the DICOM tag, separed by a pipe (|) sign.

    typedef itk::GDCMSeriesFileNames NamesGeneratorType;
    NamesGeneratorType::Pointer nameGenerator = NamesGeneratorType::New();

    nameGenerator->SetUseSeriesDetails( true );
    nameGenerator->AddSeriesRestriction("0008|0021");

    nameGenerator->SetDirectory( dirPath.string().c_str() );

    try
    {
        OSLM_DEBUG( "The directory: " << dirPath );

        // The GDCMSeriesFileNames object first identifies the list of DICOM series
        // that are present in the given directory. We receive that list in a reference
        // to a container of strings and then we can do things like printing out all
        // the series identifiers that the generator had found. Since the process of
        // finding the series identifiers can potentially throw exceptions, it is
        // wise to put this code inside a try/catch block.


        typedef std::vector< std::string >    SeriesIdContainer;

        const SeriesIdContainer & seriesUID = nameGenerator->GetSeriesUIDs();

        if ( seriesUID.size() == 0 )
        {
            SLM_DEBUG("Directory does not contain DICOM Series.");
        }
        else
        {
            SLM_DEBUG("Contains the following DICOM Series : ");
            SeriesIdContainer::const_iterator seriesItr = seriesUID.begin();
            SeriesIdContainer::const_iterator seriesEnd = seriesUID.end();
            while( seriesItr != seriesEnd )
            {
                OSLM_DEBUG(seriesItr->c_str());
                seriesItr++;
            }

            // Given that it is common to find multiple DICOM series in the same directory,
            // we must tell the GDCM classes what specific series do we want to read. In
            // this example we do this by checking first if the user has provided a series
            // identifier in the command line arguments. If no series identifier has been
            // passed, then we simply use the first series found during the exploration of
            // the directory.

            seriesItr = seriesUID.begin();
            while( seriesItr != seriesEnd )
            {
                std::string seriesIdentifier;
                seriesIdentifier = seriesItr->c_str();
                OSLM_DEBUG("Now reading series: " << seriesIdentifier);

                // We pass the series identifier to the name generator and ask for all the
                // filenames associated to that series. This list is returned in a container of
                // strings by the \code{GetFileNames()} method.
                typedef std::vector< std::string >   FileNamesContainer;
                FileNamesContainer fileNames;
                fileNames = nameGenerator->GetFileNames( seriesIdentifier );

                // The list of filenames can now be passed to the \doxygen{ImageSeriesReader}
                // using the \code{SetFileNames()} method.
                //std::reverse(fileNames.begin(), fileNames.end());
                reader->SetFileNames( fileNames );

                std::stringstream msg;
                msg << "Path : " << dirPath.leaf() << std::endl;
                msg << "Series " << seriesItr - seriesUID.begin() + 1 << "/" << seriesEnd - seriesUID.begin() << " : " << seriesIdentifier;

                // now progress event from itkReader are forwared to fwReader
                itk::LightProcessObject::Pointer castHelper= (itk::LightProcessObject *)(reader.GetPointer());
                Progressor progress(castHelper, this->getSptr(), msg.str());

                // Finally we can trigger the reading process by invoking the \code{Update()}
                // method in the reader. This call as usual is placed inside a \code{try/catch}
                // block.
                try
                {
                    // Read image dicom
                    reader->Update();
                    ImageType::Pointer itkimage = reader->GetOutput();

                    // Convert image dicom in data image
                    ::fwData::Image::sptr img = ::itkIO::dataImageFactory< ImageType >( itkimage );

                    // Add a new patient
                    ::fwData::Acquisition::NewSptr acq;
                    ::fwData::Study::NewSptr       study;
                    ::fwData::Patient::NewSptr     patient;

                    acq->setImage(img);
                    study->addAcquisition(acq);
                    patient->addStudy(study);
                    patientDB->addPatient( patient );

                    // Get string value
                    std::string nameStr     = getDicomValue(dicomIO , "0010|0010");
                    std::string sexStr      = getDicomValue(dicomIO , "0010|0040");
                    std::string modality    = getDicomValue(dicomIO , "0008|0060");
                    std::string zone        = getDicomValue(dicomIO , "0008|1030");
                    std::string id          = getDicomValue(dicomIO , "0010|0020");
                    std::string birthdateStr= getDicomValue(dicomIO , "0010|0030");
                    std::string hospital    = getDicomValue(dicomIO , "0008|0080");
                    std::string acqDateStr  = getDicomValue(dicomIO , "0008|0022");
                    std::string acqTimeStr  = getDicomValue(dicomIO , "0008|0032");
                    std::string spacing     = getDicomValue(dicomIO , "0028|0030");
                    std::string thicknessStr= getDicomValue(dicomIO , "0018|0050");
                    std::string interSliceStr= getDicomValue(dicomIO , "0018|0088");
                    std::string centerStr   = getDicomValue(dicomIO , "0028|1050");
                    std::string widthStr    = getDicomValue(dicomIO , "0028|1051");
                    std::string rescaleStr  = getDicomValue(dicomIO , "0028|1052");
                    std::string imageTypeStr    = getDicomValue(dicomIO , "0008|0008");

                    // Spacing with itk
                    std::vector< double > m_vPixelSpacing ( 3, 10000.0 );
                    m_vPixelSpacing[0] = itkimage->GetSpacing()[0];
                    m_vPixelSpacing[1] = itkimage->GetSpacing()[1];
                    m_vPixelSpacing[2] = itkimage->GetSpacing()[2];

                    // Name & firstname
                    std::string name = "";
                    std::string firstname = "";

                    std::string::size_type sizeBase = std::string::npos;
                    std::string::size_type sizeIndex = nameStr.find('^');
                    if (sizeIndex == sizeBase)
                    {
                        name = nameStr;
                        firstname = "";
                        assert( name.length() == nameStr.length() );
                    }
                    else
                    {
                        name = nameStr.substr(0, sizeIndex);
                        firstname = nameStr.substr(sizeIndex+1, nameStr.size());
                        assert( name.length() + firstname.length() + 1 == nameStr.length() );
                    }

                    bool sex = true;
                    if (sexStr[0] == 'F')
                        sex = false;

                    // Center
                    char* endptr;
                    double center = strtod( centerStr.c_str(), &endptr );
                    if ( endptr == centerStr.c_str() ) center = 0.0;

                    // width
                    double width = strtod( widthStr.c_str(), &endptr );
                    if ( endptr == widthStr.c_str() ) width = 0.0;

                    // rescale
                    double rescale = strtod( rescaleStr.c_str(), &endptr );
                    if ( endptr == rescaleStr.c_str() ) rescale = 0.0;

                    // Birthdate
                    ::boost::posix_time::ptime birthdate = ::fwTools::strToBoostDateAndTime(birthdateStr);

                    // Acq date & acq time
                    ::boost::posix_time::ptime acqDate = ::fwTools::strToBoostDateAndTime(acqDateStr,acqTimeStr);


                    OSLM_DEBUG( "Name       = " << name );
                    OSLM_DEBUG( "Firstname  = " << firstname );
                    OSLM_DEBUG( "IsMale     = " << sex );
                    OSLM_DEBUG( "Modality   = " << modality );
                    OSLM_DEBUG( "Zone       = " << zone );
                    OSLM_DEBUG( "ID         = " <<  id );
                    OSLM_DEBUG( "Birthdate  = " <<  birthdate);
                    OSLM_DEBUG( "Hospital   = " <<  hospital);
                    OSLM_DEBUG( "Acq. date  = " <<  acqDate);
                    OSLM_DEBUG( "Spacing    = " <<  m_vPixelSpacing[0] << "x" << m_vPixelSpacing[1]);
                    OSLM_DEBUG( "Slice thickness = " <<  m_vPixelSpacing[2]);
                    OSLM_DEBUG( "Window center = " << center);
                    OSLM_DEBUG( "Window width  = " << width);
                    OSLM_DEBUG( "Rescale intercept = " << rescale);
                    OSLM_DEBUG( "Image Type = " << imageTypeStr);

                    // Set field
                    img->setCRefSpacing(m_vPixelSpacing);
                    img->setWindowCenter(center);
                    img->setWindowWidth(width);
                    img->setRescaleIntercept(rescale);

                    acq->setCRefCreationDate(acqDate);
                    acq->setCRefImageType(imageTypeStr);

                    study->setCRefModality(modality);
                    study->setCRefHospital(hospital);
                    study->setCRefAcquisitionZone(zone);

                    patient->setCRefFirstname(firstname);
                    patient->setCRefName(name);
                    patient->setCRefIDDicom(id);
                    patient->setCRefBirthdate(birthdate);
                    patient->setCRefIsMale(sex);

                }
                catch (itk::ExceptionObject & ex)
                {
                    OSLM_WARN(ex);
                }

                // Change series
                seriesItr++;

            } // while ( seriesItr != seriesEnd )

        } // else ( seriesUID.size() != 0 )
    }
    catch (itk::ExceptionObject & ex)
    {
        OSLM_WARN(ex);
    }
}

//------------------------------------------------------------------------------

void DicomPatientDBReader::read()
{
    SLM_TRACE_FUNC();

    assert( ::boost::filesystem::exists( this->getFolder() ) );

#if SPYLOG_LEVEL < 5
    const bool bWarn = itk::Object::GetGlobalWarningDisplay();
    itk::Object::SetGlobalWarningDisplay(false);
#endif

    ::std::vector< ::boost::filesystem::path > dirs;
    searchRecursivelyDirectories( this->getFolder() , dirs );

    BOOST_FOREACH(::boost::filesystem::path dir, dirs)
    {
        addPatients( this->getConcreteObject() , dir );
    }

#if SPYLOG_LEVEL < 5
    itk::Object::SetGlobalWarningDisplay(bWarn);
#endif

}


//------------------------------------------------------------------------------

} // namespace itkIO

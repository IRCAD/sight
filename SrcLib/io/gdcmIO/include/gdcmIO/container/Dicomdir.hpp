/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __GDCMIO_CONTAINER_DICOMDIR_HPP__
#define __GDCMIO_CONTAINER_DICOMDIR_HPP__

#include <string>
#include <vector>

#include <gdcmSequenceOfItems.h>
#include <gdcmTag.h>

#include <fwData/Image.hpp>

#include "gdcmIO/config.hpp"

namespace gdcmIO
{
namespace container
{

/**
 * @brief   This class define a container of landmarks
 * @class   Dicomdir
 */
class GDCMIO_CLASS_API Dicomdir
{

public:

    typedef struct {
        std::string fileID;
        std::string instanceUID;
    } Image;

    typedef struct  {
        std::string instanceUID;
        std::string modality;
        std::string description;
        std::list <Image> images;
    } Serie;

    typedef struct  {
        std::string instanceUID;
        std::string studyDate;
        std::string description;
        std::list <Serie> seriesList;
    } Study;

    typedef struct  {
       std::string name;
       std::string id;
    } Patient;

    typedef std::list< Patient > PatientListType;
    typedef std::list< Study > StudyListType;
    typedef std::list< Serie > SerieListType;
    typedef std::list< Image > ImageListType;

private:

    typedef std::map< std::string, std::list <Study> > PatientMapType;
    typedef std::map< std::string, std::list <Serie> > StudyMapType;
    typedef std::map< std::string, std::list <Image> > SeriesMapType;

    ::boost::filesystem::path rootPath;
    PatientListType patientList;
    PatientMapType patients;
    StudyMapType studies;
    SeriesMapType series;

public :

    GDCMIO_API Dicomdir();
    GDCMIO_API Dicomdir(const ::boost::filesystem::path dicomDir);
    const PatientListType&  getPatients() {return patientList;}
    const StudyListType&  getStudies(std::string patientID) {return patients[patientID];}
    const SerieListType&  getSeries(std::string studyInstanceUID) {return studies[studyInstanceUID];}
    const ImageListType&  getImages(std::string serieInstanceUID) {return series[serieInstanceUID];}


    GDCMIO_API ~Dicomdir();
    /**
     * \brief    Read A DICOMDIR file.
     *
     * \param  dicomDir a path to the DICOMDIR file.
     *
     * @return    the state of the read (OK, NOK).
     */
    GDCMIO_API bool read(const ::boost::filesystem::path dicomDir);



private:
    std::string getValueOfTag(const gdcm::Item & item, const gdcm::Tag& tag);
    Patient getPatientInformation(const gdcm::Item & item);
    Study getStudyInformation(const gdcm::Item & item);
    Serie getSerieInformation(const gdcm::Item& item);
    Image getImageInformation(const gdcm::Item & item);

};
} //namespace container
} //namespace gdcmIO

#endif /* __GDCMIO_CONTAINER_DICOMDIR_HPP__ */

/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IOVTK_SSERIESDBREADER_HPP__
#define __IOVTK_SSERIESDBREADER_HPP__

#include "ioVTK/config.hpp"

#include <fwData/location/ILocation.hpp>
#include <fwData/Mesh.hpp>

#include <io/IReader.hpp>

#include <boost/filesystem/path.hpp>

#include <string>

namespace fwMedData
{
class SeriesDB;
}

namespace fwJobs
{
class IJob;
}

namespace ioVTK
{

/**
 * @brief   VTK File Reader.
 *
 * Service reading a VTK file (mesh or image) using the fwVtkIO lib.
 */
class IOVTK_CLASS_API SSeriesDBReader : public ::io::IReader
{

public:
    typedef ::fwCom::Signal< void ( SPTR(::fwJobs::IJob) ) > JobCreatedSignalType;

    /**
     * @brief Constructor. Do nothing.
     */
    IOVTK_API SSeriesDBReader() throw();

    ~SSeriesDBReader() throw()
    {
    }

    fwCoreServiceClassDefinitionsMacro( (SSeriesDBReader)( ::io::IReader) );

    /**
     * @brief Configure the vtk file path.
     *
     * This method is used to find
     * the file path  using a file selector.
     */
    IOVTK_API virtual void configureWithIHM();

protected:

    IOVTK_API virtual ::io::IOPathType getIOPathType() const;

    /**
     * @brief Starting method.
     *
     * This method is used to initialize the service.
     */
    IOVTK_API virtual void starting() throw(::fwTools::Failed);

    /**
     * @brief Stopping method.
     *
     * The stopping method is empty for this service.
     */
    IOVTK_API virtual void stopping() throw(::fwTools::Failed);

    /**
     * @brief Configuring method.
     *
     * The configuring method only calls the configuring method from the base class
     */
    IOVTK_API virtual void configuring() throw(::fwTools::Failed);

    /**
     * @brief Updating method.
     *
     * This method is used to update the service.
     * The image is read.
     */
    IOVTK_API void updating() throw(::fwTools::Failed);

    /**
     * @brief Info method.
     *
     * This method is used to give
     * informations about the service.
     */
    IOVTK_API void info(std::ostream& _sstream );

private:

    /**
     * @brief Load VTK mesh or image and push it into SeriesDB.
     * @param[in] vtkFiles vector of boost::filesystem::path.
     * @param[out] seriesDB std::shared_ptr< ::fwMedData::SeriesDB >.
     * @return bool.
     *
     * This method is used to load a mesh using the file path.
     */
    void loadSeriesDB( const ::fwData::location::ILocation::VectPathType& vtkFiles,
                       const SPTR(::fwMedData::SeriesDB)& seriesDB );

    SPTR(JobCreatedSignalType) m_sigJobCreated;

};

} // namespace ioVTK

#endif //__IOVTK_SSERIESDBREADER_HPP__

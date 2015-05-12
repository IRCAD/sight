/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IOVTK_SMODELSERIESREADER_HPP__
#define __IOVTK_SMODELSERIESREADER_HPP__

#include <string>
#include <boost/filesystem/path.hpp>

#include <fwServices/ObjectMsg.hpp>

#include <io/IReader.hpp>

#include "ioVTK/config.hpp"

namespace ioVTK
{

/**
 * @brief   Model series reader service.
 * @class   SModelSeriesReader
 *
 * Service reading a model series as .vtk files using the fwVtkIO lib.
 */
class IOVTK_CLASS_API SModelSeriesReader : public ::io::IReader
{

public:
    ~SModelSeriesReader() throw()
    {
    }

    fwCoreServiceClassDefinitionsMacro ( (SModelSeriesReader)( ::io::IReader) );

    /**
     * @brief Configure the mesh path.
     *
     * This method is used to find the file path  using a file selector.
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
     * @brief Updating method.
     *
     * This method is used to update the service.
     * The mesh is read.
     */
    IOVTK_API void updating() throw(::fwTools::Failed);

    /**
     * @brief Info method.
     *
     * This method is used to give
     * informations about the service.
     */
    IOVTK_API void info(std::ostream &_sstream );

private:

    IOVTK_API void loadMesh( const ::boost::filesystem::path file, ::fwData::Mesh::sptr mesh );

};

} // namespace ioVTK

#endif //__IOVTK_SMODELSERIESREADER_HPP__


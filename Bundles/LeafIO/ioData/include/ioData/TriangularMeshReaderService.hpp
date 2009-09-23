/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#ifndef _IODATA_TRIANGULARMESHREADER_HPP_
#define _IODATA_TRIANGULARMESHREADER_HPP_

#include <io/IReader.hpp>
#include <boost/filesystem/path.hpp>

#include "ioData/export.hpp"

namespace ioData
{
/**
 * @brief	Triangular mesh reader service.
 * @class	TriangularMeshReaderService.
 * @author	IRCAD (Research and Development Team).
 * @date	2009.
 *
 * Service reading a mesh object.
 * @li This service has no specified start and stop method.
 * @li Use setConfiguration(cfg) and configure() methods to configure the mesh filename.
 * @li Use update() to read the mesh and notify observers.
 *
 * Service registered details : \n
 * REGISTER_SERVICE( ::io::IReader , ::ioData::TriangularMeshReaderService , ::fwData::TriangularMesh )
 */
class IODATA_CLASS_API TriangularMeshReaderService : public ::io::IReader
{

public:

	 /// Super class of reader services
	typedef ::io::IReader	SuperClass;

	/**
	 * @brief	Constructor : does nothing
	 */
	IODATA_API TriangularMeshReaderService() ;

	/**
	 * @brief	Destructor
	 */
	IODATA_API ~TriangularMeshReaderService() throw() ;

	/** @name Specified reader service methods ( override from ::io::IReader )
	 * @{
	 */

	/**
	 * @brief	returns  (filename) extension
	 */
	IODATA_API virtual std::vector< std::string > getSupportedExtensions() ;

	/**
	 * @brief Configure the image path.
	 *
	 * This method is used to find
	 * the file path  using a file selector.
	 */
	IODATA_API void configureWithIHM();
	/// @}

protected:

	/** @name Service methods ( override from ::fwServices::IService )
	 * @{
	 */

	/**
	 * @brief Starting method : default does nothing.
	 *
	 * This method is used to initialize the service.
	 */
	IODATA_API virtual void starting() throw(::fwTools::Failed){};

	/**
	 * @brief Stopping method : default does nothing.
	 *
	 * The stopping method is empty for this service.
	 */
	IODATA_API virtual void stopping() throw(::fwTools::Failed){};


	/**
	 * @brief Configure service. This method is called by configure() from base service ( ::fwServices::IService )
	 *
	 * XML configuration sample:
	 * @verbatim
	<service type="::ioData::TriangularMeshReaderService">
		<filename>../matrix.trf</filename>
	</service>
	 @endverbatim
	 *
	 * Configure mesh filename.
	 */
	IODATA_API virtual void configuring( ) throw(::fwTools::Failed);

	/**
	 * @brief Updating method. This method is called by update() from base service ( ::fwServices::IService )
	 *
	 * This method is used to update the service.
	 * The mesh is read with the reader ::fwDataIO::reader::TriangularMeshReader.
	 * Notify reading.
	 */
	IODATA_API void updating() throw(::fwTools::Failed);

	/**
	 * @brief React on modifications : default does nothing.
	 *
	 * @note This method is automaticaly called by update( msg ) method from base service ( ::fwServices::IService ).
	 *
	 * @param[in] _msg information message for modification
	 */
	IODATA_API void updating( ::boost::shared_ptr< const ::fwServices::ObjectMsg > _msg ) throw(::fwTools::Failed){};


	/**
	 * @brief Info method.
	 *
	 * This method is used to give
	 * informations about the service.
	 *
	 * @param[out] _sstream output stream
	 */
	IODATA_API virtual void info(std::ostream &_sstream ) ;
	/// @}

private:

	::boost::filesystem::path m_fsMeshPath;

};

}

#endif /*_IODATA_TRIANGULARMESHREADER_HPP_*/

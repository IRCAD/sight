/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#ifndef _IODATA_TRANSFORMATIODATANMATRIX3DWRITER_HPP_
#define _IODATA_TRANSFORMATIODATANMATRIX3DWRITER_HPP_

#include <boost/filesystem/path.hpp>

#include <io/IWriter.hpp>

#include "ioData/export.hpp"

namespace ioData
{
/**
 * @brief	Transformation matrix 3D writer service.
 * @class	TransformationMatrix3DWriterService.
 * @author	IRCAD (Research and Development Team).
 * @date	2009.
 *
 * Service writing a TransformationMatrix3D object
 * @li Use setConfiguration(cfg) and configure() methods to configure the matrix filename.
 * @li Use start() to initialize the service.
 * @li Use update() to write the transformation matrix.
 * @li Use stop() to stop service before to destroy it.
 *
 * Service registered details : \n
 * REGISTER_SERVICE( ::io::IWriter , ::ioData::TransformationMatrix3DWriterService , ::fwData::TransformationMatrix3D )
 */
class IODATA_CLASS_API TransformationMatrix3DWriterService : public ::io::IWriter
{

public:

    fwCoreServiceClassDefinitionsMacro ( (TransformationMatrix3DWriterService)(::io::IWriter::Baseclass) ) ;

    /// Super class of writer services
	typedef ::io::IWriter	SuperClass;

	/**
	 * @brief	Constructor : does nothing
	 */
	IODATA_API TransformationMatrix3DWriterService() ;

	/**
	 * @brief	Destructor
	 */
	IODATA_API ~TransformationMatrix3DWriterService() throw() ;

	/** @name Specified writer service methods ( override from ::io::IWriter )
	 * @{
	 */

	/**
	 * @brief Configure the matrix path.
	 *
	 * This method is used to find
	 * the file path  using a file selector.
	 */
	IODATA_API virtual void configureWithIHM();

	/**
	 * @brief	returns  (filename) extension
	 */
	IODATA_API virtual std::vector< std::string > getSupportedExtensions() ;
	///@}


protected:

	/** @name Service methods ( override from ::fwServices::IService )
	 * @{
	 */

	/**
	 * @brief Configure service. This method is called by configure() from base service ( ::fwServices::IService )
	 *
	 * XML configuration sample:
	 * @verbatim
	<service type="::ioData::TransformationMatrix3DWriterService">
		<filename>../matrix.trf</filename>
	</service>
	 @endverbatim
	 *
	 * Configure transformation matrix filename.
	 */
	IODATA_API virtual void configuring( ) throw(::fwTools::Failed) ;

	/**
	 * @brief Starting method. This method is called by start() from base service ( ::fwServices::IService )
	 *
	 * This method is used to initialize the service. The starting method is empty for this service.
	 */
	IODATA_API virtual void starting( ) throw(::fwTools::Failed);

	/**
	 * @brief Stopping method. This method is called by stop() from base service ( ::fwServices::IService )
	 *
	 * The stopping method is empty for this service.
	 */
	IODATA_API virtual void stopping( ) throw(::fwTools::Failed);

	/**
	 * @brief Updating method. This method is called by update() from base service ( ::fwServices::IService )
	 *
	 * This method is used to update the service.
	 * The transformation matrix is written with the writer ::fwDataIO::writer::TransformationMatrix3DWriter.
	 * Notify writing.
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

	::boost::filesystem::path m_filename;
};

} // namespace ioData

#endif // _IODATA_TRANSFORMATIODATANMATRIX3DWRITER_HPP_

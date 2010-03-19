/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _IODATA_SPLINEREADER_HPP_
#define _IODATA_SPLINEREADER_HPP_

#include <io/IReader.hpp>
#include <boost/filesystem/path.hpp>
#include <fwData/TransformationMatrix3D.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/shared_ptr.hpp>

#include "ioData/export.hpp"

namespace ioData
{
/**
 * @brief   Spline reader service.
 * @class   SplineReaderService.
 * @author  IRCAD (Research and Development Team).
 * @date    2009.
 *
 * Service reading a spline object.
 * @li This service has no specified start and stop method.
 * @li Use setConfiguration(cfg) and configure() methods to configure the spline.
 * @li Use update() to read the spline and notify observers.
 *
 * Service registered details : \n
 * REGISTER_SERVICE( ::io::IReader , ::ioData::SplineReaderService , ::fwData::Spline )
 */
class IODATA_CLASS_API SplineReaderService : public ::io::IReader
{

public:
    fwCoreServiceClassDefinitionsMacro ( (SplineReaderService)( ::io::IReader::Baseclass) ) ;
    /// Super class of reader services
    typedef ::io::IReader   SuperClass;
    /**
     * @brief   Constructor
     */
    IODATA_API SplineReaderService() ;

    /**
     * @brief   Destructor
     */
    IODATA_API ~SplineReaderService() throw() ;

    /** @name Specified reader service methods ( override from ::io::IReader )
     * @{
     */
    /**
     * @brief Configure the path : default does nothing.
     *
     * This method is used to find
     * the file path  using a file selector.
     */
    IODATA_API virtual void configureWithIHM(){};

    /**
     * @brief   returns  (filename) extension
     */
    IODATA_API virtual std::vector< std::string > getSupportedExtensions() ;

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
     <service uid="tubeReader" type="::io::IReader" implementation="::ioData::SplineReaderService">
       <spline id="0" matrix="../matrix.trf" nbSides="10" radius="10">
        <point id="0"/>
        <point id="4"/>
        <point id="1"/>
        <point id="5"/>
      </spline>
    </service>
      @endverbatim
     */
    IODATA_API virtual void configuring( ) throw(::fwTools::Failed) ;

    /**
     * @brief Updating method. This method is called by update() from base service ( ::fwServices::IService )
     *
     * This method is used to update the service.
     * Read the spline and notify.
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
    /*!
     * Charge la matrice de transformation 4X4 permettant de positionner l'objet
     *
     * @param m_file fichier contenat la matrice de transformation.
     * @return TransformationMatrix3D
     */
     ::boost::shared_ptr< ::fwData::TransformationMatrix3D > loadObjectTransformationMatrix3D(std::string m_file);

    /**
     * @brief   matrice 4X4 permettant de positionner l'objet
     */
     ::boost::shared_ptr< ::fwData::TransformationMatrix3D> objectMatrix;
    /**
     * @brief   true s'il existe matrice 4X4 permettant de positionner l'objet
     */
    bool isTransfo;
    typedef std::vector< ::fwData::Spline::point > Points ;
    Points      m_points;
    double      m_radius;
    int         m_nbSides;
    int         m_idSpline;
};

}

#endif /*_IODATA_SPLINEREADER_HPP_*/

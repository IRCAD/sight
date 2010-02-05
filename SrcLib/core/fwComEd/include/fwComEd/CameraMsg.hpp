/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWCOMED_CAMERAMSG_HPP_
#define _FWCOMED_CAMERAMSG_HPP_

#include <fwServices/ObjectMsg.hpp>

#include "fwComEd/export.hpp"

namespace fwComEd
{

/**
 * @brief   Object message specialized for Camera : store modification information
 * @class   CameraMsg
 * @author  IRCAD (Research and Development Team).
 * @date    2009
 * @see     ::fwServices::ObjectMsg
 */
class FWCOMED_CLASS_API CameraMsg : public ::fwServices::ObjectMsg
{
public:

    fwCoreClassDefinitionsWithFactoryMacro((CameraMsg)(::fwServices::ObjectMsg::Baseclass), (( )) , new CameraMsg );

    /// Event identifier used to inform that a new camera is created
    FWCOMED_API static std::string NEW_CAMERA;

    /// Event identifier used to inform that a the camera is moving
    FWCOMED_API static std::string CAMERA_MOVING;

    /**
    * @brief    Constructor.
    */
    FWCOMED_API CameraMsg() throw();

    /**
    * @brief    Destuctor : does nothing.
    */
    FWCOMED_API virtual ~CameraMsg() throw();


    /**
     * @name    Camera position
     */
    // @{

    /**
     * @brief       Set the new camera position.
     * @param[in]   _positionValue array containing the x,y,z coordinates of the camera
     */
    FWCOMED_API void setPositionCamera( const double _positionValue[3]);

    /**
     * @brief   Get the new camera position.
     * @return  An array containing the x,y,z coordinates of the camera
     */
    FWCOMED_API const double* getPositionCamera( void )const;
    //@}


    /**
     * @name    Camera focal
     */
    // @{

    /**
     * @brief       Set the new camera focal.
     * @param[in]   _focalValue array containing the focal coordinates.
     */
    FWCOMED_API void setFocalCamera( const double _focalValue[4] );

    /**
     * @brief   Get the new camera focal.
     * @return  An array containing the focal coordinates.
     */
    FWCOMED_API const double* getFocalCamera( void )const;
    // @}


    /**
     * @name    Camera view up
     */
    // @{

    /**
     * @brief       Set the new camera view up.
     * @param[in]   _viewUpValue array containing the view up coordinates.
     */
    FWCOMED_API void setViewUpCamera( const double _viewUpValue[3] );

    /**
     * @brief   Get the new camera view up.
     * @return  An array containing the view up coordinates.
     */
    FWCOMED_API const double* getViewUpCamera( void )const;
    //@}


protected:

    /**
      * @brief used by operator<<(std::ostream & _sstream, IService& _service)
      */
    FWCOMED_API virtual void info(std::ostream &_sstream ) const;

private:

    double m_positionCamera[3]; ///< Camera position
    double m_focalCamera[4];    ///< Camera focal
    double m_viewUpCamera[3];   ///< Camera view up

};

} // namespace fwComEd

#endif //_FWCOMED_CAMERAMSG_HPP_


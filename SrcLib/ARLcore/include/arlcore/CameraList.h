/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _ARLCORE_CAMERALIST_H
#define _ARLCORE_CAMERALIST_H
#include <arlcore/Common.h>

#include <vector>

#include <arlcore/Camera.h>
#include <arlcore/Particle.h>

namespace arlCore
{
    class PlaneSystem;
    /**
     * @class   CameraList
     * @author  IRCAD (Research and Development Team)
     * @date    06/2009
     * @brief   List of physical cameras
     */
    class CameraList : public arlCore::Particle
    {
    public:
        //! @brief Constructor
        ARLCORE_API CameraList( PlaneSystem &universe );

        //! @brief Destructor
        ARLCORE_API ~CameraList( void );

        //! @brief Add a new camera with its unique ID and its resolution
        ARLCORE_API void addCamera( const uint32HL &GUID, unsigned int width, unsigned int heigth );
        ARLCORE_API void push_back( const uint32HL &GUID, unsigned int width, unsigned int heigth );

        //! @return Number of cameras
        ARLCORE_API unsigned int size( void ) const;

        //! @return Reference on the ith camera
        ARLCORE_API const arlCore::Camera& operator[]( unsigned int i ) const;
        ARLCORE_API arlCore::Camera& operator[]( unsigned int i );

        //! @return Reference on the last camera
        ARLCORE_API const arlCore::Camera& back( void ) const;
        ARLCORE_API arlCore::Camera& back( void );

        //! @return Reference on the list of cameras
        ARLCORE_API const std::vector<Camera>& getList( void ) const;

        /**
        * @brief Calibrate cameras at the date
        * @param[in] caliFile File with references on camera's parameters
        * @param[in] date Current date, time
        * @param[in] commonPlane for stereoscopic calibration
        * @return Number of calibrated cameras
        */
        ARLCORE_API unsigned int calibrate( const std::string &calibFile, long int date );

    private:
        std::vector<Camera> m_cameras;
        std::vector<unsigned int> m_widths;
        std::vector<unsigned int> m_heigths;
    };

} // namespace arlCore
#endif // _ARLCORE_CAMERALIST_H

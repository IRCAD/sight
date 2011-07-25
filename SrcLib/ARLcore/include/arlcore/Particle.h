/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _ARLCORE_PARTICLE_H
#define _ARLCORE_PARTICLE_H
#include <arlcore/Common.h>

#include <string>
#include <arlcore/PlaneSystem.h>

namespace arlCore
{
    class ARLCORE_CLASS_API Particle
    {
    /**
     * @class   Particle
     * @author  IRCAD (Research and Development Team)
     * @date    2007
     * @brief   Basic class that represents an object in space
     */
    public:
        /**
        * @brief Créeation of a particle that have its own frame in the universe
        * The identity ID of the frame can be got using getPlane()
        * The frame name is initialized with planeName
        */
        ARLCORE_API Particle( PlaneSystem &universe, const std::string &planeName="" );

        //! @brief Constructor by copy
        ARLCORE_API Particle( const Particle& );

        //! @brief Affectation
        ARLCORE_API Particle& operator=( const Particle& );

        //! @brief Destruction de la particule avec libération du repère dans l'univers
        ARLCORE_API virtual ~Particle( void );

        //! @return Name of the particle
        ARLCORE_API std::string getName( void ) const;

        //! @return Description textuelle de la particule
        ARLCORE_API virtual std::string getString( void ) const;

        //! @return Référence sur l'univers qui contient la particule
        ARLCORE_API PlaneSystem& getPlaneSystem( void ) const;

        //! @return ID du repère de la particule dans l'univers
        ARLCORE_API unsigned int getPlane( void ) const;

    private:
        // Univers dans lequel la particule est plongée
        PlaneSystem &m_universe;

        // ID du repère de la particule dans l'univers
        unsigned int m_plane;
    };
} // namespace arlCore
#endif // _ARLCORE_PARTICLE_H

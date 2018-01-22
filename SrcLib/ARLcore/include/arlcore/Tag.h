/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#ifndef __ARLCORE_TAG_H__
#define __ARLCORE_TAG_H__

#include <fwCore/macros.hpp>

#include <arlcore/Common.h>
#include <arlcore/Object.h>
#include <arlcore/Parameters.h>
#include <arlcore/Particle.h>
#include <arlcore/PointsList.h>

namespace arlCore
{
enum ARLCORE_TAG_PARAMETERS { ARLCORE_TAGFLAG_REGISTRATION_ERROR_EVALUATION, ARLCORE_TAG_REGISTRATION_MAXERROR,
                              ARLCORE_TAG_NBPARAMETERS };
static std::string ARLCORE_TAGPARAMETERS_NAMES[ARLCORE_TAG_NBPARAMETERS] =
{ "Registration error evaluation", "Registration max error" };
static boost::any ARLCORE_TAGPARAMETERS_DEFAULTS[ARLCORE_TAG_NBPARAMETERS] = { true, 0.0 };

class PlaneSystem;

/**
 * @brief   Detection tag
 */
class ARLCORE_CLASS_API Tag :  public Particle,
                               public arlCore::Parameters
{
public:

    fwCoreClassDefinitionsWithNFactoriesMacro( (Tag)(::fwTools::Object),
                                               ((TagFactory, ((arlCore::PlaneSystem&))((const std::string&)) ))
                                                   ((TagFactory, (( arlCore::PlaneSystem&))((PointList::csptr)) ))
                                               );

    ARLCORE_API static Tag::sptr TagFactory( arlCore::PlaneSystem& universe, const std::string& name);
    ARLCORE_API static Tag::sptr TagFactory( arlCore::PlaneSystem& universe, PointList::csptr );

    /**
     * @brief Type of registration
     * ARL_TAG_REGISTRATION_3D3D : Registration with matching 3D point
     * ARL_TAG_REGISTRATION_3DWO : Registration without matching 3D point
     * ARL_TAG_REGISTRATION_ICP : ICP registration between 2 clouds of 3D points
     */
    enum ARLCORE_TAG_REGISTRATION_TYPE { ARLCORE_TAG_REGISTRATION_UNKNOWN, ARLCORE_TAG_REGISTRATION_NONE,
                                         ARLCORE_TAG_REGISTRATION_3D3D, ARLCORE_TAG_REGISTRATION_3DWO,
                                         ARLCORE_TAG_REGISTRATION_ICP, ARLCORE_TAG_REGISTRATION_NBTYPES };

    //! @brief Constructor with a filename
    ARLCORE_API Tag( arlCore::PlaneSystem& universe, const std::string& name );

    //! @brief Constructor with a pointlist for tag's geometry
    ARLCORE_API Tag( arlCore::PlaneSystem& universe, PointList::csptr );

    //! @brief Destructor
    ARLCORE_API ~Tag( void );

    ARLCORE_API bool init( void );
    ARLCORE_API std::string getString( void ) const override;

    //! @return Measure plane
    ARLCORE_API unsigned int getMeasuresPlane() const;

    //! @return Number of geometry points
    ARLCORE_API unsigned int getNbPoints() const;

    //! @return Reference on geometry's pointlist
    ARLCORE_API arlCore::PointList::csptr getGeometry() const;

    //! @return Reference on geometry's pointlist
    ARLCORE_API arlCore::PointList::sptr getGeometry();

    //! @return Reference on measures's pointlist
    ARLCORE_API arlCore::PointList::csptr getMeasures() const;

    //! @return Reference on measures's pointlist
    ARLCORE_API arlCore::PointList::sptr getMeasures();

    //! @return Registration type
    ARLCORE_API ARLCORE_TAG_REGISTRATION_TYPE getRegistrationType( void );

    //! @return Maximum error for registration
    ARLCORE_API double getRegistrationMaxError( void );

    //! @brief Set maximum error for registration
    ARLCORE_API void setRegistrationMaxError( double error );

    //! @brief Reset points visibilities, origin plane before new detection
    ARLCORE_API void reset( void );

    /**
     * @brief Set a transformation between Tag's plane -> plane (ie : Video's plane)
     * @param[in] plane Plane of the
     * @param[in] T Transformation
     * @param[in] reset If true, measures are all computed from T
     * @param[in] date & time of the measure
     * @return false if RMS>getRegistrationMaxError()
     */
    ARLCORE_API bool setRegistration( unsigned int plane, arlCore::vnl_rigid_matrix& T, long int date, long int time,
                                      bool reset );

    //------------------------------------------------------------------------------

    ARLCORE_API double getPersistence( void ) const
    {
        return m_persistence;
    }
    //------------------------------------------------------------------------------

    ARLCORE_API void setPersistence( double p )
    {
        m_persistence = p;
    }

protected:
    PointList::sptr m_geometry;     //3D
    PointList::sptr m_measures;     //3D
    unsigned int m_measuresPlane;
    ARLCORE_TAG_REGISTRATION_TYPE m_registrationType;
    double m_registrationMaxError;
    double m_persistence;

};
} // namespace arlCore
#endif // __ARLCORE_TAG_H__

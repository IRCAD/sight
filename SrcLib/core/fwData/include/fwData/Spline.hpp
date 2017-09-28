/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDATA_SPLINE_HPP__
#define __FWDATA_SPLINE_HPP__

#include "fwData/Color.hpp"
#include "fwData/config.hpp"
#include "fwData/factory/new.hpp"

#include <vector>

namespace fwData
{

/**
 * @brief This class defines a spline object.
 */
class FWDATA_CLASS_API Spline : public Object
{
public:
    fwCoreClassDefinitionsWithFactoryMacro( (Spline)(::fwData::Object), (()), ::fwData::factory::New< Spline >);

    /**
     * @struct point
     * @brief Defines a 3D %point for a spline.
     */
    class point
    {
    public:

        int id;                      /**< @brief Identifier */
        double p[3];                 /**< @brief Point coordinates */
        double normal[3];            /**< @brief Normal coordinates*/
        ::fwData::Color::sptr c;     /**< @brief Point color*/
        bool isVisible;              /**< @brief Flag if %point is visible */

        //------------------------------------------------------------------------------

        point& operator=( const point& _point )
        {
            this->id        = _point.id;
            this->p[0]      = _point.p[0];
            this->p[1]      = _point.p[1];
            this->p[2]      = _point.p[2];
            this->normal[0] = _point.normal[0];
            this->normal[1] = _point.normal[1];
            this->normal[2] = _point.normal[2];
            this->isVisible = _point.isVisible;
            c               = ::fwData::Object::copy( _point.c );
            return(*this);
        }
    };

    /// 3D %point container
    typedef std::vector< point > Points;

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    FWDATA_API Spline(::fwData::Object::Key key);

    /**
     * @brief destructor
     */
    FWDATA_API virtual ~Spline();

    /**
     * @brief returns editable point container
     */
    FWDATA_API Points& points();

    /**
     * @{
     * @brief Get/Set value of the radius.
     */
    const double  getRadius () const;
    double& getRefRadius ();
    const double& getCRefRadius () const;
    void setRadius(double _radius);
    /// @}

    /**
     * @{
     * @brief Get/Set value of the number of slices.
     */
    const int  getNbSides () const;
    int& getRefNbSides ();
    const int& getCRefNbSides () const;
    void setNbSides (const int _nbSides);
    void setCRefNbSides (const int& _nbSides);
    /// @}

    /**
     * @{
     * @brief Get/Set value of the spline identifier.
     */
    const int  getIdSpline () const;
    int& getRefIdSpline ();
    const int& getCRefIdSpline () const;
    void setIdSpline (const int _idSpline);
    void setCRefIdSpline (const int& _idSpline);
    /// @}

    /// Defines deep copy
    FWDATA_API void cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType& cache);

protected:

    /// Points container
    Points m_points;
    double m_radius;
    int m_nbSides;
    int m_idSpline;

};

//-----------------------------------------------------------------------------

inline const double Spline::getRadius () const
{
    return m_radius;
}

//-----------------------------------------------------------------------------

inline double& Spline::getRefRadius ()
{
    return m_radius;
}

//-----------------------------------------------------------------------------

inline const double& Spline::getCRefRadius () const
{
    return m_radius;
}

//-----------------------------------------------------------------------------

inline void Spline::setRadius(double _radius)
{
    m_radius = _radius;
}

//-----------------------------------------------------------------------------

inline const int Spline::getNbSides () const
{
    return m_nbSides;
}

//-----------------------------------------------------------------------------

inline int& Spline::getRefNbSides ()
{
    return this->m_nbSides;
}

//-----------------------------------------------------------------------------

inline const int& Spline::getCRefNbSides () const
{
    return m_nbSides;
}

//-----------------------------------------------------------------------------

inline void Spline::setNbSides (const int _nbSides)
{
    m_nbSides = _nbSides;
}

//-----------------------------------------------------------------------------

inline void Spline::setCRefNbSides (const int& _nbSides)
{
    m_nbSides = _nbSides;
}

//-----------------------------------------------------------------------------

inline const int Spline::getIdSpline () const
{
    return m_idSpline;
}

//-----------------------------------------------------------------------------

inline int& Spline::getRefIdSpline ()
{
    return m_idSpline;
}

//-----------------------------------------------------------------------------

inline const int& Spline::getCRefIdSpline () const
{
    return m_idSpline;
}

//-----------------------------------------------------------------------------

inline void Spline::setIdSpline (const int _idSpline)
{
    m_idSpline = _idSpline;
}

//-----------------------------------------------------------------------------

inline void Spline::setCRefIdSpline (const int& _idSpline)
{
    m_idSpline = _idSpline;
}

//-----------------------------------------------------------------------------

} // namespace fwData

#endif // __FWDATA_SPLINE_HPP__


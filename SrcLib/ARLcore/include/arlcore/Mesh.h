/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _ARLCORE_MESH_H
#define _ARLCORE_MESH_H
#include <arlcore/Common.h>

#include <vector>

#include <vnl/vnl_vector_fixed.h>

#ifdef ANN
#include <ANN/ANN.h>
#endif // ANN

#include <arlcore/PointsList.h>

namespace arlCore
{
    class Mesh
    {
    /**
     * @class   Mesh
     * @author  IRCAD (Research and Development Team)
     * @date    04/2009
     * @brief   List of triangular shapes
     */
    public:
        //! @brief Constructor (Value of scalar for each point)
        ARLCORE_API Mesh( double pointScalar=0.0 );

        //! @brief Copy constructor
        ARLCORE_API Mesh( const Mesh& );

        //! @brief Constructor from file of triangles (see load)
        ARLCORE_API Mesh( const std::string& fileName );

        //! @brief Destructor
        ARLCORE_API ~Mesh( void );

        //! @brief Copy
        ARLCORE_API void copy( const Mesh& );

        //! @brief Clear
        ARLCORE_API void clear( void );

        //! @brief Add new triangle with 3 points
        ARLCORE_API bool addTriangle( const arlCore::Point &P1, const arlCore::Point &P2, const arlCore::Point &P3 );

        //! @return Surface of the ith [0, n[ triangle else -1
        ARLCORE_API double triangleSurface( unsigned int i ) const;

        //! @return RMS distance between each point of all triangles
        ARLCORE_API double getRMSDistance( void ) const;

        //! @return Distance between a point and the mesh. NoTriangle for the closest [0, n[
        ARLCORE_API double computeDistance( const arlCore::Point&, unsigned int &noTriangle ) const;
        ARLCORE_API double computeDistance( const arlCore::Point& ) const;

        //! @return Squared distance between a point and the mesh. NoTriangle for the closest [0, n[
        ARLCORE_API double computeDistance2( const arlCore::Point&, unsigned int &noTriangle ) const;
        ARLCORE_API double computeDistance2( const arlCore::Point& ) const;

        //! @brief Generate a surface where altitude is on X-axis
        ARLCORE_API bool generateX( double ySize, double zSize, double yStep, double zStep, double degree, double gaussianNoise, arlCore::PointList& summits );

        //! @brief Generate a surface where altitude is on Y-axis
        ARLCORE_API bool generateY( double xSize, double zSize, double xStep, double zStep, double degree, double gaussianNoise, arlCore::PointList& summits );

        //! @brief Generate a surface where altitude is on Z-axis
        ARLCORE_API bool generateZ( double xSize, double ySize, double xStep, double yStep, double degree, double gaussianNoise, arlCore::PointList& summits );

        //! @brief Load a mesh from a file (.trian, .raw or Filetype=Surface, =Planes)
        ARLCORE_API bool load( const std::string& fileName, double gaussianNoise=0.0 );

        //! @brief Save a mesh in following formats : .trian, .raw
        ARLCORE_API bool save( const std::string& fileName, bool overwrite=true ) const;

        //! @brief Sample mesh with triangle sizes lesser than step (mm)
        ARLCORE_API bool resampling( double step );

        //! @return reference on the point list of triangles
        ARLCORE_API const arlCore::PointList& getPointList( void ) const;

        //! @return reference on the triangle list
        ARLCORE_API const std::vector< vnl_vector_fixed<unsigned int, 3> >& getTriangles( void ) const;

    private:
        //! @brief Add new point & return index of the point
        unsigned int addPoint( const arlCore::Point& p, double gaussianNoise=0.0 );

        //! @brief Add new triangle with points which references in m_pointList are (a,b,c)
        bool addTriangle( unsigned int a, unsigned int b, unsigned int c );

        //! @brief Get length of each side
        vnl_vector_fixed<double,3> getLengths( unsigned int noTriangle ) const;

        //! @brief Delete duplicate points
        bool simplify( void );

        //! @brief Generate a surface where altitude is on specific axis (X=0, Y=1, Z=2)
        bool generate( unsigned int axis, double width, double length, double stepW, double stepL, double degree, double gaussianNoise, arlCore::PointList& summits );

        bool releaseTriangle( unsigned int noTriangle );

        unsigned int releasePoint( unsigned int noPoint );

        unsigned int replacePointIndex( std::vector<unsigned int> oldIndex, unsigned int newIndex );

        double m_pointScalar;
        arlCore::PointList m_pointList;
        std::vector< vnl_vector_fixed<unsigned int, 3> > m_triangles;
        std::vector< unsigned int > m_nbReferences;
        std::vector< unsigned int > m_freePoints;
        std::vector< unsigned int > m_freeTriangles;
    };
} // namespace arlCore
#endif // _ARLCORE_MESH_H

/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _ARLCORE_POINTLIST_H
#define _ARLCORE_POINTLIST_H
#include <arlcore/Common.h>

#include <vector>

#include <vgl/vgl_plane_3d.h>
#include <vgl/vgl_homg_plane_3d.h>

#ifdef ANN
#include <ANN/ANN.h>
#endif // ANN

#include <arlcore/Object.h>
#include <arlcore/Point.h>
#include <arlcore/MatrixR.h>

namespace arlCore
{
    enum ARLCORE_SCE { ARLCORE_SCE_UNKNOWN, ARLCORE_SCE_ANALYTIC, ARLCORE_SCE_GEOMETRIC, ARLCORE_SCE_OPTIMIZATION, ARLCORE_SCE_4POINTS, ARLCORE_SCE_NBMETHOD };
    static std::string ARLCORE_SCE_NAMES[ARLCORE_SCE_NBMETHOD]={ "ARLCORE_SCE_UNKNOWN", "ARLCORE_SCE_ANALYTIC", "ARLCORE_SCE_GEOMETRIC", "ARLCORE_SCE_OPTIMIZATION", "ARLCORE_SCE_4POINTS"};

    //! Point container
    /*!
     * Points that are added to the contained must have the same dimension (by default 3)
     *
     */
    class PointList : public Object
    {
    /**
     * @class   PointList
     * @author  IRCAD (Research and Development Team)
     * @date    2007
     * @brief   Point list with same dimensions. Added point are copied in the container.
     * Point destruction are managed by PointList. This class has been created to manipulate
     * easily a point list. Point list are used in most of registration algorithm (ISPPC, EPPC
     * ICP, ARTAG).  You can find also some useful functions that provide you for example:
     * - the best plane that fits your data
     * - the best line that fits your data
     * - decimation of the point cloud
     * - a point cloud randomly chosen in a specific shape
     * - if the points have been acquired on a surface that is a sphere, a function estimate
     * the center of this sphere
     * - other nice function...
     */
    public:
        //! @brief Constructor : An empty list of points with dimension dim
        ARLCORE_API PointList( unsigned int dim=3, const std::string &name="" );

        //! @brief Copy constructor
        ARLCORE_API PointList( const PointList& );

        //! @brief Constructor with the list of points
        ARLCORE_API PointList( const std::vector< const Point* >& list );

        //! @brief Constructor : Fill the list of elements of 'l' which scalar is include in [scalarMin , scalarMax] 
        ARLCORE_API PointList( const std::vector< const Point* >& l, double scalarMin, double scalarMax );

        //! @brief Affectation
        ARLCORE_API PointList& operator=( const PointList& );

        //! @brief Copy
        ARLCORE_API void copy( const PointList& );

        //! @brief Destructor
        ARLCORE_API ~PointList( void );

        ARLCORE_API std::string getString( void ) const;

        /**
         * @brief Save a file of points (arlPointList, gnuplot, .vtk)
         * @remark See arlCore::ARLCORE_POINT_SAVE_TYPE
         * @param[in] type Format of file
         * @param[in] justVisible If true save just visible points else all of them
         * @param[in] overwrite If true, overwrite file
         */
        ARLCORE_API bool save( const std::string &fileName, ARLCORE_POINT_SAVE_TYPE type=ARLCORE_POINT_SAVE_FULL, bool justVisible=false, bool overwrite=true ) const;

        /**
         * @brief Load a file of points (arlPointList, .trian, .raw)
         * @param[in] filename
         * @param[in] step If .trian or .raw, reechantillonne les triangles avec un pas en mm
         * @remark If step<=0, load only points of the triangle
         */
        ARLCORE_API bool load( const std::string &fileName, double step=0 );

        //! @brief Fill the pointlist with origin of each transformation
        ARLCORE_API unsigned int fill( const std::vector< arlCore::vnl_rigid_matrix >& );

        //! @return Reference on the intern list of points
        ARLCORE_API const std::vector< Point::sptr >& getList( void ) const;

        //! @return Reference on the ith point of the list
        ARLCORE_API Point* operator[]( unsigned int i );

        //! @return Constant reference on the ith point of the list
        ARLCORE_API const Point* operator[]( unsigned int i ) const;

        //! @return Reference on the ith point of the list
        ARLCORE_API Point* get( unsigned int i );

        //! @return Constant reference on the ith point of the list
        ARLCORE_API const Point* get( unsigned int i ) const;

        //! @return Reference on the last point of the list
        ARLCORE_API Point* back( void );

        //! @return Constant reference on the last point of the list
        ARLCORE_API const Point* back( void ) const;

        //! @brief Push all points of l at the end of the current list
        ARLCORE_API unsigned int push_back( const PointList& l );

        //! @brief Push a point at the end of the list
        ARLCORE_API bool push_back( const Point& );

        //! @brief Push the (x,y) 2D point at the end of the list
        ARLCORE_API bool push_back( double x, double y );

        //! @brief Push the (x,y,z) 3D point at the end of the list
        ARLCORE_API bool push_back( double x, double y, double z );

        //! @brief Delete the last point of the list
        ARLCORE_API void pop_back( void );

        //! @brief Clear the list and delete included points
        ARLCORE_API void clear( void );

        //! @brief Add a gaussian noise on all points of the list
        ARLCORE_API unsigned int addGaussianNoise( double standardDeviation );

        //! @return Dimension of the points of the list
        ARLCORE_API unsigned int getDimension( void ) const;

        /**
         * @brief Set a new dimension of points if the list is empty
         * @return False if the list is not empty
         */
        ARLCORE_API bool setDimension( unsigned int );

        //! @return Number of points in the list
        ARLCORE_API unsigned int size( void ) const;

        //! @return Number of visible points in the list
        ARLCORE_API unsigned int visibleSize( void ) const;

#ifdef ANN
        //! @brief Generate an ANNpointArray from the point list without the points 0
        ARLCORE_API unsigned int generateANN( ANNpointArray& ) const;

        // TODO : Use ANN kd_tree with ANNpointArray generated or updated
#endif // ANN

        /**
         * @brief : Compute the bounding box of the point cloud, the gravity center and the min & max scalar
         * boundingBox1 : Left inferior corner : Each minimum coordinates
         * boundingBox2 : Right superior corner : Each maximum coordinates
         * std : standard deviation of the point cloud
         */
        ARLCORE_API bool properties( Point &gravity, Point &boundingBox1, Point &boundingBox2, double &minscalar, double &maxscalar, double  &std ) const;

        /**
         * @brief Find in the list the nearest point of pt
         * pos : Position in the list of the found point
         * distance : Distance between pt and the found point
         * @return False if the list is empty
         */
        ARLCORE_API unsigned int findNearPoint( const Point &pt, std::vector< unsigned int > &pos, double &distance, double scalar=-1.0 ) const;

        /**
         * @brief Return a point list randomly chosen in a specified shape
         * param[in] nb : number of points you want
         * param[in] type : shape in which the points will be chosen (see ARLCORE_SHAPE definition in Point.h)
         * param[in] centre : Center of the shape
         * param[in] size : Size of the shape
         * param[in] angle : Angle of the shape or 0
         * @return Number of generated points
         * @remark See Point::shapeRandom for more details
         */
        ARLCORE_API unsigned int shapeRandom( unsigned int nb, ARLCORE_SHAPE type, const Point &centre, const double size, const double angle=0 );

        /**
         * @brief The function randomly choose N points within the point list A and put them in *this
         * param[in] A List you want to decimate
         * param[in] n Number of point you want to keep
         * @return Number of point in *this
         */
        ARLCORE_API unsigned int randomList( const arlCore::PointList &A, unsigned int n );

        /**
         * @brief Compute the plane that best fits the points which m_scalar == scalar, if scalar = O, 
         * all the points are used. The method is based on an optimization. This should be improved
         * using an SVD method (TODO ).
         * Warning : Run only with 3D points
         * @param[in] outliers = n = [0,1] Elimine n pourcent des points les plus loin, sauf au moins 3
         * @param[out] rms Average distance between points and the computed plane
         * @param[out] min Minimum distance between points and the computed plane
         * @param[out] max Maximum distance between points and the computed plane
         * @return Number of points used
         */
        ARLCORE_API unsigned int plane( vgl_plane_3d< double >& plane, double &rms, double &min, double &max, double scalar=0, double outliers=0.0 ) const;
        ARLCORE_API unsigned int plane( vgl_homg_plane_3d< double >& plane, double &rms, double &min, double &max, double scalar=0, double outliers=0.0 ) const;

        /**
         * @brief From a cloud of 3D points that should be on a sphere cap
         * this function estimate the sphere center
         * @param[out] centerEstimation Result of the sphere center estimation
         * @param[in] type 3 different methods : one is analytic, one is geometric and the last by optimization
         * more explanations on the methods are provided in PointList.cpp
         * @todo Compute the covariance on the estimation
         */
        ARLCORE_API bool sphereCenterEstimation( Point &centerEstimation, double &radius, ARLCORE_SCE type, std::vector<double> &optimiser_parameter, std::vector<double> &log ) const;

        /**
         * @brief This method is used to estimate the center of a sphere from several points on
         * its surface. The method computes the circumcircle of a triangle and return
         * 1) the center of this circle
         * 2) the plane that contains this center
         * The center of the sphere lies on the line that passes through the center of the circle
         * and that is orthogonal to the plane
         */
        ARLCORE_API bool triangleCircumcircleOrthoLine( arlCore::Point & centerEstimation, vgl_plane_3d< double > &trian_plane) const;

        /**
         * @brief This method computes from 2 lines (represented by the first (P1 P2) and second point pair (P3 P4) )
         * the intersection. If the 2 lines does not intersect, it provides the closest point
         * of the two lines. The method is taken from Paul Bourke on internet.
         * This function computes the shortest line segment (Pa and Pb) between the two lines.
         * This line segment being perpendicular to the two lines, this allows us to write
         * two equations for the dot product as:
         *  (Pa - Pb) dot (P2 - P1) = 0     and        (Pa - Pb) dot (P4 - P3) = 0
         *
         *  Since Pa lies on P1P2 it can be written Pa = P1 + mua (P2 - P1)
         *  and Pb lies on P3P4 it can also be written : Pb = P3 + mub (P4 - P3)
         *  where mua and mua are unknown coefficient that we want to solve
         *
         *  Expanding then the equation of the lines
         *  ( P1 - P3 + mua (P2 - P1) - mub (P4 - P3) ) dot (P2 - P1) = 0
         *  ( P1 - P3 + mua (P2 - P1) - mub (P4 - P3) ) dot (P4 - P3) = 0
         *
         *  Expanding these in terms of the coordinates (x,y,z) gives the following result:
         *  d_1321 + mua d_2121 - mub d_4321 = 0
         *  d_1343 + mua d_4321 - mub d_4343 = 0
         *  where d_mnop = (xm - xn)(xo - xp) + (ym - yn)(yo - yp) + (zm - zn)(zo - zp)
         *  Finally, solving for mua gives
         *  mua = ( d_1343 d_4321 - d_1321 d_4343 ) / ( d_2121 d_4343 - d_4321 d_4321 )
         *  and backsubstituting gives mub
         *  mub = ( d_1343 + mua d_4321 ) / d_4343
         *
         *  log contains the length of the segment PaPb
         *
         */
        ARLCORE_API bool twoLinesClosestPoint( arlCore::Point & closestPoint, double &log) const;

        //! @brief Compute the main direction of a point cloud with a least square fitting
        ARLCORE_API bool directionEstimation( Point &vector, double &RMS  );

        /**
         * @brief : Compute the gravity center of the point cloud and the covariance
         * matrix of the points
         * @param[out] pt : pt is modified and contains the information of gravity center and covariance
         * @return Number of point that are used.
         */
        ARLCORE_API unsigned int statistic( Point& pt ) const;

        /**
         * @brief Visualize the pointlist with gnuplot
         */
        ARLCORE_API bool plot( const std::string &options="" ) const;
        ARLCORE_API bool SLplot( void ) const;

        /**
         * @brief Apply to the point cloud a specific temporal behavior :
         * points are collapsing toward the gravity center with gravity g
         */
        ARLCORE_API bool collapse( double g=1.0 );

        /**
         * @brief Calibration of a tag from several poses
         * @param[in] tag
         * @param[in] delta Variation on the criterion (Pourcentage)
         * @param[out] xLength Length of the x-axis side
         * @param[out] yLength Length of the y-axis side
         * @param[in] nbMaxIterations
         * @return Number of iterations
         */
        ARLCORE_API unsigned int tagCalibration( const std::vector<arlCore::PointList> &tag, double delta, unsigned int nbMaxIterations, double &xLength, double &yLength );

    private:
        unsigned int m_dimension;
        std::vector< Point::sptr > m_pointList;
#ifdef ANN
        // TODO : If m_enableANNArray update m_ANNPointArray with each coordinate data_block()
        std::vector< double* > m_ANNPointArray;
        bool m_enableANNArray;
#endif // ANN
    };

    ARLCORE_API bool save( const std::vector< const Point* > &pl, const std::string &fileName, ARLCORE_POINT_SAVE_TYPE type=ARLCORE_POINT_SAVE_FULL, bool justVisible=false, bool overwrite=true );
    ARLCORE_API bool save( const std::vector< Point* > &pl, const std::string &fileName, ARLCORE_POINT_SAVE_TYPE type=ARLCORE_POINT_SAVE_FULL, bool justVisible=false, bool overwrite=true );

//  ARLCORE_API bool save( const std::vector<PointList>& lists, const std::string &fileName, ARLCORE_POINT_SAVE_TYPE type=ARLCORE_POINT_SAVE_FULL, bool overwrite=true );

    /**
     * @brief : Compute the distance betwween the points p1 and p2
     * p1 & p2 must have same dimension
     * If p1 & p2 do not have same dimension, the smallest list size will be used
     */
    ARLCORE_API bool distance( const PointList &p1, const PointList &p2, std::vector<double> &distances );
    ARLCORE_API unsigned int findNearPoint( const std::vector< Point::csptr >&, const Point &pt, std::vector< unsigned int > &pos, double &distance, double scalar=-1.0);
    //ARLCORE_API unsigned int findNearPoint( const std::vector< Point*>&, const Point &pt, std::vector< unsigned int > &pos, double &distance, double scalar=-1.0);
    ARLCORE_API unsigned int statistic( const std::vector< const Point*>&, unsigned int dimension, Point& pt );
    ARLCORE_API unsigned int statistic( const std::vector< Point*>&, unsigned int dimension, Point& pt );
    ARLCORE_API unsigned int matching3D3D( const PointList &a, const PointList &b, double gaussianError, double decimage, std::vector< const Point* > &Va, std::vector< const Point* > &Vb );
} // namespace arlCore
#endif // _ARLCORE_POINTLIST_H

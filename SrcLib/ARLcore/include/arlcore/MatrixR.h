/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _ARLCORE_MATRIXR_H
#define _ARLCORE_MATRIXR_H
#include <arlcore/Common.h>

#include <vector>
#include <string>

#include <vgl/vgl_line_3d_2_points.h>
#include <vgl/vgl_line_segment_3d.h>

#include <vnl/vnl_quaternion.h>
#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_matrix_fixed.h>

#include <arlcore/Object.h>
#include <arlcore/vnl_rotation3d_matrix.h>

namespace arlCore
{
    enum ARLCORE_REGISTER3D3D { ARLCORE_REGISTER3D3D_UNKNOWN, ARLCORE_REGISTER3D3D_LM, ARLCORE_REGISTER3D3D_POW, ARLCORE_REGISTER3D3D_GC, ARLCORE_REGISTER3D3D_NBTYPES };
    static std::string ARLCORE_REGISTER3D3D_NAMES[ARLCORE_REGISTER3D3D_NBTYPES]={ "ARLCORE_REGISTER3D3D_UNKNOWN", "ARLCORE_REGISTER3D3D_LM",
    "ARLCORE_REGISTER3D3D_POW", "ARLCORE_REGISTER3D3D_GC"};

    enum ARLCORE_VRM_DISTANCE_METHOD { ARLCORE_VRM_DISTANCE_UNKNOWN, ARLCORE_VRM_DISTANCE_3AXIS, ARLCORE_VRM_DISTANCE_TRANSLATION,
        ARLCORE_VRM_DISTANCE_OX, ARLCORE_VRM_DISTANCE_OY, ARLCORE_VRM_DISTANCE_OZ, ARLCORE_VRM_DISTANCE_NBTYPES};
    static std::string ARLCORE_VRM_DISTANCE_METHOD_NAMES[ARLCORE_VRM_DISTANCE_NBTYPES]={ "ARLCORE_VRM_DISTANCE_UNKNOWN",
        "ARLCORE_VRM_DISTANCE_3AXIS", "ARLCORE_VRM_DISTANCE_TRANSLATION",
        "ARLCORE_VRM_DISTANCE_OX", "ARLCORE_VRM_DISTANCE_OY", "ARLCORE_VRM_DISTANCE_OZ" };

    class Point;
    class PointList;
    class vnl_rigid_vector;
    /**
     * @author  IRCAD (Research and Development Team)
     * @date    2007
     * @brief   Matrice de transformation rigide
     */
    class vnl_rigid_matrix: public vnl_matrix_fixed<double,4,4>, public Object
    {
    public:
        //! @brief Default constructor : Identity
        ARLCORE_API vnl_rigid_matrix( void );

        //! @brief Copy constructor
        ARLCORE_API vnl_rigid_matrix( const vnl_rigid_matrix& );

        //! @brief Constructor
        ARLCORE_API vnl_rigid_matrix( const vnl_rigid_vector& );

        //! @brief Constructor
        ARLCORE_API vnl_rigid_matrix( const vnl_matrix_fixed<double,4,4>& );

        //! @brief Constructor by composition with a rotation matrix and a translation
        ARLCORE_API vnl_rigid_matrix( const vnl_rotation3d_matrix &rotation, const vnl_vector_fixed<double,3> &translation);

        //! @brief Constructor by loading
        ARLCORE_API vnl_rigid_matrix( const std::string &fileName );

        //! @brief Destructor
        ARLCORE_API ~vnl_rigid_matrix( void );

        //! @return Is it a valid rigid matrix : Homogen, real rotation matrix
        ARLCORE_API bool isValid( void ) const;

        //! @return Description of the current matrix
        ARLCORE_API std::string getString( void ) const;

        //! @return Description of the current matrix for planesystem
        ARLCORE_API std::string getText( void ) const;

        //! @brief Save the current matrix in a file
        ARLCORE_API bool save( const std::string &fileName, bool overwrite=true ) const;

        //! @brief Save the current matrix in a file stream
        ARLCORE_API bool save( std::fstream &f ) const;

        //! @brief Load in the current matrix from a file
        ARLCORE_API bool load( const std::string &fileName );

        //! @brief Load in the current matrix from a file stream
        ARLCORE_API bool load( std::fstream &f );

        ARLCORE_API std::string serialize( void ) const;
        ARLCORE_API bool virtual unserialize( std::stringstream &s ); // PROTECTED ?

        //! @brief Affectation
        ARLCORE_API vnl_rigid_matrix& operator=( const vnl_rigid_matrix& );

        //! @brief Affectation
        ARLCORE_API vnl_rigid_matrix& operator=( const vnl_rigid_vector& );

        //! @brief Affectation
        ARLCORE_API vnl_rigid_matrix& operator=( const vnl_matrix_fixed<double,4,4>& );

        /**
         * @brief Gaussian noise on translation
         * param[in] Xnoise Gaussian noise on X (mm)
         * param[in] Ynoise Gaussian noise on Y (mm)
         * param[in] Znoise Gaussian noise on Z (mm)
         * @return Delta distance
         */
        ARLCORE_API double GaussianNoiseTranslation( double Xnoise, double Ynoise, double Znoise );

        /**
         * @brief Gaussian noise on translation
         * param[in] Xnoise Gaussian noise on X axis (degrees)
         * param[in] Ynoise Gaussian noise on Y axis (degrees)
         * param[in] Znoise Gaussian noise on Z axis (degrees)
         * @return Delta angle
         */
        ARLCORE_API double GaussianNoiseRotation( double Xnoise, double Ynoise, double Znoise );

        /**
         * @brief Compute several distance types between current transformation and T
         * param[in] T Compared transformation
         * param[in] method
         * 3axis : RMS of distance between all unit vectors
         * Translation : Distance between origins
         * Ox : RMS of distances between origins and unit vector on Ox
         * Oy : RMS of distances between origins and unit vector on Oy
         * Oz : RMS of distances between origins and unit vector on Oz
         * param[out] errors2 Each computed square distance
         * @return Square of RMS of distances between unit vectors or <0 if error
         */
        ARLCORE_API double distance2( const vnl_rigid_matrix &T, ARLCORE_VRM_DISTANCE_METHOD method, std::vector<double> &errors ) const;
        ARLCORE_API double distance2( const vnl_rigid_matrix &T, ARLCORE_VRM_DISTANCE_METHOD method ) const;

        /**
         * @brief Compute several distance types between current transformation and T like distance2
         * @return RMS of distances between unit vectors or <0 if error
         */
        ARLCORE_API double distance( const vnl_rigid_matrix &T, ARLCORE_VRM_DISTANCE_METHOD method=ARLCORE_VRM_DISTANCE_3AXIS ) const;

        /**
         * @brief Compare current transformation Trf with T
         * This means that we compute Trf * T^(-1) and extract the 2-norm of the translation
         * and the angle of the rotation matrix
         * @param[out] errorTranslation Distance, norm (mm)
         * @param[out] errorRotation Angle (deg)
         */
        ARLCORE_API void compare( const vnl_rigid_matrix &T, double &errorTranslation, double &errorRotation ) const;

        //! @brief The same as 'compare' with the inverse of the current matrix
        ARLCORE_API void compareInverse( const vnl_rigid_matrix &T, double &errorTranslation, double &errorRotation ) const;

        //! @brief Copy
        ARLCORE_API bool copy( const vnl_rigid_matrix& );

        //! @brief Copy
        ARLCORE_API bool copy( const vnl_rigid_vector& );

        //! @brief Copy
        ARLCORE_API bool copy( const vnl_matrix_fixed<double,4,4>& );

        //! @brief Set the current matrix to identity
        ARLCORE_API bool setIdentity();

        /**
         * @brief Set a random rotation and translation
         * The new origin is included in a cube of 'size' of side
         */
        ARLCORE_API void uniform_random( double size );

        //! @return Translation part of the current matrix
        ARLCORE_API vnl_vector_fixed<double,3> getTranslation( void ) const;

        //! @return Homogen translation of the current matrix
        ARLCORE_API vnl_vector_fixed<double,4> getHTranslation( void ) const;

        //! @return Rotation vector of the current matrix
        ARLCORE_API vnl_rotation3d_matrix getRotation( void ) const;

        //! @return Rotation in quaternion
        ARLCORE_API vnl_quaternion<double> getQuaternion( void ) const;

        //! @brief Change the current translation
        ARLCORE_API bool setTranslation( const vnl_vector_fixed<double,3> & );

        //! @brief Change the current translation
        ARLCORE_API bool setTranslation( double x, double y, double z );

        //! @brief Change the x value of the current translation
        ARLCORE_API bool setX( double x );

        //! @brief Change the y value of the current translation
        ARLCORE_API bool setY( double y );

        //! @brief Change the z value of the current translation
        ARLCORE_API bool setZ( double z );

        //! @return x value of the current translation
        ARLCORE_API double getX( void ) const;

        //! @return y value of the current translation
        ARLCORE_API double getY( void ) const;

        //! @return z value of the current translation
        ARLCORE_API double getZ( void ) const;

        //! @brief Change the current rotation
        ARLCORE_API bool setRotation( const vnl_rotation3d_matrix & );

        //! @brief Change the current rotation
        ARLCORE_API bool setQuaternion( const vnl_quaternion<double> & );

        //! @return Current matrix = T1 * T2
        ARLCORE_API bool mult( const vnl_rigid_matrix &T1 , const vnl_rigid_matrix &T2 );

        /**
         * @return Current matrix = invert(T)
         * @remark Inverse operation is optimized for rigid transformation
         */
        ARLCORE_API bool invert( const vnl_rigid_matrix&T );

        /**
         * @brief Inverse the current matrix
         * @remark Inverse operation is optimized for rigid transformation
         */
        ARLCORE_API bool invert( void );

        //! @return The inverse of the current matrix
        ARLCORE_API arlCore::vnl_rigid_matrix computeInverse( void ) const;

        /**
         * @brief The current matrix becomes the mean of listMatrix
         * Use filter with the ARLCORE_TRF_FILTER_CONSTANT option
         */
        ARLCORE_API bool mean( const std::vector<arlCore::vnl_rigid_matrix> &listMatrix );

        //! @brief Compute the angle (in degrees) between the planes OXY of the 2 planes
        ARLCORE_API double getAngleOXY( void );

        //! @brief Compute the angle (in degrees) between the planes OXZ of the 2 planes
        ARLCORE_API double getAngleOXZ( void );

        //! @brief Compute the angle (in degrees) between the planes OYZ of the 2 planes
        ARLCORE_API double getAngleOYZ( void );


        /**
         * @brief Registration of 2 matching 3D points sets - Direct method
         * Current matrix (this) = A->B
         * Registration gives Bi = (this)*Ai with only pairs of visible points
         * @param[in] computeRMS If true, compute RMS of registration (mm)
         * Warning Only if size(A)==size(B) and with pairs of visible points
         * @return False if size(A)!=size(B) or there are lesser than 3 pairs of visible points
         */
        ARLCORE_API bool register3D3D( const PointList &A, const PointList &B, bool computeRMS );

        //! @brief Same method as the previous one
        ARLCORE_API bool register3D3D( const std::vector<const Point*> &A, const std::vector<const Point*> &B, bool computeRMS );

        /**
         * @brief Iterative optimization of sum trp(TxA_i - B_i). sigma_3D_Bi^(-1) . (TxA_i - B_i)
         * where sigma_3D_Bi is the covariance matrix of the B_i
         * When using LM optimization, the number of residual must be superior than the number of unknown
         * if not it returns the initialisation
         * @param[in] a point list of the first model
         * @param[in] b point list of the second model
         * @param[in] ARLCORE_REGISTER3D3D method that can be used for the optimization (LM, powell or conjugate gradient)
         * @param[in] optimiserParameters parameter for the optimization (TODO not used here)
         * @param[out] log[0] = end_error of the optimization (TODO still has to be divided by the number of point and sqrt)
         * @param[out] log[1] = start_error of the optimization (TODO still has to be divided by the number of point and sqrt)
         */
        ARLCORE_API bool register3D3DUncertainty( const PointList&a, const PointList&b, ARLCORE_REGISTER3D3D, std::vector<double> optimiserParameters, std::vector<double> &log/*, unsigned int numberOfPoints=0*/ );

        /**
         * @brief Evaluation of the register RMS and the standard deviation
         * src & dst are 2 lists of matching points
         * if n>0, the calculus is made with only the first n points
         * if n==0, all points are used for the RMS evaluation
         * @param[out] this->m_registerRMS contains after computation the registration RMS sqrt(mean(sum || Bi-T*Ai||^2))
         * @param[out] this->m_registerStdDev contains after computation the standard deviation on the
         * distance di between Bi and T*Ai : sqrt (sum (||di - average(di) ||^2)/ n) )
         * @param[out] errors is a double vector that contains the distance between Bi and T*A_i for each point
         */
        ARLCORE_API bool RMS3D3D( const PointList&src, const PointList&dst, std::vector<double> &errors );

        /**
         * @brief Registration of two 3D points sets. The correspondences between the points
         * are not known and are estimated in the function. The matching results is not available
         * TODO add a variable to return the matching result
         * @param[in] computeRMS If true compute registration RMS
         * @param[in] gaussianError Std error estimated on measured 3D points (mm)
         * @param[in] decimage ]0,1] 1:Use whole information 0.6:Use only 60% of information (faster)
         * @param[out] B = this * A
         */
        ARLCORE_API bool register3D3DwithoutMatching( const PointList &A, const PointList &B, bool computeRMS=false, double gaussianError=0.0, double decimage=1.0 );

        /**
         * @brief Registration of 2 clouds of 3D points
         * @param[in] Model Cloud of points of scanner for example
         * @param[in] Acquisition Cloud of points of the surface issue of structured light for example
         * param[out] firstRMS RMS a l'initialisation
         * param[out] lastRMS RMS a la fin du recalage
         * param[out] iterations Number of iterations
         * param[in] justVisible If true, register with just visible points else with all points
         * param[in] RMSMax Stop registration when the RMS is under 'RMSMax'
         * param[in] iterationsMax Stop registration before 'iterationMax' iterations
         * @return true if succeed
         * @remark The current value of the matrix is used as an initialization
         * If you don't know it, don't forget to set the matrix to identity
         * Warning : Respect the order : Model / Point cloud
         * Model devra etre surechantillonne
         */
        ARLCORE_API bool registerICP( const PointList &Model, const PointList &cloud,
                        double &firstRMS, double &lastRMS, unsigned int &iterations, bool justVisible=false, double RMSMax=0.01, unsigned int iterationsMax=500 );

        //! @brief Deprecated
        ARLCORE_API bool oldRegisterICP( const PointList&a, const PointList&b,
                        double &RMS, unsigned int &iterations, double RMSMax=0.01, unsigned int iterationsMax=500 );

        //! @brief Compute matrix of changement of plane with cordinates of new Origin, unit X & Z
        ARLCORE_API bool chgPlaneOXZ( const arlCore::Point &O, const arlCore::Point &X, const arlCore::Point &Z );

        //! @return pt2 <- this * pt1
        ARLCORE_API bool trf( const Point &pt1, Point &pt2 ) const;

        //! @return pt <- this * pt
        ARLCORE_API bool trf( Point &pt ) const;

        //! @return For each i, Bi <- this * Ai
        ARLCORE_API unsigned int trf( const PointList &A, PointList &B ) const;

        //! @return For each i, Ai <- this * Ai
        ARLCORE_API unsigned int trf( PointList &A ) const;

        //! @return For each i, Bi <- this * Ai
        ARLCORE_API unsigned int trf( const std::vector< const Point* >&A, std::vector< const Point* >&B ) const;

        //! @return Change the plane of the line equation : line2 <- this * line1
        ARLCORE_API bool trf( const vgl_line_3d_2_points<double> &line1, vgl_line_3d_2_points<double> &line2 ) const;

        //! @return Change the plane of the line equation : line <- this * line
        ARLCORE_API bool trf( vgl_line_3d_2_points<double> &line ) const;

        //! @return Change the plane of the segment equation : line2 <- this * line1
        ARLCORE_API bool trf( const vgl_line_segment_3d<double> &line1, vgl_line_segment_3d<double> &line2 ) const;

        //! @return Change the plane of the segment equation : line <- this * line
        ARLCORE_API bool trf( vgl_line_segment_3d<double> &line ) const;

        /**
         * @brief RMS (Root Mean Square) : Mean error from the computation of the matrix
         * When the matrix is the result of a calibration or registration,...
         */
        ARLCORE_API double getRMS( void ) const;

        //! @return Standard deviation associated to RMS
        ARLCORE_API double getStdDev( void ) const;

        //! @brief Set RMS
        ARLCORE_API void setRMS( double RMS );

        //! @brief Set standard deviation associated to RMS
        ARLCORE_API void setStdDev( double stdDev );

    private:
        double m_registerRMS;
        double m_registerStdDev;
    };

    //! @brief Rotation of thetaX around x-axis
    ARLCORE_API vnl_matrix_fixed<double,4,4> rotX(double thetaX); // degree
    //! @brief Rotation of thetaY around y-axis
    ARLCORE_API vnl_matrix_fixed<double,4,4> rotY(double thetaY); // degree
    //! @brief Rotation of thetaZ around z-axis
    ARLCORE_API vnl_matrix_fixed<double,4,4> rotZ(double thetaZ); // degree
    //! @brief Rotation of thetaX around x-axis, thetaY around y-axis and thetaZ around z-axis
    ARLCORE_API vnl_matrix_fixed<double,4,4> rotXYZ(double thetaX, double thetaY, double thetaZ); // degree
    //! @brief Rotation of alpha degrees around the vector U(x,y,z)
    ARLCORE_API vnl_matrix_fixed<double,4,4> rotVecFromOrigin(double Ux, double Uy, double Uz, double alpha); // degree
    //! @brief Rotation of alpha degrees around the vector U(x,y,z) from Point P(x,y,z)
    ARLCORE_API vnl_matrix_fixed<double,4,4> rotVecFromPoint(double Px, double Py, double Pz, double Ux, double Uy, double Uz, double alpha); // degree

    enum ARLCORE_TRF_FILTER_TYPE { ARLCORE_TRF_FILTER_CONSTANT, ARLCORE_TRF_FILTER_LINEAR, ARLCORE_TRF_FILTER_LOG, ARLCORE_TRF_FILTER_SQUARE, ARLCORE_TRF_FILTER_CUBIC  };

    //! @brief Construit une transformation rigide en appliquant un filtre a une liste temporelle de transformations
    // ARLCORE_TRF_FILTER_CONSTANT : No time filter, the result is the mean of transformations
    ARLCORE_API unsigned int filter( const std::vector<const arlCore::vnl_rigid_matrix*> &list , ARLCORE_TRF_FILTER_TYPE, vnl_rigid_matrix &average );

} // namespace arlCore
#endif // _ARLCORE_MATRIXR_H

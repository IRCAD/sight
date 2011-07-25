/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _ARLCORE_CAMERA_H
#define _ARLCORE_CAMERA_H
#include <arlcore/Common.h>

#include <string>

#include <vnl/vnl_matrix_fixed.h>
#include <vnl/vnl_vector_fixed.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_line_2d.h>
#include <vgl/vgl_plane_3d.h>

#include <arlcore/Object.h>
#include <arlcore/Particle.h>
#include <arlcore/MatrixR.h>

namespace arlCore
{
    class PlaneSystem;
    class Point;
    /**
     * @class   Camera
     * @author  IRCAD (Research and Development Team)
     * @date    2007
     * @brief   Parametres physiques d'une camera (intrinsèques et extrinsèques)
     */
    class Camera : public Object, public Particle
    {
    public:
        //! @brief Constructor
        ARLCORE_API Camera( PlaneSystem &universe );

        //! @brief Constructor
        ARLCORE_API Camera( PlaneSystem &universe, const uint32HL &GUID );

        //! @brief Constructor by copy
        ARLCORE_API Camera( const Camera &c );

        //! @brief Destructor
        ARLCORE_API ~Camera( void );

        //! @brief Affectation
        ARLCORE_API Camera& operator=(const Camera&);

        //! @brief Copy
        ARLCORE_API void copy(const Camera&);

        //! @brief Initialization with default parameters
        ARLCORE_API void init( void );

        //! @return Description of the current camera
        ARLCORE_API std::string getString( void ) const;

        //! @brief Save the current camera into a file
        ARLCORE_API bool save( const std::string &fileName, bool overwrite=true ) const;

        //! @brief Load the current camera from a file
        ARLCORE_API bool load( const std::string &fileName );

        /**
        * @brief Création d'une caméra synthétique
        * @param[in] center Point vers lequel est oriente la camera
        * @param[in] viewPoint Position du centre optique de la camera
        * @param[in] intrinsicParams Parametres intrinseques = {fx;fy;cx;cy;k1;k2;p1;p2;k3;alphaC}
        * (Selon le modele choisi - ici par defaut modele de zhang)
        * @return False si le point de vue de la camera est identique au point qu'elle regarde, True sinon
        */
        ARLCORE_API bool syntheticCamera( const Point& center , const Point& viewPoint, const std::vector< double > &intrinsicParams);

        /**
        * @return Unique ID of calibration.
        * If several cameras have the same ID, that means they share the same reference plane
        * If equal to zero <=> Unique calibration reference
        */
        ARLCORE_API long int getCalibrationID( void ) const;

        /**
        * @brief Load appropriate parameters, functions of date and image size
        * filename point to a file with references of all avalaible calibrations
        */
        ARLCORE_API bool loadParameters( const std::string &fileName, const long int date, unsigned int width, unsigned int height );

        /**
        * @brief Load appropriate parameters, functions of date and image size
        * filename point to a file with references of all avalaible calibrations
        * Old file format : Deprecated
        */
        ARLCORE_API bool loadOldParameters( const std::string &fileName, long int date, unsigned int width, unsigned int height );

        //! @brief Set the extrinsic matrix with T
        ARLCORE_API bool setExtrinsic( const vnl_rigid_matrix &T );

        //! @return Extrinsic matrix
        ARLCORE_API const vnl_rigid_matrix& getExtrinsic( void ) const;

        //! @return Inverse of extrinsic matrix
        ARLCORE_API const vnl_rigid_matrix& getInvExtrinsic( void ) const;

        //! @return true, if intrinsic matrix is intialize
        ARLCORE_API bool isIntrinsicCalibrated( void ) const;

        //! @return Intrinsic matrix
        ARLCORE_API const vnl_matrix_fixed<double,3,3>& getIntrinsicMatrix( void ) const;

        //! @return Inverse of intrinsic matrix
        ARLCORE_API const vnl_matrix_fixed<double,3,3>& getInvIntrinsicMatrix( void ) const;

        //! @return Intrinsic vector = [fx,fy,cx,cy,kc0,kc1,kc2,kc3,kc4,alphaC)
        ARLCORE_API const vnl_vector<double> getIntrinsicVector( void ) const;

        //! @return Distortion vector = [k1,k2,p1,p2,k3]
        ARLCORE_API const vnl_vector_fixed<double,5>& getDistortion( void ) const;

        //! @return cx or u0 = x-coordinate of optical center
        ARLCORE_API double getcx( void ) const;
        ARLCORE_API void setcx( double );

        //! @return cy or v0 = y-coordinate of optical center
        ARLCORE_API double getcy( void ) const;
        ARLCORE_API void setcy( double );

        //! @return fx = x-coordinate of optical center
        ARLCORE_API double getfx( void ) const;
        ARLCORE_API void setfx( double );

        //! @return fy = y-coordinate of optical center
        ARLCORE_API double getfy( void ) const;
        ARLCORE_API void setfy( double );

        //! @return AlphaC or skew
        ARLCORE_API double getAlphaC( void ) const;
        ARLCORE_API void setAlphaC( double );

        //! @return Distortion coefficients [0;4]
        ARLCORE_API double getkc( unsigned int ) const;
        ARLCORE_API bool setkc( unsigned int, double );

        /**
        * @brief Set completly or partially Intrinsic parameters with v in this order :
        * fx,fy,cx,cy,k1,k2,p1,p2,k3,alphaC
        * @return Number of set parameters
        */
        ARLCORE_API unsigned int setIntrinsic( const vnl_vector<double> &v );

        //! @brief Getter for intermediate values
        ARLCORE_API const vnl_matrix_fixed<double,2,3>& getQint( void ) const;
        ARLCORE_API const vnl_matrix_fixed<double,2,3>& getQext( void ) const;
        ARLCORE_API const vnl_vector_fixed<double,3>& getCint( void ) const;
        ARLCORE_API const vnl_vector_fixed<double,3>& getCext( void ) const;
        ARLCORE_API const vnl_vector_fixed<double,2>& getBint( void ) const;
        ARLCORE_API const vnl_vector_fixed<double,2>& getBext( void ) const;

        /**
        * @brief 3D equation of the projective line in focal plane
        * @param[in] focalPt2DH Homogen 2D point in focal plane
        * @param[out] line3D 3D equation of the projective line
        * @return True if the computation is possible
        */
        ARLCORE_API bool projectiveLine( const vnl_vector_fixed<double,3> &focalPt2DH, vgl_line_3d_2_points <double> &line3D ) const;

        /**
        * @brief Reprojection du point3D dans le repère de la caméra
        * point2D est calculé dans le plan de la focale si focalPlane==true, dans le plan pixel sinon
        * @param[in] point3DH Point 3D à reprojeter
        * @param[out] point2D reprojeté
        * @param[in] focalPlane True si le calcul est effectué dans le plan de la focal ou false dans le plan pixel
        * @return True si le calcul est possible, false sinon (si le point2D est dans le plan de la focale par exemple)
        */
        ARLCORE_API bool project3DPoint( const vnl_vector_fixed<double,4>& point3DH, vnl_vector_fixed<double,2>& point2D, bool FocalPlane=false) const;

        /**
        * @brief Reprojection du point3D dans le repère de la caméra
        * point2D est calculé dans le plan de la focale si focalPlane==true, dans le plan pixel sinon
        * @param[in] point3DH Point 3D à reprojeter
        * @param[out] point2D reprojeté
        * @param[in] focalPlane True si le calcul est effectué dans le plan de la focal ou false dans le plan pixel
        * @return True si le calcul est possible, false sinon (si le point2D est dans le plan de la focale par exemple)
        */
        ARLCORE_API bool project3DPoint( const Point& pt3D, Point& pt2D, bool FocalPlane=false) const;

        /**
        * @brief Reprojection d'une liste de points 3D dans le repère de la caméra
        * Les points 2D sont calculés dans le plan de la focale si focalPlane==true, dans le plan pixel sinon
        * @param[in] list3D Liste de point 3D à reprojeter
        * @param[out] list2D Liste de points 2D reprojetés
        * @param[in] focalPlane True si le calcul est effectué dans le plan de la focal ou false dans le plan pixel
        * @return Nombre de points 2D correctement reprojetés
        */
        ARLCORE_API unsigned int project3DPoint( const PointList& list3D, PointList& list2D, bool FocalFrame=false) const;

        //! @brief Undistort 2D pixel point
        ARLCORE_API bool undistort2DPoint(const Point& p2D, Point& p2D_undistorted) const;

        //! @brief Set a 2D point from unitFocalPlane to pixelPlane
        ARLCORE_API bool unitFocalPlaneToPixelPlane( const vgl_point_2d<double>&, vgl_point_2d<double>& ) const;

        //! @brief Set a 2D point from  pixelPlane to unitFocalPlane with or without perfect distorsion evaluation
        ARLCORE_API bool pixelPlaneToUnitFocalPlane( const Point&, Point&, bool perfectDisto ) const;
        ARLCORE_API bool pixelPlaneToUnitFocalPlane( const Point&, vnl_vector_fixed<double,3>&, bool perfectDisto ) const;
        ARLCORE_API bool pixelPlaneToUnitFocalPlane( const Point&, vgl_point_2d<double>&, bool perfectDisto ) const;
        ARLCORE_API bool pixelPlaneToUnitFocalPlane( const Point&, vgl_point_3d<double>&, bool perfectDisto ) const;
        ARLCORE_API bool pixelPlaneToUnitFocalPlane( const vgl_point_2d<double>&, vgl_point_3d<double>&, bool perfectDisto ) const;

        //! @brief Set a 3D point from focalframe to extrinsicFrame
        ARLCORE_API bool focalFrameToExtrinsicFrame( const vnl_vector_fixed< double, 3 >&, vnl_vector_fixed< double, 3 >& ) const;
        ARLCORE_API bool focalFrameToExtrinsicFrame( const vgl_point_3d< double >&, vgl_point_3d< double >& ) const;

        /**
        * @brief Calcul 2 points définis dans le plan pixel à partir d'une droite dans le plan focal
        * @param[in] (a,b,c) l'equation d'une droite dans le plan focal, ax+by+c=0
        * @param[in] (ROI1,ROI2) dans le plan pixel, coin supérieur gauche et inférieur droit
        * @param[out] (ROI1,ROI2) dans le plan pixel, recoit les coordonnes des intersections avec la ROI
        */
        ARLCORE_API bool focalToPixelLine( double a, double b, double c, Point &ROI1, Point &ROI2 ) const;

        /**
        * @brief Calcul 2 points définis dans le plan pixel à partir d'une droite dans le plan focal
        * @param[in] (a,b,c) l'equation d'une droite dans le plan focal, ax+by+c=0
        * @param[in] org Origine dans le plan pixel
        * @param[in] dst Destination dans le plan pixel
        * @param[in] nbSegments [1,n] Nombre de segments constituant la droite. Si 0, évalue le nb max de segments pour la résolution
        * @param[out] pl Recoit la liste des points en coordonnées pixel constituant chaque segment
        */
        ARLCORE_API bool focalToPixelLine( double a, double b, double c, const Point &org, const Point &dst, PointList &pl, unsigned int nbSegments=0 ) const;

        /**
        * @brief 3D equation in focal plane of a pixel line
        * @param[in] line2D 2D line equation in pixel plane
        * @param[out] line3D 3D line equation in focal plane
        * @return True if the computation is possible
        */
        ARLCORE_API bool pixelToFocalLine( const vgl_line_2d<double> &line2D, vgl_line_3d_2_points <double> &line3D ) const;

    private:
        //! @brief Refresh precalculated data
        void extCompute( void );

        //! Universal ID for the physic's camera
        uint32HL m_cameraGUID;
        long int m_noCalibration;

        vnl_rigid_matrix m_extrinsic; //!< Extrinsic matrix
        vnl_matrix_fixed<double,3,3> m_intrinsicMatrix; //!< [fx 0 cx; 0 fy cy; 0 0 1]
        vnl_vector_fixed<double,5> m_distortionCoeffs; //!< [k1, k2, p1, p2, k3]
        double m_alphaC; //!< skew


        //! Precalculated data. Redondant informations
        vnl_matrix_fixed<double,2,3> m_Qint; //!< [ fx  0 cx ; 0 fy 0 ]
        vnl_matrix_fixed<double,2,3> m_Qext; //!< extracted from the full projective 3x4 matrix (intrinsic x extrinsic)
        vnl_vector_fixed<double,2> m_Bint;
        vnl_vector_fixed<double,3> m_Cint;
        vnl_vector_fixed<double,2> m_Bext;
        vnl_vector_fixed<double,3> m_Cext;
        vnl_rigid_matrix m_invExtrinsic; //!< Invert extrinsic matrix
        vnl_matrix_fixed<double,3,3> m_invIntrinsicMatrix; //!< Invert intrinsic matrix
        vnl_vector_fixed< double,3 > m_opticalCenterInExtrinsicFrame; //!< Position 3D du centre optique dans le repère extrinsèque
    };

    /**
    * @brief Compute epipolar line in focal plane
    * @param[in] p Point in the focal plane of the 1st camera
    * @param[in] PM Passage matrix from 1st camera to 2nd camera
    * PM=camera2->getExtrinsic()).as_matrix()*vnl_matrix_inverse<double>(camera1->getExtrinsic());
    * @return Equation of epipolar line in the focal plane of the 2nd camera, ax+by+c=0
    */
    ARLCORE_API bool getEpipolar(const Point &p, const vnl_matrix_fixed<double,4,4> &PM, double &a, double &b, double &c);
    ARLCORE_API bool getEpipolar(const Point &p, const vnl_matrix_fixed<double,4,4> &PM, vgl_line_2d< double > &d);
    ARLCORE_API bool getEpipolar(const vnl_vector_fixed<double,3> &p, const vnl_matrix_fixed<double,4,4> &PM, double &a, double &b, double &c);
    ARLCORE_API bool getEpipolar(const vnl_vector_fixed<double,3> &p, const vnl_matrix_fixed<double,4,4> &PM, vgl_line_2d< double > &d);

} // namespace arlCore
#endif // _ARLCORE_CAMERA_H

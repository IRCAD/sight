/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _ARLCORE_POINT_H
#define _ARLCORE_POINT_H
#include <arlcore/Common.h>

#include <vector>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_matrix.h>

#include <arlcore/Common.h>
#include <arlcore/Object.h>
#include <arlcore/Colour.h>
#include <arlcore/vnl_covariance_matrix.h>

namespace arlCore
{
    enum ARLCORE_POINT_SAVE_TYPE { ARLCORE_POINT_SAVE_FULL, ARLCORE_POINT_SAVE_LIGHT, ARLCORE_POINT_SAVE_GNUPLOT, ARLCORE_POINT_SAVE_VTK };

    enum ARLCORE_SHAPE { ARLCORE_SHAPE_UNKNOWN, ARLCORE_SHAPE_CUBE, ARLCORE_SHAPE_SPHERE, ARLCORE_SHAPE_SPHERESURFACE, ARLCORE_SHAPE_SPHERE_CAP, ARLCORE_SHAPE_DISC,
         ARLCORE_SHAPE_PLAINSQUARE, ARLCORE_SHAPE_CIRCLE, ARLCORE_SHAPE_EDGESQUARE, ARLCORE_SHAPE_SOLIDANGLE, ARLCORE_SHAPE_SOLIDANGLE_SURFACE, ARLCORE_SHAPE_CHESSBOARD, ARLCORE_SHAPE_NBSHAPES };
    static std::string ARLCORE_SHAPE_NAMES[ARLCORE_SHAPE_NBSHAPES]={ "ARLCORE_SHAPE_UNKNOWN", "ARLCORE_SHAPE_CUBE", "ARLCORE_SHAPE_SPHERE", "ARLCORE_SHAPE_SPHERESURFACE", "ARLCORE_SHAPE_SPHERE_CAP", "ARLCORE_SHAPE_DISC", "ARLCORE_SHAPE_PLAINSQUARE", "ARLCORE_SHAPE_CIRCLE",
         "ARLCORE_SHAPE_EDGESQUARE", "ARLCORE_SHAPE_SOLIDANGLE", "ARLCORE_SHAPE_SOLIDANGLE_SURFACE",", ARLCORE_SHAPE_CHESSBOARD"};

    class Point : public Object
    {
    /**
     * @class   Point
     * @author  IRCAD (Research and Development Team)
     * @date    2007
     * @brief   Point with a dimension that can be specified by the user. This class allows
     * to attribute several important properties to a point. Generally, these properties
     * are used in image processing algorithms:
     * - coordinates : corresponds to the N coordinates of the point
     * - type : corresponds to what the point physically represents in the world. This can be
     * a radio-opaque markers, a point detected from a structured light pattern, a point 
     * from an ARToolkit pattern...
     * - confidence : in the case you are detecting points in an image and you want to associate
     * a confidence value about its detection
     * - status : gives an information on how the point life when the software is running
     * - covariance matrix : you can associate a covariance matrix with to the point. This is 
     * very useful for registration alogrithm that takes into account the uncertainty on the point
     * detection/reconstruction 
     * - visibility : when you are tracking a chessboard for example, this boolean can help you
     * to indicate which point of the chessboard are visible or not. Then, you can select 
     * very easily the visible points only in your algorithms
     */
    public:
        typedef Point* sptr;
        typedef Point const * csptr;

        //! @brief Type of point (Origin)
        enum ARLCORE_POINT_TYPE { ARLCORE_POINT_TYPE_UNKNOWN, ARLCORE_POINT_TYPE_ARTK, ARLCORE_POINT_TYPE_MRO, ARLCORE_POINT_TYPE_CHESS, ARLCORE_POINT_TYPE_ROI, ARLCORE_POINT_TYPE_CLOUD, ARLCORE_POINT_TYPE_TIP, ARLCORE_POINT_TYPE_TOP, ARLCORE_POINT_TYPE_LINE, ARLCORE_POINT_TYPE_SIFT, ARLCORE_POINT_NBTYPES };

        //! @brief Status of the point (Evolution)
        enum ARLCORE_POINT_STATUS { ARLCORE_POINT_STATUS_UNKNOWN, ARLCORE_POINT_STATUS_DETECTED, ARLCORE_POINT_STATUS_SUBPIXEL, ARLCORE_POINT_STATUS_ESTIMATE, ARLCORE_POINT_STATUS_CLOUD, ARLCORE_POINT_REPROJECTION, ARLCORE_POINT_NBSTATUS };

        //! @brief Constructor
        ARLCORE_API Point( unsigned int dimension=3, ARLCORE_POINT_TYPE type=ARLCORE_POINT_TYPE_UNKNOWN, long int date=0, long int time=0 );

        //! @brief 2D point constructor
        ARLCORE_API Point( double x, double y, ARLCORE_POINT_TYPE type=ARLCORE_POINT_TYPE_UNKNOWN, long int date=0, long int time=0 );

        //! @brief 3D point constructor
        ARLCORE_API Point( double x, double y, double z, ARLCORE_POINT_TYPE type=ARLCORE_POINT_TYPE_UNKNOWN, long int date=0, long int time=0 );

        //! @brief Copy constructor
        ARLCORE_API Point( const Point& );

        /**
        * @brief Initialization : Set default value
        * To change the dimension, you have to reinitialize the point with the new dimension
        */
        ARLCORE_API void init( unsigned int dim );

        //! @brief Affectation
        ARLCORE_API Point& operator=(const Point&);

        //! @brief Copy p in the current point
        ARLCORE_API void copy( const Point& p );

        //! @brief Destructor
        ARLCORE_API ~Point( void );

        //! @return Description of the current point
        ARLCORE_API std::string getString( void ) const;

        //! @brief Save the current point
        ARLCORE_API bool save( const std::string &fileName, bool overwrite=true ) const;

        //! @brief Add the current point into an PointList open file stream
        ARLCORE_API bool save( std::fstream &f, unsigned no=0, ARLCORE_POINT_SAVE_TYPE=ARLCORE_POINT_SAVE_FULL ) const;

        //! @brief Add the current point into a SmartPointList open file stream
        ARLCORE_API bool save( std::fstream &f, unsigned int cam, void* tag, int fringe ) const;

        //! @brief Load a point
        ARLCORE_API bool load( const std::string &fileName );

        //! @brief Load a point from an open file stream
        ARLCORE_API bool load( std::ifstream &f, int &no, unsigned int dim=0 );
        
        //! @brief Load a point from an open SmartPointList file stream
        ARLCORE_API bool load( std::ifstream &f, unsigned int &cam, void* &tag, int &fringe );

        //! @return Is the point visible?
        ARLCORE_API bool isVisible() const;

        //! @return Is the point visible?
        ARLCORE_API bool getVisible() const;

        //! @brief Set the visibility of the point
        ARLCORE_API bool setVisible( bool );

        //! @return Status of the point for internal or special usage
        ARLCORE_API ARLCORE_POINT_STATUS getStatus() const;

        //! @brief Set the status of the point
        ARLCORE_API bool setStatus(ARLCORE_POINT_STATUS);

        //! @brief Set the colour of the point
        ARLCORE_API bool setColour(unsigned int r, unsigned int g, unsigned int b);

        //! @brief Set the colour of the point
        ARLCORE_API bool setColour(const Colour &c);

        //! @return The colour of the point
        ARLCORE_API bool getColour(unsigned int &r, unsigned int &g, unsigned int &b) const;

        //! @return The colour of the point
        ARLCORE_API bool getColour(const Colour &c) const;

        //! @return The colour of the point
        ARLCORE_API const Colour& getColour( void ) const;

        //! @return Is the point colored ?
        ARLCORE_API bool isColored() const;

        // Accesses coordinates of the point
        //! @return Value of the ième coordinate
        ARLCORE_API double get( unsigned int i ) const;

        //! @brief Set the ième coordinate with v
        ARLCORE_API bool set( unsigned int i, double v );

        //! @brief Set the current point with p if it has the same dimension
        ARLCORE_API bool set( const Point& p );

        //! @brief Fill all coordinates with v
        ARLCORE_API void fill( double v );

        /**
        * @brief TODO Modifie la valeur du point en l'équipondérant avec p et les précédentes pondérations
        * Pondérer n points à partir d'un point nul revient à calculer le point moyen
        * @remark Utiliser set pour réinitialiser une nouvelle valeur non pondérée
        */
        ARLCORE_API bool pond( const Point& p );

        /**
        * @return Reference on statistic vector for each coordinates when using pond function
        * @remark Use computeStat on each returned vector to extract statistic informations
        */
        ARLCORE_API const std::vector< vnl_vector_fixed<double,5> > &getStatistic( void );

        //! @brief Ajoute les coordonnées du point p au point courant
        ARLCORE_API bool add( const Point& p );

        //! @return Value of the i th coordinate [0,size-1]
        ARLCORE_API double operator[]( unsigned int i ) const;

        //! @return 1st coordinate
        ARLCORE_API double x( void ) const;

        //! @return 2nd coordinate (0 if point dimension is below 2)
        ARLCORE_API double y( void ) const;

        //! @return 3rd coordinate (0 if point dimension is below 3)
        ARLCORE_API double z( void ) const;

        //! @brief Modify the 1st coordinate
        ARLCORE_API bool x( double );

        //! @brief Modify the 2nd coordinate
        ARLCORE_API bool y( double );

        //! @brief Modify the 3rd coordinate
        ARLCORE_API bool z( double );

        //! @return Return coordinates under vnl format
        ARLCORE_API vnl_vector<double> getCoordinates() const;

        //! @return Return homogeneous coordinates under vnl format
        ARLCORE_API vnl_vector<double> getHCoordinates() const;

        ARLCORE_API vnl_covariance_matrix& getCovMatrix();
        ARLCORE_API const vnl_covariance_matrix& getCovMatrix() const;

        //! @return Dimension of the point
        ARLCORE_API unsigned int size( void ) const;

        //! @return Return point type
        ARLCORE_API ARLCORE_POINT_TYPE getType( void ) const;

        //! @return Affecte le type du point
        ARLCORE_API bool setType( ARLCORE_POINT_TYPE );

        //! @return Return scalar associated to the point
        ARLCORE_API double getScalar( void ) const;

        //! @brief Modify scalar associated to the point
        ARLCORE_API void setScalar( double );

        //! @brief Normalize the point as a vector
        ARLCORE_API void normalize( void );

        //! @return Distance between *this and p if the dimension are the same
        ARLCORE_API double distance( const Point& ) const;

        //! @return Squared distance between *this and p if the dimension are the same
        ARLCORE_API double distance2( const Point& ) const;

        //! @brief Multiply each point coordinate with a scalar 
        ARLCORE_API bool mult( double scalaire );

        //! @brief Compute the cross product of two 3D vectors and put the result in *this : *this = vec1 x vec2
        ARLCORE_API bool cross_3D( const Point& vec1, const Point& vec2 );

        //! @brief Dot product of 2 vectors : *this = vec1^T x vec2. *this is of dimension 1 ! (covariance matrix is computed as well)
        ARLCORE_API bool dotProduct( const Point& vec1, const Point& vec2 );

        //! @brief Point 1D = var1/var2 with computation of the covariance matrix
        ARLCORE_API bool divide( const Point& var1, const Point& var2 );

        //! @brief *this =  multiplication of a scalar (1D) with a vector vec (covariance matrix is computed as well)
        ARLCORE_API bool multiply( const Point& scalar, const Point& vec );

        // Stochastic and noised point
        //! @brief Add a gaussian random noise with a std deviation (std) on the corrdinate 'index' of Point
        ARLCORE_API bool addGaussianNoise( const unsigned int index, const double std );

        //! @brief Add a gaussian random noise with a std deviation (std) on all coordinates
        ARLCORE_API bool addGaussianNoise( const double std );

        /**
        * @brief Add a uniform random noise with a std deviation (std) on the corrdinate 'index' of Point
        * The random variable is taken in the interval [-radius ; +radius]
        */
        ARLCORE_API void addUniformNoise( const unsigned int index, const double radius );

        /**
        * @brief Random creation of a point in a specified shape which is centred on *this
        * For squared shape, the faces of the cube or the sides of the square are parallel to the axis
        * Most of the function work whatever dimension the point has
        * Planar shape have last cordinates to 0 (if dimension is above 3, then the point belongs to an hyperplan)
        * @param size = Diameter of the sphere/circle or  Side of the cube/square or max value of the solid angle
        * Below are indicated the possible shape:
        * ARLCORE_SHAPE_CUBE
        * ARLCORE_SHAPE_SPHERE
        * ARLCORE_SHAPE_DISC
        * ARLCORE_SHAPE_PLAINSQUARE
        * ARLCORE_SHAPE_CIRCLE
        * ARLCORE_SHAPE_EDGESQUARE
        * ARLCORE_SHAPE_SOLIDANGLE : a random point in a solid angle oriented along the positiv Z axis
        * According to the spherical coordinate system it chooses a point with r<size, 0< theta<2Pi,  0<phi<phi_max where 
        * phi_max = acos(1-omega/2/Pi). Here omega is the solid angle given in parameter of the function.
        * ARLCORE_SHAPE_SOLIDANGLE_SURFACE : same as before but the point is randomly chosen on the surface cone
        * corresponding to the solid angle
        * ARLCORE_SHAPE_SPHERESURFACE : a random point on the surface of a sphere of radius size
        * ARLCORE_SHAPE_SPHERE_CAP : a solid angle is defined along the positiv Z axis (the parameter
        * of the solid angle is "angle"). A random point is chosen on the sphere cap of the solid angle
        */
        ARLCORE_API bool shapeRandom( ARLCORE_SHAPE, const double size, const double angle=0 );

        ARLCORE_API void setError( double error );
        ARLCORE_API double getError( void ) const;

    private:
        //! @brief Add the current point into an open file stream (PointList or SmartPointList)
        ARLCORE_API bool save( std::fstream &f, bool ctf, unsigned int cam, void* tag, int fringe, unsigned no, ARLCORE_POINT_SAVE_TYPE type ) const;

        //! @brief Load a point from an open file stream (PointList or SmartPointList)
        ARLCORE_API bool load( std::ifstream &f, unsigned int &cam, void* &tag, int &fringe, int &no, unsigned int dim=0 );
        
        //! @brief Initialization to zero of the uncertainty matrix
        bool initUncertainty( void );

        ARLCORE_POINT_TYPE m_type;
        double m_confidence;
        bool m_visibility;
        ARLCORE_POINT_STATUS m_status;
        double m_scalar;

        bool m_isColored;
        Colour m_colour;

        vnl_vector<double> m_coordinates;

        double m_ponderation;
        std::vector< vnl_vector_fixed<double,5> > m_stat;

        // Erreur à la création du point
        // 2D : Différence subpixel / 1ere estimation
        // 3D : reconstruction
        double m_error;

        // Symmetric square matrix : anisotropic uncertainties
        vnl_covariance_matrix m_covMatrix;
    };

    //! @return Distance between pt1 and pt2
    ARLCORE_API double distance( const Point &pt1, const Point &pt2 );

    /**
    * @brief Si l est trié par y croissant, retourne l'indice du point de l dont l'ordonnée est la plus proche de y
    * @param[in] indexInit Indice d'initialisation ou -1 s'il est inconnu
    */
    ARLCORE_API unsigned int closestY ( const std::vector< arlCore::Point::csptr >& l, double y, unsigned int index1=0, unsigned int index2=0, int indexInit=-1 );

    /**
    * @return Surface formée par les points.
    * Très mal approximée par la somme des distances de tous les points
    */
    ARLCORE_API double surface( const std::vector< arlCore::Point::csptr >& );

    // TODO Suppress PointsCont and replace them with PointsList
//  typedef std::vector<Point> PointsCont;
//  ARLCORE_API unsigned int load( PointsCont &list, const char *fileName, Point::ARLCORE_POINT_TYPE type = Point::ARLCORE_POINT_TYPE_UNKNOWN, long int date=0 );

} // namespace arlCore
#endif // _ARLCORE_POINT_H

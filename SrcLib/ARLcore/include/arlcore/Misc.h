/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _ARLCORE_MISC_H
#define _ARLCORE_MISC_H
#include <arlcore/Common.h>

#include <string>
#include <vector>

#include <vnl/vnl_math.h>
#include <vnl/vnl_vector_fixed.h>

#include <vgl/vgl_line_3d_2_points.h>

#include <arlcore/Point.h>
#include <arlcore/Camera.h>

namespace arlCore
{
    //const double Pi = 3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170679;
    const double DegToRad = vnl_math::pi/180.0;
    const double RadToDeg = 180.0/vnl_math::pi;
    //! @return if n is even
    template <typename T> inline bool even( const T &n ){long int a=n/2; return (T)(a*2)==n;}

    //! @brief swap 2 variables
    template <typename T> inline void swap( T &a, T &b ){T c=a;a=b;b=c;}

    class PointList;
    /**
     * @brief Fill pointlist with a 3D square grid. The first point to be built has (0,0,0)
     * coordinates and the grid is filled firstly along X axis, then Y axis. For example, if 
     * nbPointsX = 3, squareSizeX = 2, nbPointsY = 2 and squareSizeX = 4, your pointList will be:
     * 0 0 0
     * 2 0 0
     * 4 0 0
     * 0 4 0
     * 2 4 0
     * 4 4 0
     * @param[out]  pl PointList with the grid
     * @param[in]   nbPointsX Number of points in x-axis
     * @param[in]   squareSizeX Distance between 2 points in x-axis
     * @param[in]   nbPointsY Number of points in y-axis
     * @param[in]   squareSizeY Distance between 2 points in y-axis
     * @param[in]   nbPointsZ Number of points in z-axis
     * @param[in]   squareSizeZ Distance between 2 points in z-axis
     * @return  Number of points of the grid
     **/
    ARLCORE_API unsigned int fillGrid( PointList &pl, unsigned int nbPointsX, double squareSizeX, unsigned int nbPointsY, double squareSizeY, unsigned int nbPointsZ=1, double squareSizeZ=0 );

    //! @brief Plot a serie of pointlists
    ARLCORE_API bool plot( const std::vector< const arlCore::PointList* >&, const std::string &options );

    class vnl_rigid_matrix;
    ARLCORE_API bool load( std::vector< vnl_rigid_matrix >&, const std::string &fileName, bool invert=false );
    ARLCORE_API bool save( const std::vector< arlCore::vnl_rigid_matrix >&, const std::string &fileName, bool overwrite=true );

    //! @brief Calibrate tip and orientation of a needle
    ARLCORE_API bool needleCalibration( const std::vector< arlCore::vnl_rigid_matrix > &tip, arlCore::vnl_rigid_matrix &calibration, const std::vector< arlCore::vnl_rigid_matrix > &orientation, double &RMS );

    ARLCORE_API unsigned int save( const std::vector<double>&, const std::string &filename, bool overwrite=true );

    //! @brief Save list of vnl_vector with their names
    ARLCORE_API unsigned int save( const std::vector< vnl_vector<double> >& q, vnl_vector<double> ratio, const std::vector< std::string > &names, const std::string &fileName, bool overwrite=true );
    ARLCORE_API unsigned int save( const std::vector< vnl_vector<double> >& q, const std::string &filename, bool overwrite=true );

    //! @brief Load list of vectors : FileType=Vectors, Tokens={Vector, Ratio}
    ARLCORE_API unsigned int load( std::vector< vnl_vector<double> >&, const std::string &filename );
    ARLCORE_API unsigned int load( std::vector< vnl_vector<double> >&, std::vector<std::string>&names, const std::string &filename );

    //! @brief Load list of filenames : FileType=Filenames, Tokens={Name, Filename}
    ARLCORE_API unsigned int load( std::vector< std::string >&, const std::string &filename );

    //! @brief Save/Load vnl_matrix : FileType=Matrix, Tokens={Matrix}
    ARLCORE_API bool save( const vnl_matrix<double>&, const std::string &filename, bool overwrite=true );
    ARLCORE_API bool load( vnl_matrix<double>&, const std::string &filename );

    /**
     * @brief Compute RMS, mean & standard deviation from a list of double values
     * @param[in] values List of values
     * @param[in] square Set true if values are squared values
     * @param[out] min
     * @param[out] max
     * @param[out] rms Root Mean Square
     * @param[out] mean
     * @param[out] stddev Standard Deviation
     * @return Number of values
     **/
    ARLCORE_API unsigned int statistic( const std::vector<double>&values, double &min, double &max, double &rms, double &mean, double &stddev, bool square=false );

    /**
     * @brief Integrate a new error in a vector of 5 values
     * @param[in/out] stat At beginning, should be fill with zeros
     * @param[in] value Value to be added
     * @param[in] square True if the value is a squared value
     * @return Value
     * @remark Use computeStat to extract statistic values
     **/
    ARLCORE_API double addValue( vnl_vector_fixed<double,5>& stat, double value, bool square=false );

    /**
     * @brief Compute statistic values from integrate stat vector
     * @param[in] stat Contains integrate values of a statistic list
     * @param[out] mean Mean value of the list 
     * @param[out] stdDev Standard deviation of the list
     * @param[out] RMS Root mean square of the list
     * @return Number of values used
     **/
    ARLCORE_API unsigned int computeStat( const vnl_vector_fixed<double,5>& stat, double &min, double &max, double &mean, double &stdDev, double &RMS );

    //! @return Number of parameters of a polynomial (ie: 3 equations, degree 4 => 105 parameters)
    ARLCORE_API unsigned int nbPolynomialParameters( unsigned int degree, unsigned int nbEquations );

    //! @brief Compute result = Polynomial(data) with parameters of polynomial
    ARLCORE_API bool computePolynomial( unsigned int degree, const vnl_vector<double> &parameters, const vnl_vector<double> &data, vnl_vector<double> &result );

    //! @return Distance entre un point et une droite dans l'espace
    ARLCORE_API double vgl_distance_point( const vgl_line_3d_2_points<double> &l, const vgl_point_3d<double> &p );

    //! @return vgl_line_2d from slope (angle with horizontal) and rho (distance to origin)
    ARLCORE_API vgl_line_2d<double> vglLine2d( double slope, double rho );

    /**
     * @brief Fit a 3D line from a list of 3D points
     * @param[out] RMS of the distance of points to the line
     **/
    ARLCORE_API bool vgl_fit_line_3d( const std::vector< vgl_point_3d<double> > &points, vgl_line_3d_2_points<double> &line, double &RMS );

    //! @brief One-parameter family of unit vectors that are orthogonal to vec, s=[0,1[
    ARLCORE_API vgl_vector_3d<double> orthogonal_vectors( const vgl_vector_3d<double> &vec, double s);

    //! @return if two bounding box are intersected
    ARLCORE_API bool intersection( const vnl_vector_fixed<double, 6> &bound1, const vnl_vector_fixed<double, 6> &bound2 );

    /**
     * @brief From tip, top & width of a needle (pix.), return the list of points like a shape
     * @param[in] shape 0=Quadrilateral (Tip R&L, Top L&R), 1=Triangular (Tip R&L, Top )
     **/
    ARLCORE_API unsigned int computeNeedleROI( const arlCore::Point &tip, const arlCore::Point &top, unsigned int width, std::vector<arlCore::Point> &corners, unsigned int shape=1 );

    //! @brief Compute Video1Robot transformation. Return mean distance
//  ARLCORE_API double computeVideo1Robot( const std::vector< arlCore::vnl_rigid_matrix > &M06, const PointList &tip, vnl_rigid_matrix &T, double &distance, double &stdDev );

    //! @brief Compute Video2Robot transformation. Return X & RMS in pixels
    ARLCORE_API double computeVideo2RobotX(
        const std::vector< arlCore::vnl_rigid_matrix > &M06,
        const arlCore::vnl_rigid_matrix &Z,
        const std::vector< std::vector< arlCore::PointList > > &model3D,
        const std::vector<const arlCore::Camera*> &cams,
        const std::vector< std::vector< arlCore::PointList > > &points2DList,
        arlCore::vnl_rigid_matrix &X);

    //! @brief Compute Video2Robot transformation. Return Z & RMS in pixels
    ARLCORE_API double computeVideo2RobotZ(
        const std::vector< arlCore::vnl_rigid_matrix > &M06,
        const arlCore::vnl_rigid_matrix &X,
        const std::vector< std::vector< arlCore::PointList > > &model3D,
        const std::vector<const arlCore::Camera*> &cams,
        const std::vector< std::vector< arlCore::PointList > > &points2DList,
        arlCore::vnl_rigid_matrix &Z);

    //! 
    /**
     * @brief Compute Chessboard2EndEffector & Video2Robot transformations
     * @return X, Z & RMS in pixels
     **/
    ARLCORE_API double computeVideo2RobotXZ(
        const std::vector< arlCore::vnl_rigid_matrix > &M06,
        const std::vector< std::vector< arlCore::PointList > > &model3D,
        const std::vector<const arlCore::Camera*> &cams,
        const std::vector< std::vector< arlCore::PointList > > &points2DList,
        arlCore::vnl_rigid_matrix &X,
        arlCore::vnl_rigid_matrix &Z);

    /**
     * @brief Evaluate the error of reprojection with the pair of transformation X,Z from solve_AXXB
     * Set X & Y matrix with the RMS and the Standard deviation of reprojection error
     * @param[in,out] X Transformation from Robot plane to Tag plane
     * @param[in,out] Z Transformation from Video plane to Robot plane
     * @param[in] B Transformation list from Robot base to End effector
     * @param[in] videoCams References on cameras used for computing Video-Tag transformations
     * @param[in] models3DList 3D models (ie chessboard) for each pose
     * @param[in] points2DList 2D detected points for each camera & each pose
     * @param[out] errors List of reprojection errors (Use statistic to compute mean stddev,...)
     * @return RMS of reprojection errors
     **/
    ARLCORE_API double evaluationAXXB(
        const arlCore::vnl_rigid_matrix &X,
        const arlCore::vnl_rigid_matrix &Z,
        const std::vector< vnl_rigid_matrix > &B,
        const std::vector< const arlCore::Camera* > &videoCams,
        const std::vector< std::vector<arlCore::PointList> > &models3DList, //[Nbcams][NbPoses]
        const std::vector< std::vector<arlCore::PointList> > &points2DList, //[Nbcams][NbPoses]
        std::vector< double > &errors);

} // namespace arlCore

namespace arlRandom
{
    class Random
    {
    /**
     * @class   Random
     * @author  IRCAD (Research and Development Team)
     * @date    2007, 11/2008
     * @brief   Randomize values
     * @remark  While no initRandom has launched, random values are generate from random seeds
     */
    public:
        //! @brief Initialize random generation from a file (only max values or all). Return 0 or seed if fail
        ARLCORE_API static long int initRandom( const std::string &filename, long int max=-1 );

        //! @brief Initialize random generation from a seed. If seed==0 return random seed else return seed
        ARLCORE_API static long int initRandom( long int seed=0 );

        //! @return Random double number between [min,max]
        ARLCORE_API static double uniformDoubleRnd( double min=0.0, double max=1.0 );

        //! @return Random integer number between [min,max]
        ARLCORE_API static int uniformIntRnd( int min, int max ) ;

        //! @return Random number : Gaussian law centered with a standard deviation 'std'
        ARLCORE_API static double gaussianRnd( double std );

        //! @brief Generate a file of n random numbers from a seed
        ARLCORE_API static unsigned int generate( long int seed, const std::string &filename, long int n );

    private:
        static std::vector< double> m_randomTable;
        static long int m_index;

        //! @return Random value
        static double privateRand( void );

        //! @brief Initialization of random functions with a seed (if 0 random seed and return the seed)
        static long int privateRndInit( long int seed=0 );

        //! @return Random double number between [min,max]
        static double privateUniformDoubleRnd( double min=0.0, double max=1.0 );

        //! @return Random integer number between [min,max]
        static int privateUniformIntRnd( int min, int max );

        //! @return Random number : Gaussian law centered with a standard deviation 'std'
        static double privateGaussianRnd( double std );
    };

    /**
     * @brief Create a vector of random index included in [0 ; n-1]
     * @param[in]   n Number of index
     * @param[in]   m Shake only m index among n or if zero shake all
     * @param[out]  v Vector of index included in [0 ; n-1]
     *
     * @return  Number of randomized index
     * @remark Only m index are randomized among the n index
     **/
    ARLCORE_API unsigned int randomIndex( unsigned int n, std::vector<unsigned int> &v, unsigned int m=0 );
} // namespace arlRandom

namespace arlTime
{
    /**
     * @author  IRCAD (Research and Development Team)
     * @date    2008
     * @brief   Time functions
     */

    //! @return Value of now date
    ARLCORE_API long int getNowDate( void );

    //! @return Value of now time
    ARLCORE_API long int getNowTime( void );

    //! @brief Sleep during time in milliseconds
    ARLCORE_API bool sleep( double time );

} // namespace arlTime

namespace arlString
{
    /**
     * @author  IRCAD (Research and Development Team)
     * @date    2007
     * @brief   Fonctions de traitements de chaines
     */

    //! @brief Replace character org by dst in string s & return number of replaced characters
    ARLCORE_API unsigned int replace( std::string &s, char org, char dst );

    /**
     * @brief Convert a string in an another type
     * @param[in] s String to convert
     * @param[out] obj Object of type int or double
     * @return  true if the conversion has been a success.
     **/
    ARLCORE_API bool valueOf(const std::string &s, int &obj);
    ARLCORE_API bool valueOf(const std::string &s, unsigned int &obj);
    ARLCORE_API bool valueOf(const std::string &s, double &obj);

    ARLCORE_API unsigned int extractDoubles( const std::string &s, std::vector<double> &v, char separator=';' );
    ARLCORE_API unsigned int extractInts( const std::string &s, std::vector<int> &v, char separator=';' );
    ARLCORE_API unsigned int extractUInts( const std::string &s, std::vector<unsigned int> &v, char separator=';' );
    ARLCORE_API unsigned int extractBools( const std::string &s, std::vector<bool> &v, char separator=';' );
    ARLCORE_API bool extractIP( const std::string &s, unsigned int &IP1, unsigned int &IP2, unsigned int &IP3, unsigned int &IP4 );
    ARLCORE_API bool extractIP( const std::string &s, std::string &IP, unsigned int &port );
    ARLCORE_API bool extractExt( const std::string &s, std::string &ext );

    /**
     * @brief   Split a full path in a folder name, a filename and an extension name
     * @param[in]   path The full path+file name
     * @param[out]  folder Only folder name
     * @param[out]  file Only filename
     * @param[out]  ext Extension of the filename
     *
     * @return  true if the process has been a success.
     **/
    ARLCORE_API bool splitFilename( const std::string &path, std::string &folder, std::string &file, std::string &ext );

    /**
     * @brief   Set the string with the date as i.e. 2008-10-27-15-32-22 for October 27 2008 at 15h 32mn 22s
     * @param[out]  s Will be set with full date string
     * @param[in]   date
     * @param[in]   separator Character between each number (ie : ' ' or '-')
     **/
    ARLCORE_API void getFullDateString( long int date, std::string &s, char separator );

    /**
     * @brief   Set the string with the date as i.e. 2008-10-27 for October 27 2008
     * @param[out]  s Will be set with full date string
     * @param[in]   date
     * @param[in]   separator Character between each number (ie : ' ' or '-')
     **/
    ARLCORE_API void getDateString( long int date, std::string &s, char separator );

        /**
     * @brief   Set the string with the time as i.e. 15-32-22 for 15h 32mn 22s
     * @param[out]  s Will be set with time string
     * @param[in]   date
     * @param[in]   separator Character between each number (ie : ' ' or '-')
     **/
    ARLCORE_API void getTimeString( long int time, std::string &s, char separator );

} // namespace arlString

namespace arlFile
{
    /**
     * @author  IRCAD (Research and Development Team)
     * @date    2007
     * @brief   Fonctions sur les fichiers
     */

    //! @return Does the file f exist ?
    ARLCORE_API bool fileExist( const std::string &f );

    //! @brief Create a directory on the filesystem
    ARLCORE_API bool mkdir( const std::string & );

    //! @brief Copy OriginFilename to DestinationFilename
    ARLCORE_API bool fileCopy( const std::string &OriginFilename, const std::string &DestinationFilename );

    //! @brief Convert .trian file to .vtk file
    ARLCORE_API bool convertTrianToVtk( const std::string &trianFilename, const std::string &vtkFilename );

} // namespace arlFile

namespace arlSound
{
    /**
     * @author  IRCAD (Research and Development Team)
     * @date    11/2008
     * @brief   Sound functions
     */
    const double NOTE_A = 880.00; // LA
    const double NOTE_B = 987.77; // SI
    const double NOTE_C = 523.25; // DO
    const double NOTE_D = 587.33; // RE
    const double NOTE_E = 659.26; // MI
    const double NOTE_F = 698.46; // FA
    const double NOTE_G = 783.99; // SOL

    //! @brief Play frequency during a time in ms
    ARLCORE_API bool play( double frequency=NOTE_A, double time=100 );

} // namespace arlSound
#endif // _ARLCORE_MISC_H

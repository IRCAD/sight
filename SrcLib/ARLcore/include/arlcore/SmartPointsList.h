/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _ARLCORE_SMARTPOINTLIST_H
#define _ARLCORE_SMARTPOINTLIST_H
#include <arlcore/Common.h>

#include <vector>
#include <map>

#include <arlcore/Point.h>
#include <arlcore/PointsList.h>
#include <arlcore/Camera.h>

namespace arlCore
{
    /**
     * @author  IRCAD (Research and Development Team)
     * @date    2007
     * @brief
     *  
     * In a video image you may extract information of different nature.
     * The purpose of this class is to contain the 2D coordinates of points detected/extracted
     * from all video images and to associate each point to their origin: a point can belong
     * to a point set representing an optical marker, a structured light fringe...
     * In this class, a point set representing an object is called videoTag.
     * 
     * When you are tracking several videoTag in a scene, you may want to get all the points
     * of a particular videoTag in the camera number i. This class allows you to get efficiently
     * this information. You can also ask for the reprojection in all cameras of a 
     * specific 3D point, or the reprojection of all videoTag in one (or all) camera...
     * 
     * Briefly, this class provides efficient methods to get 2D points with respect to 
     * a videoTag, a camera, a 3D point, a fringe.
     * 
     * Note that when you use structured light, a videoTag will contain the fringe number
     * information as well. This means that you can ask the point list of a specific fringe
     * only.
     * 
     * French comments:
     * Liste de points 2D ordonnés par camera, videoTag et numéro de franges
     * Cette classe permet de stocker des points 2D associés à une camera et un videoTag
     * Dans le cas de VideoTag de lumière structurée, le point est également associé à un no de frange
     * SmartPointList permet d'extraire efficacement des listes de points selon différents filtres
     * et ordonnancements.
     * Elle est notamment utilisée pour stocker les points détectés dans les images video
     */
    class SmartPointList
    {
    public:
        //! @brief Default constructor of an empty list
        ARLCORE_API SmartPointList( void );

        //! @brief Copy constructor
        ARLCORE_API SmartPointList( const SmartPointList& );

        //! @brief Affectation
        ARLCORE_API SmartPointList& operator=( const SmartPointList& );

        //! @brief Copy of smartpointlist in the current list
        ARLCORE_API void copy( const SmartPointList& spl );

        //! @brief Destructor of the list and all points
        ARLCORE_API ~SmartPointList( void );

        //! @return Provide the coordinates of each point for each camera
        ARLCORE_API std::string getString( void ) const;

        //! @brief Used by the developer only. Check that the object is not corrupted by point of bad dimension
        ARLCORE_API bool integrity( void ) const;

        //! @brief Save the smartpointlist in a file
        ARLCORE_API bool save( const std::string &fileName, bool overwrite=true ) const;

        //! @brief Load a smartpointlist from a file
        ARLCORE_API bool load( const std::string &fileName );

        /**
         * @brief Add a point in the list, tagged with a cam number, videoTag pointer and number of fringe
         * VideoTag pointer and fringe number are optional
         * @return Point's pointer in the SmartPointList
         */
        ARLCORE_API Point::csptr push_back( const Point& pt, unsigned int cam, void* videoTag=0, int fringe=0 );

        /**
         * @brief Add a pointlist, tagged with a cam number, videoTag pointer and number of fringe
         * VideoTag pointer and fringe number are optional
         */
        ARLCORE_API unsigned int push_back( const std::vector< arlCore::Point::csptr >&, unsigned int cam, void* videoTag=0, int fringe=0 );

        /**
         * @brief Add a point in the list, tagged with a cam number, videoTag pointer and number of fringe
         * VideoTag pointer and fringe number are optional
         */
        ARLCORE_API bool addPoint( const Point&, unsigned int cam, void* videoTag, unsigned int tagNo );

        /**
         * @return Reference to a list of points tagged with a cam number, videoTag pointer and number of fringe
         * VideoTag pointer and fringe number are optional
         */
        ARLCORE_API const std::vector< Point::csptr >& getList( unsigned int cam, void* videoTag=0, int fringe=0 ) const;

        /**
         * @brief : If radius<0, find in the SmartPointList for the camera 'cam', the nearest point of (x,y)
         * If radius>=0, find all points inside a circle centred in (x,y)
         * @param found = References to points in the defined area
         * @param distance = Distance to the nearest point
         * @return Number of found points
         * This function does not optimize the research method. Its complexity follows the number of point.
         */
        ARLCORE_API unsigned int findNearPoint(double x, double y, unsigned int cam, std::vector< Point::csptr > &found, double &distance, double radius=-1.0) const;

        /**
         * @brief 2D visible point for each camera corresponding to the 3D point number No of the videotag in several cameras
         * @param[in] cams Cameras list
         * @param[out] whichCams = Receive true for each camera when the point exists in
         * @param[out] pl = List with only found points
         * @return Number of found points
         */
        ARLCORE_API unsigned int getPointByCam( std::vector<Point::csptr>& pl, const std::vector<const arlCore::Camera*> &cams, void* videoTag, unsigned int no, std::vector< bool >&whichCams) const;
        ARLCORE_API unsigned int getPointByCam( std::vector<Point::csptr>& pl, const std::vector<arlCore::Camera> &cams, void* videoTag, unsigned int no, std::vector< bool >&whichCams) const;
        /**
         * @brief List of 2D visible points for a cam and optionally the videoTag pointer
         * When videoTag is not given, provide the point coordinates for all videoTag
         * @return Number of points returned
         */
        ARLCORE_API unsigned int getPoints( std::vector<Point::csptr>& pl, unsigned int cam, void* videoTag=0 ) const;

        /**
         * @brief List of 2D points for a cam and optionally the videoTag pointer
         * @return Number of points returned
         */
        ARLCORE_API unsigned int getPoints( PointList&, unsigned int cam, void* videoTag=0 ) const;
        ARLCORE_API unsigned int getInvisiblePoints( std::vector<Point::csptr>&, unsigned int cam, void* t=0 ) const;

        //! @brief Visible 2D point in the camera 'cam' which is the TagId point in VideoTag t 
        ARLCORE_API bool getPoint( Point::csptr&, unsigned int cam, void* t, unsigned int TagId ) const;
        
        //! @brief Clear the list and all points
        ARLCORE_API bool clear( void );

        //! @brief Number of points in the list tagged with the triplet (cam,videotag,fringe)
        ARLCORE_API unsigned int size( unsigned int cam, void* videoTag=0, int fringe=0 ) const;

    protected:
        //! @return A list among m_listOfLists or m_listsByCam in function of the triplet
        std::vector< Point::csptr >& privateGetList( unsigned int cam, void* videoTag=0, int fringe=0 );

        //! @return Hash code for the triplet : hashcode = hash(cam, videoTag, fringe)
        double hash( unsigned int cam, void* videoTag, int fringe=0 ) const;

        //! @return the triplet for a hashcode :  (cam, videoTag, fringe) = unhash(hashcode)
        bool unhash( double key, unsigned int &cam, void* &tag, int &fringe ) const;


    private:
        struct mapOrder
        {
            bool operator()(double d1, double d2) const
            {return d1<d2;}
        };
        /**
         * @brief Map of pairs ( hashcode , index in m_listOfLists )
         * if a list with the triplet (cam, videotag, fringe) exists
         * m_mapOfLists contains its index in m_listOfLists
         */
        std::map< double, unsigned int, mapOrder > m_mapOfLists;

        /**
         * @brief Contain a list of lists of points according to triplet association in m_mapOfLists
         * The first element is always an empty list
         */
        std::vector< std::vector< Point::csptr > > m_listOfLists;

        //! @brief Each element of the vector contained the complete list of points for a camera
        std::vector< std::vector< Point::csptr > > m_listsByCam;
    };

    /**
    * @brief Calcule les appariements epipolaires pour n cameras
    * @param[in] cameras Liste des cameras
    * @param[in] points2D Listes des points2D pour chaque camera
    * @param[out] matching Liste des appariements. Contient une liste de pointeurs sur les points 2D de chaque camera. En cas de non appariement le pointeur est nul.
    * @param[in] gaussianNoise Bruit moyen en pixels sur les points 2D détectés ou cliqués
    * @return Nombre d'appariements
    */
    ARLCORE_API unsigned int epipolarMatching( const std::vector<Camera>& cameras, const SmartPointList &points2D, std::vector< std::vector<Point::csptr> >&matching, double gaussianNoise );

/*  class SynchronizedList
    {
    public:

        // Create an empty point
        ARLCORE_API SynchronizedList( void );
        ARLCORE_API ~SynchronizedList( void );

        ARLCORE_API unsigned int size( void ) const;
        ARLCORE_API unsigned int addList( const PointsCont &list );
        ARLCORE_API long int operator []( unsigned int i ) const;
        ARLCORE_API bool getLists( MultiPointsCont* &lists );

        ARLCORE_API bool print( void ) const;

    private:
        void destroy( void );
        unsigned int erase( long int time);

        std::vector<long int> m_timeStamps;
        MultiPointsCont m_lists;
    };*/
} // namespace arlCore
#endif // _ARLCORE_SmartPointList_H

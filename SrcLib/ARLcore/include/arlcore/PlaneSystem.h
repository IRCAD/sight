/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _ARLCORE_PLANE_SYSTEM_H
#define _ARLCORE_PLANE_SYSTEM_H
#include <arlcore/Common.h>

#include <vector>
#include <string>

#include <arlcore/Object.h>
#include <arlcore/MatrixR.h>

namespace arlCore
{
    class PlaneSystem : public Object
    {
    /**
     * @class   PlaneSystem
     * @author  IRCAD (Research and Development Team)
     * @date    2007
     * @brief   Ensemble de reperes cartesiens lies par des transformations rigides
     */
    public:
        //! Constant Used to reference unknown plane
        ARLCORE_API static const unsigned int NoPlane;

        //! @brief Create an empty PlaneSystem
        ARLCORE_API PlaneSystem( const std::string &name="" );

        //! @brief Destructor
        ARLCORE_API ~PlaneSystem( void );

        //! @return Description of the current plane system
        ARLCORE_API std::string getString( void ) const;

        //! @brief Save the planeSystem : Not yet implemented
        ARLCORE_API bool save( const std::string &fileName, bool overwrite=true ) const;

        //! @brief Load a planeSystem : Not yet implemented
        ARLCORE_API bool load( const std::string &fileName );

        //! @brief Save graph of planeSystem for a graphviz visualization
        ARLCORE_API unsigned int saveGraph( const std::string& ) const;

        //! @brief Run a graphviz visualization
        ARLCORE_API unsigned int printGraph( void ) const;

        //! @brief Add the 3D plane in the PlaneSystem and return its ID [1,..[
        ARLCORE_API unsigned int add3DPlane( const Object &o );
        ARLCORE_API unsigned int add3DPlane( const std::string &s="" );
        ARLCORE_API bool setPlaneName( unsigned int plane, const std::string &s );

        //! @brief Release an anused plane in the graph
        ARLCORE_API bool releasePlane( unsigned int plane );

        //! @return The number of 3D planes in the PlaneSystem
        ARLCORE_API unsigned int getNbPlanes( void ) const;

        //! @return status of a plane number
        ARLCORE_API bool getPlaneStatus( unsigned int plane ) const;

        //! @return The ID of the 3D plane with the name [1,..[
        ARLCORE_API bool getPlaneID( std::string name, unsigned int &ID ) const;

        //! @return The name of the 3D plane  with the ID [1,..[
        ARLCORE_API bool getPlaneName( unsigned int ID, std::string &name ) const;

        //! @return Weight of the transformation at index
        ARLCORE_API double getWeight( unsigned int index ) const;

        //! @return Weight of the transformation (or inverse)
        ARLCORE_API double getWeight( unsigned int plane1, unsigned int plane2 ) const;

        /**
        * @brief If it exists, set T with the transformation from Plane1 to Plane2
        * @remark The age of the transformation is these of the older transformation in the path
        */
        ARLCORE_API bool getTrf( unsigned int Plane1, unsigned int Plane2, vnl_rigid_matrix &T, bool verbose=false );

        /**
        * @brief If it exists, set T with the transformation from Plane1 to Plane2
        * If ageT is the age of the transformation and ageU, the age of the universe
        * ageU-ageT should be lesser or equal than ageTolerance else return false
        * @param[in] ageTolerance in milliseconds (ms). If <0.0, equivalent to infinity
        * @remark If ageTolerance = 0, getTrf returns true only if the age of the transformation is exactly these of the planesystem
        */
        ARLCORE_API bool getTrf( unsigned int Plane1, unsigned int Plane2, vnl_rigid_matrix &T, double ageTolerance, bool verbose=false );

        /**
        * @brief If it exists, set T with the transformation from Plane1 to Plane2
        * If ageT is the age of the transformation and ageR, the age composed with date and time
        * ageR-ageT should be lesser or equal than ageTolerance
        * @param[in] ageTolerance in milliseconds (ms)
        * @remark If ageTolerance = 0, getTrf returns true only if the age of the transformation is exactly (date,time)
        */
        ARLCORE_API bool getTrf( unsigned int Plane1, unsigned int Plane2, vnl_rigid_matrix &T, long int date, long int time, double ageTolerance=0, bool verbose=false );

        //! @brief Set the transformation from Plane1 and Plane2
        ARLCORE_API bool setTrf( unsigned int plane1, unsigned int plane2, const vnl_rigid_matrix& T );

        //! @brief Set and date the transformation from Plane1 and Plane2
        ARLCORE_API bool setTrf( unsigned int Plane1, unsigned int Plane2, const vnl_rigid_matrix& T, long int date, long int time );

        //! @brief Set to identity  and date the transformation from Plane1 and Plane2
        ARLCORE_API bool setIdentity( unsigned int Plane1, unsigned int Plane2, const long int date=0, const long int time=0 );

        //! @brief Reset the transformation from Plane1 and Plane2 and its inverse
        ARLCORE_API bool resetTrf( unsigned int Plane1, unsigned int Plane2 );

        //! @brief Reset all transformation around Plane
        ARLCORE_API bool resetTrf( unsigned int Plane );

        //! @return Distance between origins of Plane1 and Plane2, if the transformation exists
        ARLCORE_API bool distance( unsigned int Plane1, unsigned int Plane2, double &dist/*, long int &date, long int &time*/ );

        //! @brief Set pt2 with the coordinates in Plane2 of pt1 set in Plane1
        ARLCORE_API bool chgPlane( unsigned int Plane1, const Point &pt1, unsigned int Plane2, Point &pt2);

        //! @brief Set pt with the origin of Plane1 in the Plane2
        ARLCORE_API bool getOrigin( unsigned int Plane1, unsigned int Plane2, Point &pt);

    private:
        class Path
        {
        /**
        * @class    Path
        * @author   IRCAD (Research and Development Team)
        * @date     04/2008
        * @brief    Chemin dans un planesystem
        */
        public:
            //! @brief Constructor
            ARLCORE_API Path( const PlaneSystem& );

            //! @brief Default constructor
            ARLCORE_API Path( const Path& );

            //! @brief Destructor
            ARLCORE_API ~Path( void );

            //! @return ith plane of the path else 0
            ARLCORE_API unsigned int operator[]( unsigned int i ) const;

            //! @brief Print the path
            ARLCORE_API void print( void ) const;

            //! @brief Number of planes in the path
            ARLCORE_API unsigned int size( void ) const;

            //! @brief Weight of the path (Sum of the Transformation's weight)
            ARLCORE_API double weight( void ) const;

            //! @brief Add a new plane and return the length
            ARLCORE_API unsigned int push_back( unsigned int plane );

            //! @brief Remove last plane and return if it was possible
            ARLCORE_API bool pop_back( void );

        private:
            const PlaneSystem& m_universe;
            std::vector< unsigned int > m_planes;
            std::vector< double > m_weights;
            double m_weight;
        };

        /**
        * @brief States for each transformation
        * STATE_UNDEFINED   : Transformation is undefined
        * STATE_IDENTITY    : Identity
        * STATE_CALIBRATION : Transformation is set by a user throw setTrf() at time(0,0)
        * STATE_SET         : Transformation is set by a user throw setTrf() at a real time
        * STATE_COMPUTED    : Transformation is deducted by the planesystem
        */
        enum ARL_PLANE_STATE { STATE_UNDEFINED, STATE_IDENTITY, STATE_CALIBRATION, STATE_SET, STATE_COMPUTED, NBSTATES };

        /**
        * @brief Mark the transformation in the direction Plane1 to Plane2
        * @remark Tags used to mark the transformations which are in a path
        * to solve a problem of cyclic path
        */
        void tag( unsigned int Plane1, unsigned int Plane2 );
        
        //! @brief Untag the transformations Plane1 to Plane2
        void untag( unsigned int Plane1, unsigned int Plane2 );

        //! @return True if the transformation at index is tagged
        bool isTagged( unsigned int index ) const;

        //! @return True if the transformation Plane1 to Plane2 is tagged
        bool isTagged( unsigned int Plane1, unsigned int Plane2 ) const;
        
        //! @brief Untag all transformations of the universe
        void untagAll( void );

        //! @return untagged status at the index
        ARL_PLANE_STATE getStatus( unsigned int index ) const;

        //! @return untagged status at the index
        ARL_PLANE_STATE getStatus( unsigned int Plane1, unsigned int Plane2 ) const;

        //! @return True if Plane1 & 2 exist in the universe
        bool outOfRange( unsigned int Plane1, unsigned int Plane2 ) const;
        
        //! @return True if Plane exists in the universe
        bool outOfRange( unsigned int Plane ) const;
        
        //! @return The index in the table for the transformations from Plane1 to Plane2
        unsigned int getIndex( unsigned int Plane1, unsigned int Plane2 ) const;

        //! @return The index for the inverse transformation in the table
        unsigned int getInvIndex( unsigned int index ) const;

        /**
        * @brief Find the path between Plane1 & Plane2 : Path get the list of all intermediate planes
        * @return True if the path is found else false
        * @remark Only transformations with correct date are considered except if date-time==0
        * In this case, all valid transformations are considered
        */
        //bool findPath( unsigned int Plane1, unsigned int Plane2, std::vector< unsigned int > &path, long int date, long int time);
        bool findPath( unsigned int Plane1, unsigned int Plane2, Path &path, long int date, long int time);

        //! @brief NoPlaneDestination with its weight (for whoIsConnected)
        typedef std::pair<unsigned int, double> PlaneWeight;

        /**
        * @brief Find all close neighbour planes of Plane which have a valid date-time
        * @return The number of neigbour
        */
        unsigned int whoIsConnected( unsigned int connectedAt, unsigned int finalDestination, std::vector<PlaneWeight> &Planes, long int date, long int time, bool SetInv=false, bool Tag=false);

        /**
        * @brief Are two planes connected to each other at a specific time ?
        *
        * @param[in] plane1 Origin plane
        * @param[in] plane2 Destination plane
        * @param[in] SetInv Compute the inverse if set
        * @param[in] Tag Tag the path if set
        *
        * @return true if the process has been a success.
        *
        * @remark Is there a transformation between the 2 planes Plane1->Plane2 ?
        * Return true if there is a transformation in the direction Plane2->Plane1
        * If SetInv is true, Plane1->Plane2 is computed as the inverse of Plane2->Plane1
        * with the same state as Plane2->Plane1
        * If Tag is true, the path is tagged
        */
        bool isConnected( unsigned int Plane1, unsigned int Plane2, long int date, long int time, bool SetInv=false, bool Tag=false);

        //! @brief Set the transformation at index
        bool setTrf( unsigned int index, const vnl_rigid_matrix& T, long int date, long int time );
    
        //! @brief Compute the transformation between the first and the last of path
        //bool setTrf( const std::vector< unsigned int > &path );
        bool setTrf( const Path &path );

        //! @brief Reset all deduced transformations related to the transformation at the index
        void eraseComputedTrf( unsigned int index );

        //! @brief List of matrix of all transformations between all planes. The diag is not used.
        std::vector< vnl_rigid_matrix > m_trfTable;

        //! @brief List of states of the transformations. The diag is not used.
        std::vector<ARL_PLANE_STATE> m_trfState;

        //! @brief List of states of the transformations. The diag is not used.
        std::vector<double> m_trfWeight;

        //! @brief List of names of each plane
        std::vector<std::string> m_lstName;

        //! @brief Status for each plane. True, if valid, false if it's released
        std::vector<bool> m_status;
        std::vector<unsigned int> m_releasedPlanes;
    };

    class TransformationFilter
    {
    public:
        /**
         * @brief Constructor
         * @param[in] duration Duration of filter in milliseconds
         * @param[in] type Filter type
         */
        ARLCORE_API TransformationFilter( const PlaneSystem&, long int duration, ARLCORE_TRF_FILTER_TYPE );

        //! @brief Copy constructor
        ARLCORE_API TransformationFilter( const TransformationFilter& );

        //! @brief Destructor
        ARLCORE_API ~TransformationFilter( void );

        //! @brief Add a new transformation
        ARLCORE_API void push_back( const vnl_rigid_matrix& );

        //! @return Filtered transformation
        ARLCORE_API unsigned int get( arlCore::vnl_rigid_matrix& );

    private:
        /**
         * @brief Delete too old transformations
         * @param[in] all if true, test all transformations else just the last
         */
        void timeCleaning( bool all=false );

        void clear( void );

        const PlaneSystem& m_universe;
        long int m_duration;
        long int m_lastTime;
        ARLCORE_TRF_FILTER_TYPE m_filterType;
        std::vector<const vnl_rigid_matrix*> m_trfList;
    };
} // namespace arlCore
#endif // _ARLCORE_PLANE_SYSTEM_H

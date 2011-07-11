/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _ARLCORE_SCENE_H
#define _ARLCORE_SCENE_H
#include <arlcore/Common.h>

#include <vector>
#include <map>

#include <arlcore/PlaneSystem.h>
#include <arlcore/Camera.h>
#include <arlcore/CameraList.h>
#include <arlcore/Point.h>
#include <arlcore/PointsList.h>
#include <arlcore/Tags.h>

namespace arlCore
{
    class SmartPointList;
    class Scene
    {
    /**
     * @class   Scene
     * @author  IRCAD (Research and Development Team)
     * @date    2007
     * @brief   Virtual scene generator
     * A scene is a plane system with cameras and tags wich contain lists of point
     * After setting cameras and tags in the scene, we can simulate the detection of the tag points
     * on the image produced by the cameras.
     * We can also apply a dynamic behavior to the points, as for example collapsing
     * 
     * This class contains basic methods to create a virtual world. Practically, you have
     * to define a new class (see in Scenes.h) that derives from Scene, and to write 
     * the specific methods you need for the virtual world you create for your experiment.
     */
    public:
        //! @brief Constructor of an empty scene in the planesystem
        ARLCORE_API Scene( PlaneSystem& );

        //! @brief Destructor
        ARLCORE_API ~Scene( void );

        /**
         * @brief This function add in the scene a Tag (3D object composed of a 3D point list) that is
         * randomly generated from the parameter ARLCORE_SHAPE, centre and size (see the function
         * PointList::shapeRandom() in PointList.h)
         */
        ARLCORE_API Tag* addTag(unsigned int nbPoints, ARLCORE_SHAPE shapeType, const Point &centre, double size);

        /**
         * @brief 
         * Once a scene is created, this function creates "nbCams" camera in the world. The camera 
         * optical centers are randomly created in a sphere of radius "sphereRadius" and of center "centre".
         * All the cameras are looking toward the Point "centre". Their orientation is chosen randomly as well.
         * In order to avoid camera creation too close to "centre". Each camera is moved away from "centre"
         * along the axis ("centre" camera_optical_center) with a magnitude of distMin. The intrinsic parameters
         * of each camera are randomly chosen using "intrinsicParam" and "intrinsicRange":
         * fx = intrinsicParam[0] + UniformNoise(intrinsic_range[0])
         * fy = intrinsicParam[1] + UniformNoise(intrinsic_range[1])
         * cx = intrinsicParam[2] + UniformNoise(intrinsic_range[2])
         * cy = intrinsicParam[3] + UniformNoise(intrinsic_range[3])
         * k1 = intrinsicParam[4] + UniformNoise(intrinsic_range[4])
         * k2 = intrinsicParam[5] + UniformNoise(intrinsic_range[5])
         * k3 = intrinsicParam[6] + UniformNoise(intrinsic_range[6])
         * k4 = intrinsicParam[7] + UniformNoise(intrinsic_range[7])
         */
        ARLCORE_API virtual bool addCameras(unsigned int nbCams, const Point &centre, double sphereRadius, double distMin,
            const std::vector<double> &intrinsicParam, const std::vector<double> &intrinsicRange);

        /**
         * @brief 
         * Once a scene is created, this function creates "nbCams" camera in the world. The camera 
         * optical centers are randomly created in a solid angle of angle "angleMin" and 
         * of radius "sphereRadius" and of center "centre".
         * All the cameras are looking toward the Point "centre". Their orientation is chosen randomly as well.
         * In order to avoid camera creation too close to "centre". Each camera is moved away from "centre"
         * along the axis ("centre" camera_optical_center) with a magnitude of distMin. The intrinsic parameters
         * of each camera are randomly chosen using "intrinsicParam" and "intrinsicRange":
         * fx = intrinsicParam[0] + UniformNoise(intrinsic_range[0])
         * fy = intrinsicParam[1] + UniformNoise(intrinsic_range[1])
         * cx = intrinsicParam[2] + UniformNoise(intrinsic_range[2])
         * cy = intrinsicParam[3] + UniformNoise(intrinsic_range[3])
         * k1 = intrinsicParam[4] + UniformNoise(intrinsic_range[4])
         * k2 = intrinsicParam[5] + UniformNoise(intrinsic_range[5])
         * k3 = intrinsicParam[6] + UniformNoise(intrinsic_range[6])
         * k4 = intrinsicParam[7] + UniformNoise(intrinsic_range[7])
         */
        ARLCORE_API virtual bool addCameras(unsigned int nbCams, const Point &centre, double sphereRadius, double distMin,
            const std::vector<double> &intrinsicParam, const std::vector<double> &intrinsicRange, double angleMin);

        //! @brief Visualise la scene avec gnuplot
        ARLCORE_API bool plot( void ) const;

        //! @brief Get the plane system id
        ARLCORE_API PlaneSystem& getPlaneSystem( void );

        //! @brief Get the number of camera in the scene
        ARLCORE_API unsigned int getNbCameras( void ) const;

        //! @brief Get a vector that contains pointer on each camera in the scene
        ARLCORE_API const arlCore::CameraList& getCameras( void ) const;

        //! @brief Get Tags in the scene
        ARLCORE_API Tags& getTags( void );

        /**
         * @brief 
         * This function is fondamental, it provides in a SmartPointList "spl" the point 2D coordinates
         * that corresponds to the projection of the points belonging to the Tag "tagNo" in the 
         * video image of the camera "cam".
         * 
         * In case you want to add some noise to the coordinates you get (for evaluation purpose...),
         * you can add it using the variable "gaussianNoise". It will add zero mean gaussian noise
         * with std dev "gaussianNoise" on each point coordinate.
         * 
         */
        ARLCORE_API unsigned int detection( unsigned int cam, unsigned int tagNo, SmartPointList &spl, double gaussianNoise=0 );

    protected:
        ARLCORE_API Scene( PlaneSystem&, Tags* );
        ARLCORE_API virtual bool dynamicBehavior( unsigned int step );
        ARLCORE_API virtual unsigned int detectionBehavior( unsigned int cam, SmartPointList &, std::vector< const Tag* > &tags, std::map< const Tag*, unsigned int >& allTags );

        Tags *m_tags;

    private:
        arlCore::CameraList m_cameras;
        PlaneSystem &m_universe;
    };
} // namespace ArlCore
#endif // _ARLCORE_SCENE_H

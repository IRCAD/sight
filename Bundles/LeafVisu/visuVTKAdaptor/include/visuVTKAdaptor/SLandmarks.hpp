/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUVTKADAPTOR_SLANDMARKS_HPP__
#define __VISUVTKADAPTOR_SLANDMARKS_HPP__

#include "visuVTKAdaptor/config.hpp"

#include <fwData/Landmarks.hpp>

#include <fwRenderVTK/IVtkAdaptorService.hpp>
#include <fwRenderVTK/vtk/fwHandleRepresentation3D.hpp>

#include <fwThread/Timer.hpp>

#include <vtkHandleWidget.h>
#include <vtkSmartPointer.h>

class vtkCommand;
class vtkActor2D;

namespace visuVTKAdaptor
{

/**
 * @brief Adaptor for landmarks in the field of an image.
 *
 * Registers a ::fwData::PointList in to the generic scene for the landmarks in the given Image
 *
 * @section Slots Slots
 * - \b addPoint(groupName): add the new point in the scene
 * - \b insertPoint(groupName, index): add the new point in the scene
 * - \b removePoint(groupName, index): remove the point from the scene
 * - \b modifyGroup(groupName): modify the landmarks color, label, visibility, ...
 * - \b removeGroup(groupName): remove the group's landmarks from the scene
 * - \b addGroup(groupName): add group's landmarks to the scene
 * - \b modifyPoint(groupName, index): modify point position
 * - \b renameGroup(oldName, newName): update the label with the new name
 * - \b selectPoint(groupName, index): select the point (blink from point color to green)
 * - \b deselectPoint(groupName, index): deselect the point (reset current color)
 * - \b show(bool): show or hide the landmarks
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
        <adaptor id="..." class="::visuVTKAdaptor::Landmarks" objectId="imageKey">
            <config renderer="default" picker="default" transform="..."/>
        </adaptor>
   @endcode
 * @subsection Configuration Configuration
 * - \b transform : transformation matrix applied to the current landmark.
 *
 */
class VISUVTKADAPTOR_CLASS_API SLandmarks : public ::fwRenderVTK::IVtkAdaptorService
{

public:

    fwCoreServiceClassDefinitionsMacro( (SLandmarks)(::fwRenderVTK::IVtkAdaptorService) );
    fwCoreAllowSharedFromThis();

    /// Widget used to display and interact with landmarks.
    typedef vtkSmartPointer< vtkHandleWidget > LandmarkWidgetType;
    typedef vtkSmartPointer< vtkActor2D > LabelActorType;

    VISUVTKADAPTOR_API SLandmarks() throw();

    VISUVTKADAPTOR_API virtual ~SLandmarks() throw();

    VISUVTKADAPTOR_API virtual void show(bool b = true);

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect Image::s_LANDMARK_ADDED_SIG to this::s_UPDATE_LANDMARKS_SLOT
     * Connect Image::s_LANDMARK_REMOVED_SIG to this::s_UPDATE_LANDMARKS_SLOT
     * Connect Image::s_LANDMARK_DISPLAYED_SIG to this::s_UPDATE_LANDMARKS_SLOT
     */
    VISUVTKADAPTOR_API virtual KeyConnectionsType getObjSrvConnections() const;

    /// Deselect the current point and emit the corresponding Landmarks signal
    void deselect();

protected:

    VISUVTKADAPTOR_API void doStart() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doStop() throw(fwTools::Failed);

    VISUVTKADAPTOR_API void doConfigure() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doSwap() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doUpdate() throw(fwTools::Failed);

private:

    /// Stores a group of widgets.
    typedef std::vector< LandmarkWidgetType > LandmarksWidgetContainerType;

    /// Maps group names to widget groups.
    typedef std::map< std::string, LandmarksWidgetContainerType > GroupWidgetsMapType;

    /// Maps Landmarks widget to labels
    typedef std::map< LandmarkWidgetType, LabelActorType > WidgetLabelMapType;

    /// Maps Landmarks widget to its vtkCommand
    typedef std::map< LandmarkWidgetType, vtkCommand* > WidgetCommandMapType;

    typedef std::pair< std::string, size_t > PointPairType;

    /// Slot: called when a new point is appended to the group.
    void addPoint(std::string groupName);

    /// Slot: called when a landmark is inserted inside a group.
    void insertPoint(std::string groupName, size_t index);

    /// Slot: called when a landmark is removed.
    void removePoint(std::string groupName, size_t index);

    /// Slot: called when a group attribute (other than it's points) is modified.
    void modifyGroup(std::string groupName);

    /// Slot: called when a group has been removed.
    void removeGroup(std::string groupName);

    /// Slot: called when a new landmark group is created.
    void addGroup(std::string groupName);

    /// Slot: called when a point is modified
    void modifyPoint(std::string groupName, size_t index);

    /// Slot: called when a group is renamed
    void renameGroup(std::string oldName, std::string newName);

    /// Slot: called when a point is selected
    void selectPoint(std::string groupName, size_t index);

    /// Slot: called when a point is deselected
    void deselectPoint(std::string groupName, size_t index);

    /// Switch color for selected point: (even: green, odd: point default color). Called by the timer
    void changeColor(const vtkSmartPointer< ::fwRenderVTK::vtk::fwHandleRepresentation3D >& rep,
                     const std::array<double, 3>& color1, const std::array<double, 3>& color2);

    /// Remove all the points from the scene
    void clearLandmarks();

    /// Creates a new handle configured with it's group attributes and it's position.
    vtkSmartPointer<vtkHandleWidget> newHandle(const ::fwData::Landmarks::sptr& landmarks,
                                               const std::string& groupName,
                                               size_t pointIndex);

    /// command to listen left click outside landmarks to deselect them
    vtkCommand* m_noSelectionCommand;

    /// Structure holding all widgets groups and mapping them to their names.
    GroupWidgetsMapType m_handles;

    /// Structure to associate widgets to label
    WidgetLabelMapType m_labels;

    /// Structure to associate widgets to vtkCommand
    WidgetCommandMapType m_commands;

    /// Timer used to blink selected point
    ::fwThread::Timer::sptr m_timer;

    /// Selected point information (groupName, index)
    PointPairType m_selectedPoint;

    /// Counter to switch color for selected point: (even: green, odd: point default color)
    size_t m_count;

    /// boolean to indicates if the transform is present
    bool m_hasTransform;
};

} //namespace visuVTKAdaptor

#endif // __VISUVTKADAPTOR_SLANDMARKS_HPP__

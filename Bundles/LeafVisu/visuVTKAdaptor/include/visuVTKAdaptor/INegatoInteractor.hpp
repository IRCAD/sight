/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */
/* ***** BEGIN CONTRIBUTORS BLOCK *****
 * Initial authors:
 *  - Jean-Baptiste.Fasquel (LISA Laboratory, Angers University, France)
 * ****** END CONTRIBUTORS BLOCK ****** */

#ifndef VISUVTKADAPTOR_INEGATOINTERACTOR_HPP_
#define VISUVTKADAPTOR_INEGATOINTERACTOR_HPP_

#include <utility>
#include <fwServices/IService.hpp>
#include <fwData/ValuedPoint.hpp>
#include <fwComEd/helper/MedicalImageAdaptor.hpp>

#include "visuVTKAdaptor/NegatoOneSlice.hpp"
#include "visuVTKAdaptor/config.hpp"

namespace visuVTKAdaptor
{

class NegatoOneSlicePickingCallback;

/**
 * @brief   Base class for interactions on 2D negatoscope.
 * @class   INegatoInteractor.
 * @author Copyright (C) LISA Laboratory, Angers University (Jean-Baptiste Fasquel).
 * @date    2010.
 *
 * This class defines the API of a type service dedicated to interactions on 2D Negatoscope. Implementations of the type of service will be associated to the NegatoOneSlice class.
 * @note This could be further generalized to interaction on any kind of rendered element
 */
class VISUVTKADAPTOR_CLASS_API INegatoInteractor : public ::fwServices::IService
{
public:


    fwCoreServiceClassDefinitionsMacro ( (INegatoInteractor)(::fwServices::IService) ) ;

    ///Constructor
    VISUVTKADAPTOR_API INegatoInteractor() throw() ;
    ///Destructor
    VISUVTKADAPTOR_API virtual ~INegatoInteractor() throw() ;
    /**
     * @name General service methods
     */

    //@{
    /// Update some attributes and invoke doStart()
    VISUVTKADAPTOR_API virtual void starting() throw ( ::fwTools::Failed ) ;

    /// Invoke doStart()
    VISUVTKADAPTOR_API virtual void stopping() throw ( ::fwTools::Failed ) ;
    /// Invoke doConfigure()
    VISUVTKADAPTOR_API virtual void configuring() throw ( ::fwTools::Failed ) ;
    /// Invoke doUpdate()
    VISUVTKADAPTOR_API virtual void updating() throw ( ::fwTools::Failed ) ;
    /// Invoke doUpdate(_msg)
    VISUVTKADAPTOR_API virtual void updating( fwServices::ObjectMsg::csptr _msg ) throw ( ::fwTools::Failed ) ;
    //@}


    /**
     * @name Setters
     */

    //@{
    /**
     * Updates the picked index on negatoscope, and the intensity
     * @param[in] _index[3] the picked index (x - 0, y - 1, z - 2)
     * @param[in] value the picked intensity
     * @note it corresponds to the last picked index IN negatoscope
     */
    VISUVTKADAPTOR_API void updatePickedIndex(int _index[3],double _value);

    /**
     * Updates the picked index on negatoscope
     * @param[in] _index[3] the picked index (x - 0, y - 1, z - 2)
     * @note it corresponds to the last picked index IN negatoscope
     */
    VISUVTKADAPTOR_API void updateWorldPickedPoint(double _point[3]);

    /**
     * Updates the picking event identifier (see vtkCommand event identifiers)
     * @param[in] eventId the event identifier
     */
    VISUVTKADAPTOR_API void updatePickingEventId(unsigned long eventId);

    /**
     * Updates the picking validity
     * @param[in] isOnNegato true iff picking occurs on negatoscope
     */
    VISUVTKADAPTOR_API void updatePickingValidity(bool isOnNegato);
    //@}

    /**
     * Process picking: update the text to be displayed and force rendering update
     * @note Should be done through the update() method in order to preserve the genericity
     */
    VISUVTKADAPTOR_API virtual void processPicking() throw(fwTools::Failed) ;

protected :

    /**
     * @name Methods to be overriden by specific services
     */

    //@{
    /// Does nothing special
    VISUVTKADAPTOR_API void doSwap() throw(fwTools::Failed) {};
    /// Does nothing special
    VISUVTKADAPTOR_API virtual void doConfigure() throw(fwTools::Failed) {};
    /// Does nothing special
    VISUVTKADAPTOR_API void doUpdate(fwServices::ObjectMsg::csptr message ) throw(fwTools::Failed) {};

    /// Do not override
    VISUVTKADAPTOR_API void doUpdate() throw(fwTools::Failed) {};

    /// To attach m_pickingCallBack to the interactor (for specific types of vtk events)
    VISUVTKADAPTOR_API virtual void doStart() throw(fwTools::Failed) = 0 ;
    /// Detach m_pickingCallBack to the interactor (for specific types of vtk events)
    VISUVTKADAPTOR_API virtual void doStop() throw(fwTools::Failed) = 0 ;

    /// Invoked by the processPicking() method: override it define specific behaviours
    VISUVTKADAPTOR_API virtual void doProcessPicking() throw(fwTools::Failed) = 0 ;
    //@}


    ///Return negatoscope orientation
    VISUVTKADAPTOR_API ::fwData::Image::wptr                            getImage();

    ///Return negatoscope orientation
    VISUVTKADAPTOR_API ::fwComEd::helper::MedicalImageAdaptor::Orientation    getOrientation();

    ///Use to store axis of plane orthogonal to the current orientation
    typedef std::pair< int , int > TAxis;

    /**
     * Return axis orthogonal versus the orientation.
     * For example, if orientation is Z_AXIS, valid indices are 0 and 1
     * For example, if orientation is Y_AXIS, valid indices are 0 and 2
     * For example, if orientation is X_AXIS, valid indices are 1 and 2
     * @return a pair storing indices
     */
    VISUVTKADAPTOR_API TAxis    getPlaneAxis();

    /**
     * @name Data available when a picking is processed
     */

    //@{
    unsigned long                                             m_vtkEventId ;
    ::fwData::ValuedPoint::sptr                                m_valuedIndex;
    ::fwData::Point::sptr                                    m_worldPoint;
    bool                                                    m_isPickingInside;
    bool                                                    m_isCtrlPressed;
    bool                                                    m_isShiftPressed;
    char                                                    m_keyCode;
    double                                                     m_priority;
    //@}

    /// VtkCommand managing the picking: the picker only has the vtkActor representing the negatoscope
    NegatoOneSlicePickingCallback    *                        m_pickingCallBack;
    /// The reference service: the negatoscope
    ::visuVTKAdaptor::NegatoOneSlice::wptr                     m_adaptor ;


} ;

}

#endif /*VISUVTKADAPTOR_INEGATOINTERACTOR_HPP_*/

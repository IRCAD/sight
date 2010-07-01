/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */
/* ***** BEGIN CONTRIBUTORS BLOCK *****
 * Initial authors:
 *  - Jean-Baptiste.Fasquel (LISA Laboratory, Angers University, France)
 * ****** END CONTRIBUTORS BLOCK ****** */

#ifndef _VISUVTKADAPTOR_NEGATOWINDOWER_HPP_
#define _VISUVTKADAPTOR_NEGATOWINDOWER_HPP_

#include <fwData/TransfertFunction.hpp>

#include <fwComEd/helper/MedicalImageAdaptor.hpp>
#include <fwRenderVTK/IVtkAdaptorService.hpp>

#include "visuVTKAdaptor/config.hpp"
#include "visuVTKAdaptor/NegatoOneSlice.hpp"

class vtkCommand;

namespace visuVTKAdaptor
{


/**
 * @brief   Windowing on 2D negatoscope.
 * @class   NegatoWindower.
 * @author  LISA Laboratory, Angers University.
 * @date 	2010
 *
 * This adaptor is designed to support attachement as a service to a NegatoOneSlice service. This service focuses on windowing the associated
 * image to which NegatoOneSlice is attached. Windowing is performed is mouse moving with right mouse button pressed
 * This service can be integrated as
 * \code
 * <object type="Image">
 * <service implementation="::visuVTKAdaptor::NegatoOneSlice" type="::fwRenderVTK::IVtkAdaptorService" autoComChannel="yes">
 *   <config scene="genericScene" renderer="default" picker="myPicker" sliceIndex="axial" />
 *   <service implementation="::visuVTKAdaptor::NegatoWindower" type="::fwRenderVTK::IVtkAdaptorService" autoComChannel="no" />
 * </service>
 * </object>
 * \endcode
 * instead of, with NegatoWindowingInteractor
 * \code
 * <service uid="genericScene" implementation="::fwRenderVTK::VtkRenderService" type="::fwRender::IRender" autoComChannel="yes">
 *  <scene>
 *   <adaptor uid="negato" id="negato" class="::visuVTKAdaptor::NegatoOneSlice" objectId="myImage">
 *    <config renderer="default" picker="myPicker" sliceIndex="axial" />
 *   </adaptor>
 *   <adaptor uid="interactor" id="interactor" class="::visuVTKAdaptor::NegatoWindowingInteractor" objectId="myImage">
 *     <config renderer="default" picker="myPicker" />
 *    </adaptor>
 *  </scene>
 * </service>
 * \endcode
 * This class is similar to the NegatoWindowingInteractor except that
 *  \li
 *  simplified configuration (XML), as deduced from the one of the NegatoOneSlice: both renderer and picker are automatically retrieved
 * \li
 *  it does not inherit ::fwComEd::helper::MedicalImageAdaptor
 *  \li
 *  its work as a service of service (automatic destruction when removing the negatoscope
 *
 * @note Perspective: manage windowing on negato without systematically modifying image windowing, to avoid systematic synchronisation of windowing when there
 * are several negatoscope on a similar image, each one presenting a specific windowing (managed by its NegatoWindower subservice).
 */
class VISUVTKADAPTOR_CLASS_API NegatoWindower : public ::fwRenderVTK::IVtkAdaptorService
{
public:
	/// To define the picking mode being either single or multi (point(s))
	typedef enum
	{
		SINGLEPOINT,
		MULTIPOINT
	} PickingMode;

    fwCoreServiceClassDefinitionsMacro ( (NegatoWindower)(::fwRenderVTK::IVtkAdaptorService) ) ;

    VISUVTKADAPTOR_API NegatoWindower() throw();

    VISUVTKADAPTOR_API virtual ~NegatoWindower() throw();
    /**
     * @name Specific methods used by the callback
     */

    //@{
    ///qsdfqsdf
    VISUVTKADAPTOR_API void startWindowing();
    ///qsdfqsdf
    VISUVTKADAPTOR_API void stopWindowing();

    VISUVTKADAPTOR_API void updateWindowing( double , double);

    VISUVTKADAPTOR_API void resetWindowing();
    //@}
protected:
    /**
      * @name General service methods
      */

     //@{
    VISUVTKADAPTOR_API void doStart() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doStop() throw(fwTools::Failed);
    ///qsdfqsdf
    VISUVTKADAPTOR_API void configuring() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doSwap() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doUpdate() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doUpdate(::fwServices::ObjectMsg::csptr msg) throw(fwTools::Failed);
    //@}
    ///Callback
    vtkCommand *m_vtkObserver;
    ///Associated negatoscope
    ::visuVTKAdaptor::NegatoOneSlice::sptr m_negato;
    double m_initialWindow;
    double m_initialLevel;
    double m_priority;
};




} //namespace visuVTKAdaptor

#endif // _VISUVTKADAPTOR_NEGATOWINDOWER_HPP_

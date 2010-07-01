/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */
/* ***** BEGIN CONTRIBUTORS BLOCK *****
 * Initial authors:
 *  - Jean-Baptiste.Fasquel (LISA Laboratory, Angers University, France)
 * ****** END CONTRIBUTORS BLOCK ****** */

#ifndef _VISUVTKADAPTOR_NEGATOPOINTPICKER_HPP_
#define _VISUVTKADAPTOR_NEGATOPOINTPICKER_HPP_

#include <fwData/TransfertFunction.hpp>

#include <fwComEd/helper/MedicalImageAdaptor.hpp>
#include <fwRenderVTK/IVtkAdaptorService.hpp>

#include "visuVTKAdaptor/config.hpp"
#include "visuVTKAdaptor/Text.hpp"
#include "visuVTKAdaptor/NegatoOneSlice.hpp"

class vtkCommand;

namespace visuVTKAdaptor
{


/**
 * @brief   Picking on 2D negatoscope.
 * @class   NegatoPointPicker.
 * @author  LISA Laboratory, Angers University.
 * @date    2010.
 *
 * This adaptor is designed to support attachement as a service to a NegatoOneSlice service. This service focuses on picking, picked points being
 * affected to the associated image through the identifier ::fwComEd::ImageMsg::PICKEDPOINTS, on pressing the left mouse button.
 * This service can be integrated as
 * \code
 * <object type="Image">
 * <service implementation="::visuVTKAdaptor::NegatoOneSlice" type="::fwRenderVTK::IVtkAdaptorService" autoComChannel="yes">
 *   <config scene="genericScene" renderer="default" picker="myPicker" sliceIndex="axial" />
 *   <service implementation="::visuVTKAdaptor::NegatoPointPicker" type="::fwRenderVTK::IVtkAdaptorService" autoComChannel="no" mode="single" text="off"/>
 * </service>
 * </object>
 * \endcode
 * Options are
 * \li
 * mode can single (one point is picked) or multi (several points are picked before notifying)
 * \li
 * text (on/off) display coordinates and current value of the point indicated by the mouse (changes on mouse move)
 */
class VISUVTKADAPTOR_CLASS_API NegatoPointPicker : public ::fwRenderVTK::IVtkAdaptorService
{

public:
	/// To define the picking mode being either single or multi (point(s))
	typedef enum
	{
		SINGLEPOINT,
		MULTIPOINT
	} PickingMode;

    fwCoreServiceClassDefinitionsMacro ( (NegatoPointPicker)(::fwRenderVTK::IVtkAdaptorService) ) ;

    VISUVTKADAPTOR_API NegatoPointPicker() throw();

    VISUVTKADAPTOR_API virtual ~NegatoPointPicker() throw();

    /**
     * @name Specific methods used by the callback
     */

    //@{
    VISUVTKADAPTOR_API void addPickedPoint(double pickedPoint[3]) throw(fwTools::Failed);
    VISUVTKADAPTOR_API void notifyPicking();
    VISUVTKADAPTOR_API void emptyPickedPoints();
    VISUVTKADAPTOR_API void updateText(double pickedPoint[3]) throw(fwTools::Failed);
    VISUVTKADAPTOR_API void clearText() throw(fwTools::Failed);
    VISUVTKADAPTOR_API PickingMode getPickingMode();
    VISUVTKADAPTOR_API bool isTextShown();
    //@}


protected:

    /**
     * @name General service methods
     */

    //@{
    VISUVTKADAPTOR_API void doStart() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doStop() throw(fwTools::Failed);

    VISUVTKADAPTOR_API void configuring() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doSwap() throw(fwTools::Failed);

    VISUVTKADAPTOR_API void doUpdate() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doUpdate(::fwServices::ObjectMsg::csptr msg) throw(fwTools::Failed);
    //@}
    /// Callback
    vtkCommand *m_vtkObserver;

    ///base object
    ::visuVTKAdaptor::NegatoOneSlice::wptr 	m_negato;
    ///base object of base object
    ::fwData::Image::wptr					m_image;
    ///image field
    ::fwData::ValuedPointList::wptr 		m_pickedPoints;
    ///Observation priority
    double m_priority;
    ///Picking mode
    PickingMode	m_pickingMode;
    ///Text shown on/off
    bool		m_showText;
    ///Text adaptor attached to the image (if configured for showing text)
    ::visuVTKAdaptor::Text::wptr            m_textAdaptor;
};




} //namespace visuVTKAdaptor

#endif // _VISUVTKADAPTOR_NEGATOPOINTPICKER_HPP_

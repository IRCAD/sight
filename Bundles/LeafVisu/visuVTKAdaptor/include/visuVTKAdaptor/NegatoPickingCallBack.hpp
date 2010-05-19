/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */
/* ***** BEGIN CONTRIBUTORS BLOCK *****
 * Initial authors:
 *  - Jean-Baptiste.Fasquel (LISA Laboratory, Angers University, France)
 * ****** END CONTRIBUTORS BLOCK ****** */

#ifndef VISUVTKADAPTOR_NEGATOPICKINGCALLBACK_HPP_
#define VISUVTKADAPTOR_NEGATOPICKINGCALLBACK_HPP_

#include <vtkCommand.h>
#include <vtkImageData.h>

#include <fwRenderVTK/IVtkAdaptorService.hpp>

#include "visuVTKAdaptor/NegatoOneSlice.hpp"
#include "visuVTKAdaptor/config.hpp"
#include "visuVTKAdaptor/INegatoInteractor.hpp"

namespace visuVTKAdaptor
{

class VISUVTKADAPTOR_CLASS_API NegatoOneSlicePickingCallback : public vtkCommand
{
public:
    VISUVTKADAPTOR_API static NegatoOneSlicePickingCallback *New() ;

    VISUVTKADAPTOR_API NegatoOneSlicePickingCallback() ;

    VISUVTKADAPTOR_API ~NegatoOneSlicePickingCallback() ;

    VISUVTKADAPTOR_API virtual void Execute( vtkObject *caller, unsigned long eventId, void *) ;

    VISUVTKADAPTOR_API void setContext( ::visuVTKAdaptor::NegatoOneSlice::wptr adaptor,::visuVTKAdaptor::INegatoInteractor::wptr processor) ;

protected :

    void showInformations() ;

    ::visuVTKAdaptor::NegatoOneSlice::wptr                     m_adaptor ;
    vtkImageData *                                            m_vtkImage;
    ::visuVTKAdaptor::INegatoInteractor::wptr                 m_pickingProcessor ;
    vtkAbstractPropPicker *                                    m_picker;

};

}

#endif /*VISUVTKADAPTOR_NEGATOPICKINGCALLBACK_HPP_*/

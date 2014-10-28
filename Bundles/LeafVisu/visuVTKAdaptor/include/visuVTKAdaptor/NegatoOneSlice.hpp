/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _VISUVTKADAPTOR_NEGATOONESLICE_HPP_
#define _VISUVTKADAPTOR_NEGATOONESLICE_HPP_

#include <fwData/Image.hpp>

#include <fwRenderVTK/IVtkAdaptorService.hpp>
#include <fwComEd/helper/MedicalImageAdaptor.hpp>

#include "visuVTKAdaptor/config.hpp"

class vtkObject;


namespace visuVTKAdaptor
{

class VISUVTKADAPTOR_CLASS_API NegatoOneSlice: public ::fwComEd::helper::MedicalImageAdaptor, public ::fwRenderVTK::IVtkAdaptorService
{

public:

    fwCoreServiceClassDefinitionsMacro ( (NegatoOneSlice)(::fwRenderVTK::IVtkAdaptorService) ) ;

    VISUVTKADAPTOR_API NegatoOneSlice() throw();

    VISUVTKADAPTOR_API virtual ~NegatoOneSlice() throw();

    void setAllowAlphaInTF(bool allow) {m_allowAlphaInTF = allow;};
    void setInterpolation(bool interpolation){m_interpolation = interpolation;};
    void setVtkImageSourceId(std::string id) {m_imageSourceId = id;};
    void setVtkImageSource(vtkObject *obj)   {m_imageSource = obj;};
    void setActorOpacity(double actorOpacity) {m_actorOpacity = actorOpacity;};

protected :

    VISUVTKADAPTOR_API void doStart() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doStop() throw(fwTools::Failed);

    VISUVTKADAPTOR_API void doUpdate() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doReceive(::fwServices::ObjectMsg::csptr msg) throw(fwTools::Failed);

    /**
     * @brief Configures the service
     *
     * @verbatim
       <adaptor id="negatoOneSlice" class="::visuVTKAdaptor::NegatoOneSlice" objectId="imageKey">
           <config renderer="default" picker="negatodefault" sliceIndex="axial"
                   transform="trf" tfalpha="yes" interpolation="off" vtkimagesource="imgSource" actorOpacity="1.0"
                   selectedTFKey="tkKey" tfSelectionFwID="selectionID" />
       </adaptor>
       @endverbatim
     * - \b renderer (mandatory): defines the renderer to show the arrow. It must be different from the 3D objects renderer.
     * - \b picker (mandatory): identifier of the picker
     * - \b sliceIndex (optional, axial/frontal/sagittal, default=axial): orientation of the negato
     * - \b transform (optional): the vtkTransform to associate to the adaptor
     * - \b interpolation (optional, yes/no, default=yes): if true, the image pixels are interpolated
     * - \b vtkimagesource (optional): source image, used for blend
     * - \b actorOpacity (optional, default=1.0): actor opacity (float)
     * - \b tfSelectionFwID (optional): fwID of the composite containing transfer functions
     * - \b selectedTFKey (optional): key of the transfer function to use in negato
     */
    VISUVTKADAPTOR_API void configuring() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doSwap() throw(fwTools::Failed);

    bool m_manageImageSource;
    std::string m_imageSourceId;
    vtkObject  *m_imageSource;

    bool m_allowAlphaInTF;
    bool m_interpolation;
    double m_actorOpacity;

    vtkObject* getImageSource();
    void cleanImageSource();
    ::fwRenderVTK::IVtkAdaptorService::sptr getImageSliceAdaptor();
    ::fwRenderVTK::IVtkAdaptorService::sptr getImageAdaptor();

    ::fwRenderVTK::IVtkAdaptorService::wptr m_imageAdaptor;
    ::fwRenderVTK::IVtkAdaptorService::wptr m_imageSliceAdaptor;

};




} //namespace visuVTKAdaptor

#endif // _VISUVTKADAPTOR_NEGATOONESLICE_HPP_

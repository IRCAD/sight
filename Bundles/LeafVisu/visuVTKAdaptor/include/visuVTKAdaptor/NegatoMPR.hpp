/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _VISUVTKADAPTOR_NEGATOMPR_HPP_
#define _VISUVTKADAPTOR_NEGATOMPR_HPP_

#include "visuVTKAdaptor/IImagesAdaptor.hpp"

#include "visuVTKAdaptor/config.hpp"

namespace visuVTKAdaptor
{


class VISUVTKADAPTOR_CLASS_API NegatoMPR: public ::visuVTKAdaptor::IImagesAdaptor
{

public:

    fwCoreServiceClassDefinitionsMacro ( (NegatoMPR)(::fwRenderVTK::IVtkAdaptorService::Baseclass) ) ;

    VISUVTKADAPTOR_API NegatoMPR() throw();

    VISUVTKADAPTOR_API virtual ~NegatoMPR() throw();

    typedef enum
    {
        NO_SLICE = 0,
        ONE_SLICE,
        THREE_SLICES
    } SliceMode;

protected :

    typedef ::fwRuntime::ConfigurationElement::sptr Configuration;

    VISUVTKADAPTOR_API void doStart() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doStop() throw(fwTools::Failed);

    VISUVTKADAPTOR_API void doUpdate() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doUpdate(::fwServices::ObjectMsg::csptr msg) throw(fwTools::Failed);
    VISUVTKADAPTOR_API void configuring() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doSwap() throw(fwTools::Failed);

    void addAdaptor(std::string adaptor, int axis=-1);

    void setSliceMode(SliceMode sliceMode);
    SliceMode getSliceMode();
    bool is3dModeEnabled();
    void set3dMode( bool enabled );

private:

    bool m_3dModeEnabled;
    SliceMode m_sliceMode;
    SliceMode m_backupedSliceMode;
};




} //namespace visuVTKAdaptor

#endif // _VISUVTKADAPTOR_NEGATOMPR_HPP_

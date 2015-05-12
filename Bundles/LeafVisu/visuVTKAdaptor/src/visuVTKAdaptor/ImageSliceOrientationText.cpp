/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuVTKAdaptor/ImageSliceOrientationText.hpp"

#include <fwComEd/fieldHelper/MedicalImageHelpers.hpp>

#include <fwServices/Base.hpp>

#include <fwData/Image.hpp>

#include <vtkSmartPointer.h>
#include <vtkTextProperty.h>
#include <vtkTextMapper.h>
#include <vtkActor2D.h>
#include <vtkRenderer.h>

#include <boost/algorithm/string.hpp>



fwServicesRegisterMacro( ::fwRenderVTK::IVtkAdaptorService, ::visuVTKAdaptor::ImageSliceOrientationText,
                         ::fwData::Object );

namespace visuVTKAdaptor
{

class ImageSliceOrientationTextPImpl
{
public:
    typedef vtkSmartPointer<vtkActor2D> TextActorPtr;
    typedef vtkSmartPointer<vtkTextMapper> TextMapperPtr;

    void configure(TextActorPtr& actor, TextMapperPtr& mapper)
    {
        actor  = TextActorPtr::New();
        mapper = TextMapperPtr::New();

        mapper->GetTextProperty()->SetFontFamilyToCourier(); // Fixed-width font
        mapper->GetTextProperty()->ShadowOn(); // better contrast
        mapper->GetTextProperty()->BoldOn();
        actor->SetMapper(mapper);
        actor->GetPositionCoordinate()->SetCoordinateSystemToNormalizedViewport();
        actor->GetPosition2Coordinate()->SetCoordinateSystemToNormalizedViewport();
    }

    ImageSliceOrientationTextPImpl()
    {
        configure(m_right, m_rightMapper);
        configure(m_left, m_leftMapper);
        configure(m_top, m_topMapper);
        configure(m_bottom, m_bottomMapper);

        vtkTextProperty* textProp;

        m_right->SetPosition(0.99, 0.5);
        m_rightMapper->SetInput("");
        textProp = m_rightMapper->GetTextProperty();
        textProp->SetJustificationToRight();
        textProp->SetVerticalJustificationToCentered();

        m_left->SetPosition(0.01, 0.5);
        m_leftMapper->SetInput("");
        textProp = m_leftMapper->GetTextProperty();
        textProp->SetJustificationToLeft();
        textProp->SetVerticalJustificationToCentered();

        m_top->SetPosition(0.5, 0.99);
        m_topMapper->SetInput("");
        textProp = m_topMapper->GetTextProperty();
        textProp->SetJustificationToCentered();
        textProp->SetVerticalJustificationToTop();

        m_bottom->SetPosition(0.5, 0.01);
        m_bottomMapper->SetInput("");
        textProp = m_bottomMapper->GetTextProperty();
        textProp->SetJustificationToCentered();
        textProp->SetVerticalJustificationToBottom();
    }

    void setText(const std::string &str)
    {
        m_rightStr     = "-";
        m_leftStr      = "-";
        m_anteriorStr  = "-";
        m_porteriorStr = "-";
        m_superiorStr  = "-";
        m_inferiorStr  = "-";

        if (!str.empty())
        {
            std::vector<std::string> locations;
            ::boost::algorithm::split( locations, str, ::boost::algorithm::is_any_of(",") );
            SLM_ASSERT("Six location should be given, got : " << locations.size() << ":" << str, locations.size());

            m_rightStr     = (locations.size()>0) ? locations[0] : "";
            m_leftStr      = (locations.size()>1) ? locations[1] : "";
            m_anteriorStr  = (locations.size()>2) ? locations[2] : "";
            m_porteriorStr = (locations.size()>3) ? locations[3] : "";
            m_superiorStr  = (locations.size()>4) ? locations[4] : "";
            m_inferiorStr  = (locations.size()>5) ? locations[5] : "";
        }

    }

    void setOrientation( ::fwComEd::helper::MedicalImageAdaptor::Orientation orientation )
    {
        switch (orientation)
        {
            case ::fwComEd::helper::MedicalImageAdaptor::X_AXIS: // Sagittal
                m_rightMapper->SetInput(m_anteriorStr.c_str());
                m_leftMapper->SetInput(m_porteriorStr.c_str());
                m_topMapper->SetInput(m_superiorStr.c_str());
                m_bottomMapper->SetInput(m_inferiorStr.c_str());
                break;
            case ::fwComEd::helper::MedicalImageAdaptor::Y_AXIS: // Frontal
                m_rightMapper->SetInput(m_leftStr.c_str());
                m_leftMapper->SetInput(m_rightStr.c_str());
                m_topMapper->SetInput(m_superiorStr.c_str());
                m_bottomMapper->SetInput(m_inferiorStr.c_str());
                break;
            case ::fwComEd::helper::MedicalImageAdaptor::Z_AXIS: // Axial
                m_rightMapper->SetInput(m_leftStr.c_str());
                m_leftMapper->SetInput(m_rightStr.c_str());
                m_topMapper->SetInput(m_anteriorStr.c_str());
                m_bottomMapper->SetInput(m_porteriorStr.c_str());
                break;
            default:
                m_rightMapper->SetInput("");
                m_leftMapper->SetInput("");
                m_topMapper->SetInput("");
                m_bottomMapper->SetInput("");
                SLM_ASSERT("bad orientation : " << orientation, 0);
        }
    }

    std::string m_rightStr;
    std::string m_leftStr;
    std::string m_anteriorStr;
    std::string m_porteriorStr;
    std::string m_superiorStr;
    std::string m_inferiorStr;

    TextMapperPtr m_rightMapper;
    TextMapperPtr m_leftMapper;
    TextMapperPtr m_topMapper;
    TextMapperPtr m_bottomMapper;
    TextActorPtr m_right;
    TextActorPtr m_left;
    TextActorPtr m_top;
    TextActorPtr m_bottom;
};

//------------------------------------------------------------------------------

ImageSliceOrientationText::ImageSliceOrientationText() throw() : m_pimpl( new ImageSliceOrientationTextPImpl )
{
}

//------------------------------------------------------------------------------

ImageSliceOrientationText::~ImageSliceOrientationText() throw()
{
}

//------------------------------------------------------------------------------

void ImageSliceOrientationText::doStart() throw(fwTools::Failed)
{
    if(m_initialOrientation == "axial")
    {
        this->setOrientation(Z_AXIS);
    }
    else if(m_initialOrientation == "frontal")
    {
        this->setOrientation(Y_AXIS);
    }
    else // sagittal
    {
        this->setOrientation(X_AXIS);
    }

    if(!m_locations.empty())
    {
        // this->getRenderer()->AddActor2D(m_pimpl->m_right);
        this->addToRenderer( m_pimpl->m_right );
        this->addToRenderer( m_pimpl->m_left );
        this->addToRenderer( m_pimpl->m_top );
        this->addToRenderer( m_pimpl->m_bottom );
    }
    this->doUpdate();
}

//------------------------------------------------------------------------------

void ImageSliceOrientationText::doStop() throw(fwTools::Failed)
{
    this->removeAllPropFromRenderer();
}

//------------------------------------------------------------------------------

void ImageSliceOrientationText::doSwap() throw(fwTools::Failed)
{
    this->doStop();
    this->doStart();
}

//------------------------------------------------------------------------------

void ImageSliceOrientationText::doUpdate() throw(::fwTools::Failed)
{
    m_pimpl->setText(m_locations);
    m_pimpl->setOrientation(m_orientation);
}

//------------------------------------------------------------------------------

void ImageSliceOrientationText::setOrientation( Orientation orientation )
{
    this->::fwComEd::helper::MedicalImageAdaptor::setOrientation(orientation);
    m_pimpl->setOrientation(orientation);
}

//------------------------------------------------------------------------------

void ImageSliceOrientationText::doReceive(::fwServices::ObjectMsg::csptr msg) throw(::fwTools::Failed)
{
    if ( msg->hasEvent( ::fwComEd::ImageMsg::CHANGE_SLICE_TYPE ))
    {
        ::fwData::Object::csptr cObjInfo = msg->getDataInfo( ::fwComEd::ImageMsg::CHANGE_SLICE_TYPE );
        ::fwData::Object::sptr objInfo   = ::boost::const_pointer_cast< ::fwData::Object > ( cObjInfo );
        ::fwData::Composite::sptr info   = ::fwData::Composite::dynamicCast ( objInfo );

        int fromSliceType = ::fwData::Integer::dynamicCast( info->getContainer()["fromSliceType"] )->value();
        int toSliceType   = ::fwData::Integer::dynamicCast( info->getContainer()["toSliceType"] )->value();

        if( toSliceType == static_cast<int>(m_orientation) )
        {
            this->setOrientation( static_cast< Orientation >( fromSliceType ));
        }
        else if(fromSliceType == static_cast<int>(m_orientation))
        {
            this->setOrientation( static_cast< Orientation >( toSliceType ));
        }
    }
    else if (msg->hasEvent(::fwComEd::ImageMsg::BUFFER)
             || msg->hasEvent(::fwComEd::ImageMsg::NEW_IMAGE)
             ||msg->hasEvent(::fwComEd::ImageMsg::MODIFIED))
    {
        this->doStop();
        this->doStart();
        this->doUpdate();
    }
}

//------------------------------------------------------------------------------

void ImageSliceOrientationText::configuring() throw(fwTools::Failed)
{

    ::fwServices::IService::ConfigType srvconfig = this->getConfigTree().get_child("config");

    m_locations = srvconfig.get("locations","");

    // R,L,A,P,S,I, right, left, anterior, posterior, superior, inferior, referenced by :
    // http://en.wikipedia.org/wiki/Anatomical_terms_of_location#Human_anatomy
    m_locations = (m_locations == "default") ? "R,L,A,P,S,I" : m_locations;

    m_initialOrientation = srvconfig.get("initialOrientation","axial");
    SLM_TRACE("initialOrientation " + m_initialOrientation);
}

//------------------------------------------------------------------------------


} //namespace visuVTKAdaptor

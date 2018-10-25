/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuVTKAdaptor/SImageSliceOrientationText.hpp"

#include <fwCom/Slot.hpp>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwData/Image.hpp>

#include <fwDataTools/fieldHelper/MedicalImageHelpers.hpp>

#include <fwServices/macros.hpp>

#include <boost/algorithm/string.hpp>

#include <vtkActor2D.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkTextMapper.h>
#include <vtkTextProperty.h>

fwServicesRegisterMacro( ::fwRenderVTK::IAdaptor, ::visuVTKAdaptor::SImageSliceOrientationText);

namespace visuVTKAdaptor
{

class ImageSliceOrientationTextPImpl
{
public:
    typedef vtkSmartPointer<vtkActor2D> TextActorPtr;
    typedef vtkSmartPointer<vtkTextMapper> TextMapperPtr;

    //------------------------------------------------------------------------------

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

    //------------------------------------------------------------------------------

    void setText(const std::string& str)
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

            m_rightStr     = (locations.size() > 0) ? locations[0] : "";
            m_leftStr      = (locations.size() > 1) ? locations[1] : "";
            m_anteriorStr  = (locations.size() > 2) ? locations[2] : "";
            m_porteriorStr = (locations.size() > 3) ? locations[3] : "";
            m_superiorStr  = (locations.size() > 4) ? locations[4] : "";
            m_inferiorStr  = (locations.size() > 5) ? locations[5] : "";
        }

    }

    //------------------------------------------------------------------------------

    void setOrientation( ::fwDataTools::helper::MedicalImage::Orientation orientation )
    {
        switch (orientation)
        {
            case ::fwDataTools::helper::MedicalImage::X_AXIS: // Sagittal
                m_rightMapper->SetInput(m_anteriorStr.c_str());
                m_leftMapper->SetInput(m_porteriorStr.c_str());
                m_topMapper->SetInput(m_superiorStr.c_str());
                m_bottomMapper->SetInput(m_inferiorStr.c_str());
                break;
            case ::fwDataTools::helper::MedicalImage::Y_AXIS: // Frontal
                m_rightMapper->SetInput(m_leftStr.c_str());
                m_leftMapper->SetInput(m_rightStr.c_str());
                m_topMapper->SetInput(m_superiorStr.c_str());
                m_bottomMapper->SetInput(m_inferiorStr.c_str());
                break;
            case ::fwDataTools::helper::MedicalImage::Z_AXIS: // Axial
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

static const ::fwCom::Slots::SlotKeyType s_UPDATE_SLICE_TYPE_SLOT = "updateSliceType";

static const ::fwServices::IService::KeyType s_IMAGE_INPUT = "image";

//------------------------------------------------------------------------------

SImageSliceOrientationText::SImageSliceOrientationText() noexcept :
    m_pimpl( new ImageSliceOrientationTextPImpl )
{
    newSlot(s_UPDATE_SLICE_TYPE_SLOT, &SImageSliceOrientationText::updateSliceType, this);
}

//------------------------------------------------------------------------------

SImageSliceOrientationText::~SImageSliceOrientationText() noexcept
{
}

//------------------------------------------------------------------------------

void SImageSliceOrientationText::starting()
{
    this->initialize();

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
    this->updating();
}

//------------------------------------------------------------------------------

void SImageSliceOrientationText::stopping()
{
    this->removeAllPropFromRenderer();
}

//------------------------------------------------------------------------------

void SImageSliceOrientationText::updating()
{
    m_pimpl->setText(m_locations);
    m_pimpl->setOrientation(m_orientation);

    this->requestRender();
}

//------------------------------------------------------------------------------

void SImageSliceOrientationText::setOrientation( Orientation orientation )
{
    this->::fwDataTools::helper::MedicalImage::setOrientation(orientation);
    m_pimpl->setOrientation(orientation);
}

//-----------------------------------------------------------------------------

void SImageSliceOrientationText::updateSliceType(int from, int to)
{
    if( to == static_cast<int>(m_orientation) )
    {
        this->setOrientation( static_cast< Orientation >( from ));
    }
    else if(from == static_cast<int>(m_orientation))
    {
        this->setOrientation( static_cast< Orientation >( to ));
    }
}

//------------------------------------------------------------------------------

void SImageSliceOrientationText::configuring()
{
    this->configureParams();

    const ConfigType srvconfig = this->getConfigTree().get_child("config");

    m_locations = srvconfig.get("locations", "");

    // R,L,A,P,S,I, right, left, anterior, posterior, superior, inferior, referenced by :
    // http://en.wikipedia.org/wiki/Anatomical_terms_of_location#Human_anatomy
    m_locations = (m_locations == "default") ? "R,L,A,P,S,I" : m_locations;

    m_initialOrientation = srvconfig.get("initialOrientation", "axial");
    SLM_TRACE("initialOrientation " + m_initialOrientation);
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SImageSliceOrientationText::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push(s_IMAGE_INPUT, ::fwData::Image::s_MODIFIED_SIG, s_UPDATE_SLOT);
    connections.push(s_IMAGE_INPUT, ::fwData::Image::s_SLICE_TYPE_MODIFIED_SIG, s_UPDATE_SLICE_TYPE_SLOT);
    connections.push(s_IMAGE_INPUT, ::fwData::Image::s_BUFFER_MODIFIED_SIG, s_UPDATE_SLOT);

    return connections;
}

//------------------------------------------------------------------------------

} //namespace visuVTKAdaptor

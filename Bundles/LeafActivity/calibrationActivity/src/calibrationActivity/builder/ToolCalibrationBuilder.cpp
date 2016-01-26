/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#include "calibrationActivity/builder/ToolCalibrationBuilder.hpp"

#include <fwActivities/builder/registry/macros.hpp>

#include <fwData/Composite.hpp>
#include <fwData/Object.hpp>
#include <fwData/Vector.hpp>
#include <fwData/String.hpp>

#include <fwMedData/Study.hpp>

#include <fwGui/dialog/InputDialog.hpp>

#include <set>

namespace calibrationActivity
{
namespace builder
{

fwActivitiesBuilderRegisterMacro( ::calibrationActivity::builder::ToolCalibrationBuilder,
                                  "::calibrationActivity::builder::ToolCalibrationBuilder");

//-----------------------------------------------------------------------------

ToolCalibrationBuilder::ToolCalibrationBuilder(::fwActivities::IBuilder::Key key) :
    fwActivities::builder::ActivitySeriesInitData(key)
{
}

//-----------------------------------------------------------------------------

ToolCalibrationBuilder::~ToolCalibrationBuilder()
{
}

//-----------------------------------------------------------------------------

::fwMedData::ActivitySeries::sptr ToolCalibrationBuilder::buildData(
    const ::fwActivities::registry::ActivityInfo& activityInfo,
    ::fwData::Vector::sptr currentSelection ) const
{
    ::fwMedData::ActivitySeries::sptr actSeries =
        this->ActivitySeriesInitData::buildData(activityInfo, currentSelection);
    ::fwData::Composite::sptr dataActivity = actSeries->getData();

    ::fwGui::dialog::InputDialog::sptr dial = ::fwGui::dialog::InputDialog::New();

    std::string ids = dial->showInputDialog("Choose Tag Ids ", "Detected Tag Ids (seperated by ',')","101");

    ::fwData::String::sptr tagsId = ::fwData::String::New(ids);


    (*dataActivity)["tagsIdString"] = tagsId;


    return actSeries;


}

//-----------------------------------------------------------------------------

} // namespace builder
} // namespace calibrationActivity

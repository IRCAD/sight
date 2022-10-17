/************************************************************************
 *
 * Copyright (C) 2022 IRCAD France
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#pragma once

#include "modules/ui/qt/config.hpp"

#include <data/ImageSeries.hpp>
#include <data/ModelSeries.hpp>
#include <data/SeriesSet.hpp>

#include <service/IController.hpp>

namespace sight::module::ui::qt::series
{

/**
 * @brief This service is used to select a series from a series set and then extract it to the appropriate typed data.
 *
 * This service is a work in progress and will be updated in Sight 22.0 to pop-up a dialog to let the user select the
 * series.
 *
 * The output objects must be marked as "deferred" in the XML configuration.
 *
 * @section Signals Signals
 * - \b imageSelected(): Emitted when an image is selected.
 * - \b modelSelected(): Emitted when a model is selected.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
        <service uid="..." type="sight::module::ui::qt::series::SSelectDialog" >
           <in key="seriesSet" uid="..."/>
           <out key="modelSeries" uid="..."/>
           <out key="imageSeries" uid="..."/>
           <out key="image" uid="..."/>
        </service>
       @endcode
 * @subsection Input Input
 * - \b seriesSet [sight::data::SeriesSet]: database series from which the series extracted.
 *
 * @subsection Output Output
 * - \b modelSeries [sight::data::ModelSeries]: output model series extracted from the database series.
 * - \b imageSeries [sight::data::ImageSeries]: output image series extracted from the database series.
 * - \b image [sight::data::Image]: output image extracted from the database series.
 */
class MODULE_UI_QT_CLASS_API SSelectDialog : public service::IController
{
public:

    /// Type of signal sent when a screen is selected.
    using SelectedSignalType = core::com::Signal<void ()>;

    static const core::com::Signals::SignalKeyType s_IMAGE_SELECTED_SIG;
    static const core::com::Signals::SignalKeyType s_MODEL_SELECTED_SIG;

    SIGHT_DECLARE_SERVICE(SSelectDialog, service::IController);

    /// Constructor
    MODULE_UI_QT_API SSelectDialog();

    /// Destructor
    MODULE_UI_QT_API ~SSelectDialog() override = default;

protected:

    /// Does nothing
    MODULE_UI_QT_API void starting() override;

    /// Configure the service
    MODULE_UI_QT_API void configuring() override;

    /// Does nothing
    MODULE_UI_QT_API void stopping() override;

    /// Extract the object(s)
    MODULE_UI_QT_API void updating() override;

private:

    sight::data::ptr<sight::data::SeriesSet, sight::data::Access::in> m_series_set {this, "seriesSet"};
    sight::data::ptr<sight::data::ModelSeries, sight::data::Access::out> m_modelSeries {this, "modelSeries"};
    sight::data::ptr<sight::data::ImageSeries, sight::data::Access::out> m_imageSeries {this, "imageSeries"};
    sight::data::ptr<sight::data::Image, sight::data::Access::out> m_image {this, "image"};
};

} // namespace sight::module::ui::qt::series

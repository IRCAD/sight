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

#include "SSelectDialog.hpp"

#include <core/com/Signal.hxx>

namespace sight::module::ui::qt::series
{

const core::com::Signals::SignalKeyType SSelectDialog::s_IMAGE_SELECTED_SIG = "imageSelected";
const core::com::Signals::SignalKeyType SSelectDialog::s_MODEL_SELECTED_SIG = "modelSelected";

//------------------------------------------------------------------------------

SSelectDialog::SSelectDialog()
{
    newSignal<SelectedSignalType>(s_IMAGE_SELECTED_SIG);
    newSignal<SelectedSignalType>(s_MODEL_SELECTED_SIG);
}

//------------------------------------------------------------------------------

void SSelectDialog::configuring()
{
}

//------------------------------------------------------------------------------

void SSelectDialog::starting()
{
}

//-----------------------------------------------------------------------------

void SSelectDialog::updating()
{
    const auto seriesDB = m_seriesDB.lock();
    if(seriesDB == nullptr)
    {
        SIGHT_THROW_EXCEPTION(sight::data::Exception("Missing input database series"));
    }

    auto series = seriesDB->getContainer();
    if(series.empty())
    {
        SIGHT_THROW_EXCEPTION(sight::data::Exception("The database series is empty, nothing can be extracted."));
    }

    if(!series.empty())
    {
        // TODO: Prompt later to select the element to extract, now just take the first element
        SIGHT_INFO(
            "[SSelectDialog] Extracting the first element of the seriesBD, future development will prompt"
            " the user to pick a series"
        );
        auto firstElement = series[0];
        auto modelSeries  = sight::data::ModelSeries::dynamicCast(firstElement);
        if(modelSeries)
        {
            m_modelSeries = modelSeries;
            auto sig = this->signal<data::Object::ModifiedSignalType>(s_MODEL_SELECTED_SIG);
            sig->asyncEmit();
        }
        else
        {
            auto imageSeries = sight::data::ImageSeries::dynamicCast(firstElement);
            if(imageSeries)
            {
                m_imageSeries = imageSeries;
                auto image = imageSeries->getImage();

                m_image = image;
                auto sig = this->signal<data::Object::ModifiedSignalType>(s_IMAGE_SELECTED_SIG);
                sig->asyncEmit();
            }
        }
    }
}

//-----------------------------------------------------------------------------

void SSelectDialog::stopping()
{
}

//-----------------------------------------------------------------------------

} // namespace sight::module::ui::qt::series

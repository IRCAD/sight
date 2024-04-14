/************************************************************************
 *
 * Copyright (C) 2018-2023 IRCAD France
 * Copyright (C) 2018-2020 IHU Strasbourg
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

// cspell:ignore NOLINTNEXTLINE

#include <core/base.hpp>

#include <itkOutputWindow.h>

/**
 * @brief Outputs ITK messages to the sight log.
 */
// NOLINTNEXTLINE(cppcoreguidelines-virtual-class-destructor): used via pointers
class itk_logger : public ::itk::OutputWindow
{
public:

    using self_t    = itk_logger;
    using pointer_t = ::itk::SmartPointer<self_t>;

    itkTypeMacro(ItkLogger, ::itk::Object);

    static pointer_t New();

    void DisplayText(const char* _txt) override;
    void DisplayErrorText(const char* _txt) override;
    void DisplayWarningText(const char* _txt) override;
    void DisplayDebugText(const char* _txt) override;

private:

    itk_logger();
    ~itk_logger() override;
};

//------------------------------------------------------------------------------

itk_logger::pointer_t itk_logger::New()
{
    return {new itk_logger()};
}

//------------------------------------------------------------------------------

itk_logger::itk_logger()
= default;

//------------------------------------------------------------------------------

itk_logger::~itk_logger()
= default;

//------------------------------------------------------------------------------

void itk_logger::DisplayText(const char* _txt)
{
    SIGHT_INFO("[ITK]: " << _txt);
}

//------------------------------------------------------------------------------

void itk_logger::DisplayErrorText(const char* _txt)
{
    SIGHT_ERROR("[ITK]: " << _txt);
}

//------------------------------------------------------------------------------

void itk_logger::DisplayWarningText(const char* _txt)
{
    SIGHT_WARN("[ITK]: " << _txt);
}

//------------------------------------------------------------------------------

void itk_logger::DisplayDebugText(const char* _txt)
{
    SIGHT_DEBUG("[ITK]: " << _txt);
}

//------------------------------------------------------------------------------

struct logger_instantiator
{
    logger_instantiator()
    {
        auto logger = itk_logger::New();
        ::itk::OutputWindow::SetInstance(logger);
        logger->Delete();
    }
};

//------------------------------------------------------------------------------

static logger_instantiator instantiator;

/************************************************************************
 *
 * Copyright (C) 2018-2022 IRCAD France
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
class ItkLogger : public ::itk::OutputWindow
{
public:

    using Self    = ItkLogger;
    using Pointer = ::itk::SmartPointer<Self>;

    itkTypeMacro(ItkLogger, ::itk::Object);

    static Pointer New();

    void DisplayText(const char* _txt) override;
    void DisplayErrorText(const char* _txt) override;
    void DisplayWarningText(const char* _txt) override;
    void DisplayDebugText(const char* _txt) override;

private:

    ItkLogger();
    ~ItkLogger() override;
};

//------------------------------------------------------------------------------

ItkLogger::Pointer ItkLogger::New()
{
    return {new ItkLogger()};
}

//------------------------------------------------------------------------------

ItkLogger::ItkLogger()
= default;

//------------------------------------------------------------------------------

ItkLogger::~ItkLogger()
= default;

//------------------------------------------------------------------------------

void ItkLogger::DisplayText(const char* _txt)
{
    SIGHT_INFO("[ITK]: " << _txt);
}

//------------------------------------------------------------------------------

void ItkLogger::DisplayErrorText(const char* _txt)
{
    SIGHT_ERROR("[ITK]: " << _txt);
}

//------------------------------------------------------------------------------

void ItkLogger::DisplayWarningText(const char* _txt)
{
    SIGHT_WARN("[ITK]: " << _txt);
}

//------------------------------------------------------------------------------

void ItkLogger::DisplayDebugText(const char* _txt)
{
    SIGHT_DEBUG("[ITK]: " << _txt);
}

//------------------------------------------------------------------------------

struct LoggerInstantiator
{
    LoggerInstantiator()
    {
        ItkLogger::Pointer logger = ItkLogger::New();
        ::itk::OutputWindow::SetInstance(logger);
        logger->Delete();
    }
};

//------------------------------------------------------------------------------

static LoggerInstantiator instantiator;

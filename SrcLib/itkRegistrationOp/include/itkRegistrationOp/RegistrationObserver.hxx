/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __ITKREGISTRATIONOP_REGISTRATIONOBSERVER_HXX__
#define __ITKREGISTRATIONOP_REGISTRATIONOBSERVER_HXX__

#include <fwGui/dialog/ProgressDialog.hpp>

#include <itkCommand.h>

/**
 * @brief Observer reporting registration progress.
 */
template< typename OptimizerType >
class RegistrationObserver : public ::itk::Command
{
public:
    typedef  RegistrationObserver Self;
    typedef ::itk::Command Superclass;
    typedef ::itk::SmartPointer<Self>   Pointer;
    itkNewMacro( Self );

    /// Command to be executed. Updates the progress bar.
    void Execute(::itk::Object* caller, const ::itk::EventObject& event) override;

    /// Const overload of the above method.
    void Execute(const ::itk::Object* object, const ::itk::EventObject& event) override;

    /// Set the maximum number of steps allowed to the optimizer.
    inline void setMaxIterations(unsigned long maxIters);

    /// True if optimization canceled on user request.
    inline bool forceStopped() const;

private:

    /// Constructor, initializes progress dialog and sets the user cancel callback.
    RegistrationObserver();

    /// Displays registration progress.
    ::fwGui::dialog::ProgressDialog m_dialog;

    /// Maximum number of steps allowed to the optimizer. Used to compute progress.
    unsigned long m_maxIters;

    /// Set to true when the user asks to cancel registration.
    bool m_stop;
};

//------------------------------------------------------------------------------

template<typename OptimizerType>
void RegistrationObserver<OptimizerType>::Execute(itk::Object* caller, const itk::EventObject& event)
{
    {
        if(m_stop)
        {
            OptimizerType* optimizer = dynamic_cast< OptimizerType* >( caller );

            optimizer->StopOptimization();
        }

        Execute( (const itk::Object*)caller, event);
    }
}

//------------------------------------------------------------------------------

template<typename OptimizerType>
void RegistrationObserver<OptimizerType>::Execute(const itk::Object* object, const itk::EventObject& event)
{
    {
        if( ::itk::IterationEvent().CheckEvent( &event ) )
        {
            const OptimizerType* optimizer = dynamic_cast< const OptimizerType* >( object );

            const unsigned int itNum = static_cast<unsigned int>(optimizer->GetCurrentIteration()) + 1;

            std::string msg = "Number of iterations : " + std::to_string(itNum);
            m_dialog(static_cast<float>(itNum)/static_cast<float>(m_maxIters), msg);
            m_dialog.setMessage(msg);

            OSLM_DEBUG("Number of iterations : " << itNum);
            OSLM_DEBUG("Current value : " << optimizer->GetValue());
            OSLM_DEBUG("Current parameters : " << optimizer->GetCurrentPosition() );
        }
    }
}

//------------------------------------------------------------------------------

template<typename OptimizerType>
void RegistrationObserver<OptimizerType>::setMaxIterations(unsigned long maxIters)
{
    m_maxIters = maxIters;
}

//------------------------------------------------------------------------------

template<typename OptimizerType>
bool RegistrationObserver<OptimizerType>::forceStopped() const
{
    return m_stop;
}

//------------------------------------------------------------------------------

template<typename OptimizerType>
RegistrationObserver<OptimizerType>::RegistrationObserver() :
    m_dialog("Automatic Registration", "Registring, please be patient."),
    m_stop(false)
{
    m_dialog.setCancelCallback([this]()
    {
        this->m_stop = true;
    });
}

#endif // __ITKREGISTRATIONOP_REGISTRATIONOBSERVER_HXX__


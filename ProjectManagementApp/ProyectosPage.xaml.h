#pragma once

#include "ProyectosPage.g.h"

namespace winrt::ProjectManagementApp::implementation
{
    struct ProyectosPage : ProyectosPageT<ProyectosPage>
    {
        ProyectosPage()
        {
            // Xaml objects should not call InitializeComponent during construction.
            // See https://github.com/microsoft/cppwinrt/tree/master/nuget#initializecomponent
        }
    };
}

namespace winrt::ProjectManagementApp::factory_implementation
{
    struct ProyectosPage : ProyectosPageT<ProyectosPage, implementation::ProyectosPage>
    {
    };
}

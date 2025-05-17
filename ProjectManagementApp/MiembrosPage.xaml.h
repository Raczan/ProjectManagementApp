#pragma once

#include "MiembrosPage.g.h"

namespace winrt::ProjectManagementApp::implementation
{
    struct MiembrosPage : MiembrosPageT<MiembrosPage>
    {
        MiembrosPage()
        {
            // Xaml objects should not call InitializeComponent during construction.
            // See https://github.com/microsoft/cppwinrt/tree/master/nuget#initializecomponent
        }
    };
}

namespace winrt::ProjectManagementApp::factory_implementation
{
    struct MiembrosPage : MiembrosPageT<MiembrosPage, implementation::MiembrosPage>
    {
    };
}

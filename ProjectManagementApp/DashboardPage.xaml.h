#pragma once

#include "DashboardPage.g.h"

namespace winrt::ProjectManagementApp::implementation
{
    struct DashboardPage : DashboardPageT<DashboardPage>
    {
        DashboardPage()
        {
            // Xaml objects should not call InitializeComponent during construction.
            // See https://github.com/microsoft/cppwinrt/tree/master/nuget#initializecomponent
        }
    };
}

namespace winrt::ProjectManagementApp::factory_implementation
{
    struct DashboardPage : DashboardPageT<DashboardPage, implementation::DashboardPage>
    {
    };
}

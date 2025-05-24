#pragma once

#include "DashboardPage.g.h"
#include "db/sqlite3.h"

namespace winrt::ProjectManagementApp::implementation
{
    struct DashboardPage : DashboardPageT<DashboardPage>
    {
        DashboardPage()
        {
            // Xaml objects should not call InitializeComponent during construction.
            // See https://github.com/microsoft/cppwinrt/tree/master/nuget#initializecomponent
        }

        void Page_Loaded(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void LoadDashboardData();
    };
}

namespace winrt::ProjectManagementApp::factory_implementation
{
    struct DashboardPage : DashboardPageT<DashboardPage, implementation::DashboardPage>
    {
    };
}
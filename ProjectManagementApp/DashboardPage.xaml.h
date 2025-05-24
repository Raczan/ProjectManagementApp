#pragma once

#include "DashboardPage.g.h"
#include "db/sqlite3.h"
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Microsoft.UI.Xaml.Data.h>

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
        void LoadUserActivities();
        void LoadUserProjects();
        void LoadTeamMembers();

    private:
        sqlite3* db = nullptr;
        int currentUserId = 1;
        winrt::Windows::Foundation::Collections::IObservableVector<winrt::Windows::Foundation::IInspectable> activitiesCollection = nullptr;
        winrt::Windows::Foundation::Collections::IObservableVector<winrt::Windows::Foundation::IInspectable> projectsCollection = nullptr;
        winrt::Windows::Foundation::Collections::IObservableVector<winrt::Windows::Foundation::IInspectable> membersCollection = nullptr;
    };

    struct StatusToColorConverter : winrt::implements<StatusToColorConverter, Microsoft::UI::Xaml::Data::IValueConverter>
    {
        winrt::Windows::Foundation::IInspectable Convert(winrt::Windows::Foundation::IInspectable const& value, winrt::Windows::UI::Xaml::Interop::TypeName const&, winrt::Windows::Foundation::IInspectable const&, winrt::hstring const&)
        {
            auto status = winrt::unbox_value<winrt::hstring>(value);
            if (status == L"Pendiente") return winrt::box_value(L"#F57C00");
            if (status == L"En Progreso") return winrt::box_value(L"#0078D4");
            if (status == L"Finalizado") return winrt::box_value(L"#388E3C");
            return winrt::box_value(L"#666666");
        }
        winrt::Windows::Foundation::IInspectable ConvertBack(winrt::Windows::Foundation::IInspectable const&, winrt::Windows::UI::Xaml::Interop::TypeName const&, winrt::Windows::Foundation::IInspectable const&, winrt::hstring const&) { return nullptr; }
    };
}

namespace winrt::ProjectManagementApp::factory_implementation
{
    struct DashboardPage : DashboardPageT<DashboardPage, implementation::DashboardPage>
    {
    };
}
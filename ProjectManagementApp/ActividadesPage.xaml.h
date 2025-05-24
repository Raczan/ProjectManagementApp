#pragma once

#include "ActividadesPage.g.h"
#include "db/sqlite3.h"
#include <winrt/Windows.Foundation.Collections.h>

namespace winrt::ProjectManagementApp::implementation
{
    struct ActividadesPage : ActividadesPageT<ActividadesPage>
    {
        ActividadesPage()
        {
            // Xaml objects should not call InitializeComponent during construction.
            // See https://github.com/microsoft/cppwinrt/tree/master/nuget#initializecomponent
        }

        void Page_Loaded(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void LoadUserActivities();

    private:
        sqlite3* db = nullptr;
        int currentUserId = 1; // Por ahora hardcodeado, después lo puedes obtener del contexto de usuario
        winrt::Windows::Foundation::Collections::IObservableVector<winrt::Windows::Foundation::IInspectable> activitiesCollection = nullptr;

        void openDatabase();
        void closeDatabase();
        winrt::hstring getStatusColor(const std::string& status);
        winrt::hstring getStatusBackgroundColor(const std::string& status);
        winrt::hstring getStatusTextColor(const std::string& status);
    };
}

namespace winrt::ProjectManagementApp::factory_implementation
{
    struct ActividadesPage : ActividadesPageT<ActividadesPage, implementation::ActividadesPage>
    {
    };
}
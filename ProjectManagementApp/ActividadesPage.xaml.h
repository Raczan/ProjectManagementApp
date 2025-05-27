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

        }

        void Page_Loaded(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void LoadUserActivities();

    private:
        sqlite3* db = nullptr;
        int currentUserId = 1;
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
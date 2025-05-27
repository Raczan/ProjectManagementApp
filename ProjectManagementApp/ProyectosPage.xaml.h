#pragma once

#include "ProyectosPage.g.h"
#include "db/sqlite3.h"
#include <winrt/Windows.Foundation.Collections.h>

namespace winrt::ProjectManagementApp::implementation
{
    struct ProyectosPage : ProyectosPageT<ProyectosPage>
    {
        ProyectosPage()
        {
            // Xaml objects should not call InitializeComponent during construction.
            // See https://github.com/microsoft/cppwinrt/tree/master/nuget#initializecomponent
        }

        void Page_Loaded(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void LoadProjectActivities();
        void SetProjectId(int projectId);
        void OnNavigatedTo(Microsoft::UI::Xaml::Navigation::NavigationEventArgs const& e);

    private:
        sqlite3* db = nullptr;
        int currentProjectId = 0;
        winrt::hstring projectName = L"";
        winrt::Windows::Foundation::Collections::IObservableVector<winrt::Windows::Foundation::IInspectable> activitiesCollection = nullptr;

        void openDatabase();
        void closeDatabase();
        void loadProjectInfo();
        winrt::hstring getStatusColor(const std::string& status);
        winrt::hstring getStatusBackgroundColor(const std::string& status);
        winrt::hstring getStatusTextColor(const std::string& status);
        winrt::hstring getPriorityBackgroundColor(const std::string& priority);
        winrt::hstring getPriorityTextColor(const std::string& priority);
    };
}

namespace winrt::ProjectManagementApp::factory_implementation
{
    struct ProyectosPage : ProyectosPageT<ProyectosPage, implementation::ProyectosPage>
    {
    };
}
#pragma once

#include "MainPage.g.h"
#include "db/sqlite3.h"

namespace winrt::ProjectManagementApp::implementation
{
    struct MainPage : MainPageT<MainPage>
    {
    private:
        void openDashboardPage();
        void openActividadesPage();
        void openMiembrosPage();
		void openProyectosPage();
		sqlite3* db;
		void openDatabase();
        void closeDatabase();
        int usuario_actual_id = 1;
        void cargarUsuarioActual();
    public:
        MainPage()
        {
            // Xaml objects should not call InitializeComponent during construction.
            // See https://github.com/microsoft/cppwinrt/tree/master/nuget#initializecomponent
        }
        void Page_Loaded(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void Page_Unloaded(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void NavigationView_ItemInvoked(winrt::Microsoft::UI::Xaml::Controls::NavigationView const& sender, winrt::Microsoft::UI::Xaml::Controls::NavigationViewItemInvokedEventArgs const& args);
        void mainFrame_Navigated(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Navigation::NavigationEventArgs const& e);
        void nav_BackRequested(winrt::Microsoft::UI::Xaml::Controls::NavigationView const& sender, winrt::Microsoft::UI::Xaml::Controls::NavigationViewBackRequestedEventArgs const& args);
        void LoadProjects();
    };
}

namespace winrt::ProjectManagementApp::factory_implementation
{
    struct MainPage : MainPageT<MainPage, implementation::MainPage>
    {
    };
}

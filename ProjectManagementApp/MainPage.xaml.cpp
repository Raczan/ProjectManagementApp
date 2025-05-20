#include "pch.h"
#include "MainPage.xaml.h"
#if __has_include("MainPage.g.cpp")
#include "MainPage.g.cpp"
#endif
#include "winrt/Windows.UI.Xaml.Interop.h"
#include <winrt/Windows.Storage.h>

using namespace winrt::Microsoft::UI::Xaml::Controls;
using namespace winrt;
using namespace Microsoft::UI::Xaml;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::ProjectManagementApp::implementation
{
    void MainPage::Page_Loaded(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e)
    {
        LoadProjects();
		openDatabase();
    }

    void MainPage::LoadProjects()
    {
        try {
            auto projectsNavItem = ProjectsNavItem();

            if (projectsNavItem == nullptr) {
                return;
            }

            projectsNavItem.MenuItems().Clear();
            std::vector<hstring> projectsList = {
              L"Ventas",
              L"Cobros",
              L"Clientes"
            };

            for (const auto& projectName : projectsList)
            {
                NavigationViewItem item;
                item.Content(box_value(projectName));
                projectsNavItem.MenuItems().Append(item);
            }
        }
        catch (winrt::hresult_error const& ex) {
            OutputDebugString((L"Error en LoadProjects: " + std::wstring(ex.message()) + L"\n").c_str());
        }
        catch (...) {
            OutputDebugString(L"Error desconocido en LoadProjects\n");
        }
    }

    void MainPage::Page_Unloaded(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e)
    {
        closeDatabase();
    }

    void MainPage::NavigationView_ItemInvoked(winrt::Microsoft::UI::Xaml::Controls::NavigationView const& sender, winrt::Microsoft::UI::Xaml::Controls::NavigationViewItemInvokedEventArgs const& args)
    {
        auto container = args.InvokedItemContainer();
        if (container == nullptr)
            return;

        if (container.Tag() != nullptr) {
            try {
                hstring tag = unbox_value<hstring>(container.Tag());

                if (tag == L"Dashboard")
                    openDashboardPage();
                else if (tag == L"Actividades")
                    openActividadesPage();
                else if (tag == L"Miembros")
                    openMiembrosPage();
            }
            catch (...) {
                OutputDebugString(L"Error desconocido en NavigationView_ItemInvoked\n");
            }
        }
    }
    
    void MainPage::mainFrame_Navigated(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Navigation::NavigationEventArgs const& e)
    {
        nav().IsBackEnabled(mainFrame().CanGoBack());
    }

    void MainPage::nav_BackRequested(winrt::Microsoft::UI::Xaml::Controls::NavigationView const& sender, winrt::Microsoft::UI::Xaml::Controls::NavigationViewBackRequestedEventArgs const& args)
    {
        mainFrame().GoBack();
    }

    void MainPage::openDashboardPage()
    {
        mainFrame().Navigate(xaml_typename<DashboardPage>());
    }

    void MainPage::openActividadesPage()
    {
        mainFrame().Navigate(xaml_typename<ActividadesPage>());
    }

    void MainPage::openMiembrosPage()
    {
        mainFrame().Navigate(xaml_typename<MiembrosPage>());
    }

    void MainPage::openProyectosPage()
    {
        mainFrame().Navigate(xaml_typename<ProyectosPage>());
    }

    void MainPage::openDatabase()
    {
        auto localFolder = Windows::Storage::ApplicationData::Current().LocalFolder();
        std::wstring fullPath = std::wstring(localFolder.Path()) + L"\\database.db";
        std::string path = winrt::to_string(fullPath);
        const char* dbFilename = path.c_str();

        std::wstring debugMsg = L"Ubicación de la BD: " + fullPath + L"\n";
        OutputDebugString(debugMsg.c_str());

        int result = sqlite3_open(dbFilename, &db);

        if (result != SQLITE_OK) {
            OutputDebugString(L"Error al abrir la base de datos\n");
        }
        else {
            OutputDebugString(L"Base de datos abierta correctamente\n");
        }
    }
    
    void MainPage::closeDatabase()
    {
		sqlite3_close(db);
    }
}

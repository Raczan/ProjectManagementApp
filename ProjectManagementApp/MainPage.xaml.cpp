#include "pch.h"
#include "MainPage.xaml.h"
#if __has_include("MainPage.g.cpp")
#include "MainPage.g.cpp"
#endif
#include "winrt/Windows.UI.Xaml.Interop.h"
#include <winrt/Windows.Storage.h>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <string>
#include <winrt/Microsoft.UI.Xaml.Media.Imaging.h>
#include <winrt/Windows.Foundation.h>


using namespace winrt::Microsoft::UI::Xaml::Controls;
using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Xaml::Controls;
using namespace Microsoft::UI::Xaml::Navigation;
using namespace Windows::Foundation;

namespace winrt::ProjectManagementApp::implementation
{
    void MainPage::Page_Loaded(IInspectable const&, RoutedEventArgs const&)
    {
        openDatabase();
        cargarUsuarioActual();
        LoadProjects();
        openDashboardPage();
        nav().SelectedItem(nav().MenuItems().GetAt(0));
    }

    void MainPage::Page_Unloaded(IInspectable const&, RoutedEventArgs const&)
    {
        closeDatabase();
    }

    // En MainPage.xaml.cpp, actualiza la función NavigationView_ItemInvoked
    void MainPage::NavigationView_ItemInvoked(NavigationView const& sender, NavigationViewItemInvokedEventArgs const& args)
    {
        auto invokedContainer = args.InvokedItemContainer();
        if (!invokedContainer) return;

        auto tag = invokedContainer.Tag();
        if (!tag) return;

        auto tagString = winrt::unbox_value<winrt::hstring>(tag);

        if (tagString == L"Dashboard")
        {
            openDashboardPage();
        }
        else if (tagString == L"Actividades")
        {
            openActividadesPage();
        }
        else if (tagString == L"Miembros")
        {
            openMiembrosPage();
        }
        else if (tagString == L"Proyectos")
        {
            openProyectosPage();
        }
        else
        {
            // Verificar si es un proyecto específico (el tag sería "proyecto_X" donde X es el ID)
            std::wstring tagStr = tagString.c_str();
            if (tagStr.find(L"proyecto_") == 0)
            {
                try
                {
                    // Extraer el ID del proyecto del tag
                    std::wstring projectIdStr = tagStr.substr(9); // "proyecto_".length() = 9
                    int projectId = std::stoi(projectIdStr);
                    openProyectosPageWithProject(projectId);
                }
                catch (...)
                {
                    // Si hay error parsing el ID, abrir página de proyectos general
                    openProyectosPage();
                }
            }
        }
    }
    
    void MainPage::mainFrame_Navigated(IInspectable const&, NavigationEventArgs const& e)
    {
        nav().IsBackEnabled(mainFrame().CanGoBack());
    }

    void MainPage::nav_BackRequested(NavigationView const&, NavigationViewBackRequestedEventArgs const&)
    {
        if (mainFrame().CanGoBack())
        {
            mainFrame().GoBack();
        }
    }

    void MainPage::openDashboardPage()
    {
        mainFrame().Navigate(xaml_typename<ProjectManagementApp::DashboardPage>());
    }

    void MainPage::openActividadesPage()
    {
        mainFrame().Navigate(xaml_typename<ProjectManagementApp::ActividadesPage>());
    }

    void MainPage::openMiembrosPage()
    {
        mainFrame().Navigate(xaml_typename<ProjectManagementApp::MiembrosPage>());
    }

    void MainPage::openProyectosPage()
    {
        mainFrame().Navigate(xaml_typename<ProjectManagementApp::ProyectosPage>());
    }

    void MainPage::openDatabase()
    {
        try
        {
            auto localFolder = Windows::Storage::ApplicationData::Current().LocalFolder();
            auto localFolderPath = localFolder.Path();
            std::wstring dbPath = std::wstring(localFolderPath) + L"\\projectmanagement.db";

            // Convertir a string para SQLite
            std::string dbPathStr(dbPath.begin(), dbPath.end());

            int result = sqlite3_open(dbPathStr.c_str(), &db);
            if (result != SQLITE_OK)
            {
                // Error al abrir la base de datos
                db = nullptr;
            }
        }
        catch (...)
        {
            db = nullptr;
        }
    }

    void MainPage::closeDatabase()
    {
        if (db)
        {
            sqlite3_close(db);
            db = nullptr;
        }
    }

    void MainPage::cargarUsuarioActual()
    {
        // Establecer usuario ID 1 como predeterminado
        usuario_actual_id = 1;

        // Cargar datos completos del usuario desde la base de datos
        if (db == nullptr) return;

        std::string sql = "SELECT nombre, apellido, departamento, url_imagen_perfil FROM Usuarios WHERE usuario_id = ? LIMIT 1";
        sqlite3_stmt* stmt;

        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
            return;
        }

        sqlite3_bind_int(stmt, 1, usuario_actual_id);

        if (sqlite3_step(stmt) == SQLITE_ROW) {
            const char* nombre = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            const char* apellido = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            const char* departamento = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            const char* url_imagen = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));

            // Actualizar nombre del usuario
            std::string nombreCompleto;
            if (nombre) nombreCompleto += nombre;
            if (apellido) nombreCompleto += " " + std::string(apellido);

            auto userName = this->FindName(L"UserNameText").as<winrt::Microsoft::UI::Xaml::Controls::TextBlock>();
            if (userName && !nombreCompleto.empty()) {
                userName.Text(winrt::to_hstring(nombreCompleto));
            }

            // Actualizar rol/departamento del usuario
            auto userRole = this->FindName(L"UserRoleText").as<winrt::Microsoft::UI::Xaml::Controls::TextBlock>();
            if (userRole && departamento) {
                userRole.Text(winrt::to_hstring(departamento));
            }

            // Cargar imagen de perfil
            if (url_imagen && strlen(url_imagen) > 0) {
                try {
                    auto userPicture = this->FindName(L"UserPicture").as<winrt::Microsoft::UI::Xaml::Controls::PersonPicture>();
                    if (userPicture) {
                        auto bitmapImage = winrt::Microsoft::UI::Xaml::Media::Imaging::BitmapImage();

                        std::wstring wideUrl = std::wstring(winrt::to_hstring(url_imagen).c_str());
                        winrt::Windows::Foundation::Uri uri(wideUrl);
                        bitmapImage.UriSource(uri);
                        userPicture.ProfilePicture(bitmapImage);
                    }
                }
                catch (const winrt::hresult_error& ex) {
                    // Error al cargar imagen - continuar sin imagen
                    OutputDebugStringW((L"Error al cargar imagen: " + ex.message() + L"\n").c_str());
                }
            }
        }

        sqlite3_finalize(stmt);
    }

    void MainPage::LoadProjects()
    {
        if (!db) return;

        // Limpiar elementos existentes del NavigationViewItem de Proyectos
        auto projectsNavItem = ProjectsNavItem();
        projectsNavItem.MenuItems().Clear();

        const char* sql = "SELECT DISTINCT p.proyecto_id, p.nombre FROM Proyectos p LEFT JOIN Actividades a ON p.proyecto_id = a.proyecto_id LEFT JOIN Asignaciones asig ON a.actividad_id = asig.actividad_id WHERE p.responsable_id = ? OR asig.usuario_id = ? ORDER BY p.nombre";

        sqlite3_stmt* stmt;

        if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK)
        {
            sqlite3_bind_int(stmt, 1, usuario_actual_id);
            sqlite3_bind_int(stmt, 2, usuario_actual_id);
            while (sqlite3_step(stmt) == SQLITE_ROW)
            {
                int projectId = sqlite3_column_int(stmt, 0);
                const char* projectName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));

                if (projectName)
                {
                    // Crear NavigationViewItem para cada proyecto
                    auto projectItem = NavigationViewItem();
                    projectItem.Content(winrt::box_value(winrt::to_hstring(projectName)));

                    // Crear tag único para cada proyecto
                    std::wstring tagStr = L"proyecto_" + std::to_wstring(projectId);
                    projectItem.Tag(winrt::box_value(winrt::hstring(tagStr)));

                    // Agregar al elemento Proyectos
                    projectsNavItem.MenuItems().Append(projectItem);
                }
            }
        }
        sqlite3_finalize(stmt);
    }

    void MainPage::openProyectosPageWithProject(int projectId)
    {
        mainFrame().Navigate(winrt::xaml_typename<ProjectManagementApp::ProyectosPage>(), winrt::box_value(projectId));
    }
}
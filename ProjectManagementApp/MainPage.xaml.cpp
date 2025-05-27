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

    void MainPage::NavigationView_ItemInvoked(NavigationView const&, NavigationViewItemInvokedEventArgs const& args)
    {
        auto invokedItem = args.InvokedItem();
        auto invokedItemContainer = args.InvokedItemContainer();

        if (invokedItemContainer != nullptr)
        {
            auto tag = invokedItemContainer.Tag();
            if (tag != nullptr)
            {
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
                else if (tagString.size() > 8 && std::wstring(tagString).substr(0, 8) == L"Project_")
                {
                    auto projectIdStr = std::wstring(tagString).substr(8);
                    try
                    {
                        int projectId = std::stoi(projectIdStr);
                        openProyectosPage();
                    }
                    catch (...)
                    {
                        openProyectosPage();
                    }
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

        // Obtener el NavigationViewItem de Proyectos
        auto projectsNavItem = ProjectsNavItem();

        // Limpiar items existentes
        projectsNavItem.MenuItems().Clear();

        // Consulta SQL para obtener proyectos del usuario actual
        const char* sql = R"(
            SELECT proyecto_id, nombre, estado, prioridad 
            FROM Proyectos 
            WHERE responsable_id = ? 
            ORDER BY nombre COLLATE NOCASE
        )";

        sqlite3_stmt* stmt;
        int result = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);

        if (result == SQLITE_OK)
        {
            // Bind del parámetro usuario_actual_id
            sqlite3_bind_int(stmt, 1, usuario_actual_id);

            // Ejecutar la consulta y procesar resultados
            while (sqlite3_step(stmt) == SQLITE_ROW)
            {
                int projectId = sqlite3_column_int(stmt, 0);
                const char* nombrePtr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
                const char* estadoPtr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
                const char* prioridadPtr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));

                if (nombrePtr)
                {
                    // Convertir de UTF-8 a UTF-16 para WinRT
                    std::string nombreUtf8(nombrePtr);
                    std::string estadoUtf8 = estadoPtr ? std::string(estadoPtr) : "";
                    std::string prioridadUtf8 = prioridadPtr ? std::string(prioridadPtr) : "";

                    // Conversión manual de UTF-8 a UTF-16
                    int sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, nombreUtf8.c_str(), -1, nullptr, 0);
                    std::wstring nombreWide(sizeNeeded - 1, 0);
                    MultiByteToWideChar(CP_UTF8, 0, nombreUtf8.c_str(), -1, &nombreWide[0], sizeNeeded);

                    winrt::hstring projectName{ nombreWide };

                    // Crear NavigationViewItem para el proyecto
                    NavigationViewItem projectItem;
                    projectItem.Content(winrt::box_value(projectName));

                    // Crear tag único para el proyecto
                    winrt::hstring projectTag = L"Project_" + winrt::to_hstring(projectId);
                    projectItem.Tag(winrt::box_value(projectTag));

                    // Agregar al NavigationViewItem de Proyectos
                    projectsNavItem.MenuItems().Append(projectItem);
                }
            }

            sqlite3_finalize(stmt);
        }

        // Si no hay proyectos, agregar un item informativo
        if (projectsNavItem.MenuItems().Size() == 0)
        {
            NavigationViewItem noProjectsItem;
            noProjectsItem.Content(winrt::box_value(L"Sin proyectos asignados"));
            noProjectsItem.IsEnabled(false);
            projectsNavItem.MenuItems().Append(noProjectsItem);
        }
    }
}
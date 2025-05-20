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

    void MainPage::openDatabase() {
        std::wstring appDataPath = winrt::Windows::Storage::ApplicationData::Current().LocalFolder().Path().c_str();
        std::string dbPath = winrt::to_string(appDataPath) + "\\projectmanagement.db";
        OutputDebugStringA(("Intentando abrir/crear base de datos: " + dbPath + "\n").c_str());


        bool dbExists = std::filesystem::exists(dbPath);
        OutputDebugStringA(dbExists ? "Base de datos ya existe\n" : "Base de datos no existe, se creará nueva\n");

        int rc = sqlite3_open(dbPath.c_str(), &db);
        if (rc != SQLITE_OK) {
            const char* errorMsg = sqlite3_errmsg(db);
            std::string error = "ERROR al abrir base de datos: " + std::string(errorMsg) + "\n";
            OutputDebugStringA(error.c_str());
            return;
        }

        OutputDebugStringA("Base de datos abierta correctamente\n");

        if (!dbExists) {
            try {
                wchar_t modulePath[MAX_PATH];
                GetModuleFileNameW(NULL, modulePath, MAX_PATH);
                std::filesystem::path exePath(modulePath);
                std::filesystem::path sqlFilePath = exePath.parent_path() / "projectmanagement.sql";

                std::ifstream sqlFile(sqlFilePath);
                if (!sqlFile.is_open()) {
                    std::string error = "ERROR: No se pudo abrir el archivo SQL\n";
                    OutputDebugStringA(error.c_str());
                    return;
                }

                OutputDebugStringA("Archivo SQL abierto correctamente, leyendo contenido\n");

                std::stringstream buffer;
                buffer << sqlFile.rdbuf();
                std::string sqlScript = buffer.str();
                sqlFile.close();


                char* errorMsg = nullptr;
                OutputDebugStringA("Ejecutando script SQL\n");

                const char* pzTail = sqlScript.c_str();
                sqlite3_stmt* stmt;
                int statementCount = 0;

                while (*pzTail) {
                    rc = sqlite3_prepare_v2(db, pzTail, -1, &stmt, &pzTail);
                    if (rc != SQLITE_OK) {
                        std::string prepareError = "Error preparando sentencia SQL: " + std::string(sqlite3_errmsg(db)) + "\n";
                        OutputDebugStringA(prepareError.c_str());
                        continue;
                    }

                    if (stmt) {
                        statementCount++;
                        rc = sqlite3_step(stmt);
                        if (rc != SQLITE_DONE && rc != SQLITE_ROW) {
                            std::string execError = "Error ejecutando sentencia SQL: " + std::string(sqlite3_errmsg(db)) + "\n";
                            OutputDebugStringA(execError.c_str());
                        }
                        sqlite3_finalize(stmt);
                    }
                }

                OutputDebugStringA(("Inicialización completa. Sentencias ejecutadas: " + std::to_string(statementCount) + "\n").c_str());
            }
            catch (const std::exception& e) {
                std::string error = "Excepción al inicializar la base de datos: " + std::string(e.what()) + "\n";
                OutputDebugStringA(error.c_str());
            }
        }

        cargarUsuarioActual();
    }

    void MainPage::closeDatabase() {
        if (db != nullptr) {
            OutputDebugStringA("Cerrando conexión a base de datos\n");
            sqlite3_close(db);
            db = nullptr;
            OutputDebugStringA("Conexión cerrada\n");
        }
    }

    void MainPage::cargarUsuarioActual() {
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

            std::string nombreCompleto;
            if (nombre) nombreCompleto += nombre;
            if (apellido) nombreCompleto += " " + std::string(apellido);

            auto userName = this->FindName(L"UserNameText").as<winrt::Microsoft::UI::Xaml::Controls::TextBlock>();
            if (userName && !nombreCompleto.empty()) {
                userName.Text(winrt::to_hstring(nombreCompleto));
            }

            auto userRole = this->FindName(L"UserRoleText").as<winrt::Microsoft::UI::Xaml::Controls::TextBlock>();
            if (userRole && departamento) {
                userRole.Text(winrt::to_hstring(departamento));
            }

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
                    OutputDebugStringW((L"Error al cargar imagen: " + ex.message() + L"\n").c_str());
                }
            }
        }

        sqlite3_finalize(stmt);
    }
}

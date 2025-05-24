#include "pch.h"
#include "DashboardPage.xaml.h"
#if __has_include("DashboardPage.g.cpp")
#include "DashboardPage.g.cpp"
#endif
#include "MainPage.xaml.h"
#include <winrt/Windows.Storage.h>

using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Xaml::Controls;

namespace winrt::ProjectManagementApp::implementation
{
    void DashboardPage::Page_Loaded(IInspectable const& sender, RoutedEventArgs const& e)
    {
        LoadDashboardData();
    }

    void DashboardPage::LoadDashboardData()
    {
        sqlite3* db = nullptr;
        sqlite3_stmt* stmt = nullptr;
        int rc;

        // Abrimos la base de datos directamente
        winrt::hstring dbPath = Windows::Storage::ApplicationData::Current().LocalFolder().Path() + L"\\projectmanagement.db";
        std::string dbPathUtf8 = winrt::to_string(dbPath);
        rc = sqlite3_open(dbPathUtf8.c_str(), &db);
        if (rc != SQLITE_OK) {
            return;
        }

        // Consultar la vista dashboard_resumen
        const char* sql = "SELECT * FROM vista_dashboard_resumen";
        rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
        if (rc != SQLITE_OK) {
            sqlite3_close(db);
            return;
        }

        // Ejecutar la consulta
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            int proyectosActivos = sqlite3_column_int(stmt, 0);
            int totalTareas = sqlite3_column_int(stmt, 1);
            int tareasCompletadas = sqlite3_column_int(stmt, 2);
            int tareasProximasVencer = sqlite3_column_int(stmt, 3);

            ActiveProjectsCount().Text(winrt::to_hstring(proyectosActivos));
            TotalActivitiesCount().Text(winrt::to_hstring(totalTareas));
            CompletedActivitiesCount().Text(winrt::to_hstring(tareasCompletadas));
            UpcomingDueCount().Text(winrt::to_hstring(tareasProximasVencer));
        }

        sqlite3_finalize(stmt);
        sqlite3_close(db);
    }
}
#include "pch.h"
#include "DashboardPage.xaml.h"
#if __has_include("DashboardPage.g.cpp")
#include "DashboardPage.g.cpp"
#endif
#include "MainPage.xaml.h"
#include <winrt/Windows.Storage.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <vector>

using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Xaml::Controls;
using namespace Windows::Foundation::Collections;

namespace winrt::ProjectManagementApp::implementation
{
    void DashboardPage::Page_Loaded(IInspectable const& sender, RoutedEventArgs const& e)
    {
        LoadDashboardData();
        LoadUserActivities();
		LoadUserProjects();
        LoadTeamMembers();
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

    void DashboardPage::LoadUserActivities()
    {
        sqlite3* db = nullptr;
        sqlite3_stmt* stmt = nullptr;
        int rc;

        // Abrimos la base de datos
        winrt::hstring dbPath = Windows::Storage::ApplicationData::Current().LocalFolder().Path() + L"\\projectmanagement.db";
        std::string dbPathUtf8 = winrt::to_string(dbPath);
        rc = sqlite3_open(dbPathUtf8.c_str(), &db);
        if (rc != SQLITE_OK) {
            return;
        }

        // Consulta para obtener las actividades del usuario actual
        const char* sql = R"(
            SELECT 
                a.nombre as actividad_nombre,
                p.nombre as proyecto_nombre,
                a.fecha_fin_prevista,
                a.estado
            FROM Actividades a
            INNER JOIN Proyectos p ON a.proyecto_id = p.proyecto_id
            INNER JOIN Asignaciones asig ON a.actividad_id = asig.actividad_id
            WHERE asig.usuario_id = ?
            ORDER BY a.fecha_fin_prevista ASC
            LIMIT 4
        )";

        rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
        if (rc != SQLITE_OK) {
            sqlite3_close(db);
            return;
        }

        // Bind del parámetro usuario_id
        sqlite3_bind_int(stmt, 1, currentUserId);

        // Crear colección observable para las actividades
        activitiesCollection = winrt::single_threaded_observable_vector<IInspectable>();

        // Ejecutar la consulta y procesar resultados
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            // Obtener datos de la consulta
            const char* actividadNombre = (const char*)sqlite3_column_text(stmt, 0);
            const char* proyectoNombre = (const char*)sqlite3_column_text(stmt, 1);
            const char* fechaFin = (const char*)sqlite3_column_text(stmt, 2);
            const char* estado = (const char*)sqlite3_column_text(stmt, 3);

            // Convertir a hstring
            winrt::hstring actividadName = winrt::to_hstring(actividadNombre ? actividadNombre : "");
            winrt::hstring projectName = winrt::to_hstring(proyectoNombre ? proyectoNombre : "");
            winrt::hstring dueDate = winrt::to_hstring(fechaFin ? fechaFin : "");
            winrt::hstring statusText = winrt::to_hstring(estado ? estado : "");

            // Crear objeto para mostrar en la lista usando PropertySet
            Windows::Foundation::Collections::PropertySet activityItem;

            activityItem.Insert(L"ActivityName", winrt::box_value(actividadName));
            activityItem.Insert(L"ProjectName", winrt::box_value(projectName));
            activityItem.Insert(L"DueDate", winrt::box_value(dueDate));
            activityItem.Insert(L"Status", winrt::box_value(statusText));

            // Agregar colores basados en estado
            winrt::hstring bgColor, textColor;
            if (statusText == L"Pendiente") {
                bgColor = L"#FFF8E1"; textColor = L"#F57C00";
            }
            else if (statusText == L"En Progreso") {
                bgColor = L"#E8F5E9"; textColor = L"#388E3C";
            }
            else if (statusText == L"Finalizado") {
                bgColor = L"#F3E5F5"; textColor = L"#7B1FA2";
            }
            else {
                bgColor = L"#F5F5F5"; textColor = L"#666666";
            }


            activityItem.Insert(L"StatusBg", winrt::box_value(bgColor));
            activityItem.Insert(L"StatusColor", winrt::box_value(textColor));

            activitiesCollection.Append(winrt::box_value(activityItem));
        }

        // Asignar la colección al ListView
        ActivitiesList().ItemsSource(activitiesCollection);

        sqlite3_finalize(stmt);
        sqlite3_close(db);
    }

    void DashboardPage::LoadUserProjects()
    {
        sqlite3* db = nullptr;
        sqlite3_stmt* stmt = nullptr;
        int rc;

        winrt::hstring dbPath = Windows::Storage::ApplicationData::Current().LocalFolder().Path() + L"\\projectmanagement.db";
        std::string dbPathUtf8 = winrt::to_string(dbPath);
        rc = sqlite3_open(dbPathUtf8.c_str(), &db);
        if (rc != SQLITE_OK) return;

        const char* sql = R"(
        SELECT DISTINCT
            p.nombre as proyecto_nombre,
            p.fecha_fin_prevista,
            p.estado,
            COUNT(a.actividad_id) as total_actividades
        FROM Proyectos p
        INNER JOIN Actividades a ON p.proyecto_id = a.proyecto_id
        INNER JOIN Asignaciones asig ON a.actividad_id = asig.actividad_id
        WHERE asig.usuario_id = ?
        GROUP BY p.proyecto_id, p.nombre, p.fecha_fin_prevista, p.estado
        ORDER BY p.fecha_fin_prevista ASC
        LIMIT 4
    )";

        rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
        if (rc != SQLITE_OK) {
            sqlite3_close(db);
            return;
        }

        sqlite3_bind_int(stmt, 1, currentUserId);
        projectsCollection = winrt::single_threaded_observable_vector<IInspectable>();

        while (sqlite3_step(stmt) == SQLITE_ROW) {
            const char* proyectoNombre = (const char*)sqlite3_column_text(stmt, 0);
            const char* fechaFin = (const char*)sqlite3_column_text(stmt, 1);
            const char* estado = (const char*)sqlite3_column_text(stmt, 2);
            int totalActividades = sqlite3_column_int(stmt, 3);

            winrt::hstring projectName = winrt::to_hstring(proyectoNombre ? proyectoNombre : "");
            winrt::hstring dueDate = winrt::to_hstring(fechaFin ? fechaFin : "");
            winrt::hstring statusText = winrt::to_hstring(estado ? estado : "");

            Windows::Foundation::Collections::PropertySet projectItem;
            projectItem.Insert(L"ProjectName", winrt::box_value(projectName));
            projectItem.Insert(L"DueDate", winrt::box_value(dueDate));
            projectItem.Insert(L"Status", winrt::box_value(statusText));
            projectItem.Insert(L"ActivityCount", winrt::box_value(winrt::to_hstring(totalActividades) + L" Actividades"));

            // Colores para proyectos
            winrt::hstring bgColor, textColor;
            if (statusText == L"Pendiente") {
                bgColor = L"#FFF8E1"; textColor = L"#F57C00";
            }
            else if (statusText == L"En Progreso") {
                bgColor = L"#E8F5E9"; textColor = L"#388E3C";
            }
            else if (statusText == L"Finalizado") {
                bgColor = L"#F3E5F5"; textColor = L"#7B1FA2";
            }
            else {
                bgColor = L"#F5F5F5"; textColor = L"#666666";
            }

            projectItem.Insert(L"StatusBg", winrt::box_value(bgColor));
            projectItem.Insert(L"StatusColor", winrt::box_value(textColor));

            projectsCollection.Append(winrt::box_value(projectItem));
        }

        ProjectsList().ItemsSource(projectsCollection);

        sqlite3_finalize(stmt);
        sqlite3_close(db);
    }

    void DashboardPage::LoadTeamMembers()
    {
        sqlite3* db = nullptr;
        sqlite3_stmt* stmt = nullptr;
        int rc;

        winrt::hstring dbPath = Windows::Storage::ApplicationData::Current().LocalFolder().Path() + L"\\projectmanagement.db";
        std::string dbPathUtf8 = winrt::to_string(dbPath);
        rc = sqlite3_open(dbPathUtf8.c_str(), &db);
        if (rc != SQLITE_OK) return;

        const char* sql = R"(
            SELECT nombre, apellido, departamento, url_imagen_perfil
            FROM Usuarios 
            WHERE usuario_id != ?
            ORDER BY nombre
            LIMIT 8
        )";

        rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
        if (rc != SQLITE_OK) {
            sqlite3_close(db);
            return;
        }

        sqlite3_bind_int(stmt, 1, currentUserId);
        sqlite3_bind_int(stmt, 2, currentUserId);
        membersCollection = winrt::single_threaded_observable_vector<IInspectable>();

        while (sqlite3_step(stmt) == SQLITE_ROW) {
            const char* nombre = (const char*)sqlite3_column_text(stmt, 0);
            const char* apellido = (const char*)sqlite3_column_text(stmt, 1);
            const char* departamento = (const char*)sqlite3_column_text(stmt, 2);
            const char* urlImagen = (const char*)sqlite3_column_text(stmt, 3);

            winrt::hstring memberName = winrt::to_hstring(nombre ? nombre : "") + L" " + winrt::to_hstring(apellido ? apellido : "");
            winrt::hstring memberRole = winrt::to_hstring(departamento ? departamento : "");
            winrt::hstring profileImage = winrt::to_hstring(urlImagen ? urlImagen : "");

            Windows::Foundation::Collections::PropertySet memberItem;
            memberItem.Insert(L"MemberName", winrt::box_value(memberName));
            memberItem.Insert(L"MemberRole", winrt::box_value(memberRole));
            memberItem.Insert(L"ProfileImageUrl", winrt::box_value(profileImage));

            membersCollection.Append(winrt::box_value(memberItem));
        }

        MembersList().ItemsSource(membersCollection);

        sqlite3_finalize(stmt);
        sqlite3_close(db);
    }
}
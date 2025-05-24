#include "pch.h"
#include "ActividadesPage.xaml.h"
#if __has_include("ActividadesPage.g.cpp")
#include "ActividadesPage.g.cpp"
#endif
#include <winrt/Windows.Storage.h>

using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Xaml::Controls;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;

namespace winrt::ProjectManagementApp::implementation
{
    void ActividadesPage::Page_Loaded(IInspectable const& sender, RoutedEventArgs const& e)
    {
        openDatabase();
        LoadUserActivities();
    }

    void ActividadesPage::openDatabase()
    {
        try
        {
            winrt::hstring dbPath = Windows::Storage::ApplicationData::Current().LocalFolder().Path() + L"\\projectmanagement.db";
            std::string dbPathUtf8 = winrt::to_string(dbPath);

            int result = sqlite3_open(dbPathUtf8.c_str(), &db);

            if (result != SQLITE_OK)
            {
                sqlite3_close(db);
                db = nullptr;
            }
        }
        catch (...)
        {
            db = nullptr;
        }
    }

    void ActividadesPage::closeDatabase()
    {
        if (db)
        {
            sqlite3_close(db);
            db = nullptr;
        }
    }

    void ActividadesPage::LoadUserActivities()
    {
        if (!db) return;

        activitiesCollection = single_threaded_observable_vector<IInspectable>();

        // Query para obtener las actividades del usuario actual
        std::string query = R"(
            SELECT 
                a.actividad_id,
                a.nombre AS activity_name,
                p.nombre AS project_name,
                a.fecha_fin_prevista AS due_date,
                a.estado AS status,
                u.nombre || ' ' || u.apellido AS assignee_name,
                u.url_imagen_perfil AS assignee_image
            FROM Actividades a
            INNER JOIN Proyectos p ON a.proyecto_id = p.proyecto_id
            INNER JOIN Asignaciones asig ON a.actividad_id = asig.actividad_id
            INNER JOIN Usuarios u ON asig.usuario_id = u.usuario_id
            WHERE asig.usuario_id = ?
            ORDER BY 
                CASE a.estado 
                    WHEN 'En Progreso' THEN 1
                    WHEN 'Pendiente' THEN 2 
                    WHEN 'Finalizado' THEN 3
                END,
                a.fecha_fin_prevista ASC
        )";

        sqlite3_stmt* stmt;
        int result = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);

        if (result == SQLITE_OK)
        {
            sqlite3_bind_int(stmt, 1, currentUserId);

            while (sqlite3_step(stmt) == SQLITE_ROW)
            {
                // Crear un objeto para cada actividad
                auto activityMap = single_threaded_map<hstring, IInspectable>();

                // Obtener datos de la consulta
                const char* activityName = (const char*)sqlite3_column_text(stmt, 1);
                const char* projectName = (const char*)sqlite3_column_text(stmt, 2);
                const char* dueDate = (const char*)sqlite3_column_text(stmt, 3);
                const char* status = (const char*)sqlite3_column_text(stmt, 4);
                const char* assigneeName = (const char*)sqlite3_column_text(stmt, 5);
                const char* assigneeImage = (const char*)sqlite3_column_text(stmt, 6);

                // Llenar el mapa con los datos
                activityMap.Insert(L"ActivityName", box_value(to_hstring(activityName ? activityName : "")));
                activityMap.Insert(L"ProjectName", box_value(to_hstring(projectName ? projectName : "")));
                activityMap.Insert(L"DueDate", box_value(to_hstring(dueDate ? dueDate : "")));
                activityMap.Insert(L"Status", box_value(to_hstring(status ? status : "")));
                activityMap.Insert(L"AssigneeName", box_value(to_hstring(assigneeName ? assigneeName : "")));
                activityMap.Insert(L"AssigneeImageUrl", box_value(to_hstring(assigneeImage ? assigneeImage : "")));

                // Colores según el estado
                std::string statusStr = status ? status : "";
                activityMap.Insert(L"StatusColor", box_value(getStatusColor(statusStr)));
                activityMap.Insert(L"StatusBackgroundColor", box_value(getStatusBackgroundColor(statusStr)));
                activityMap.Insert(L"StatusTextColor", box_value(getStatusTextColor(statusStr)));

                // Para múltiples asignados (por ahora solo uno)
                activityMap.Insert(L"HasMultipleAssignees", box_value(L"Collapsed"));
                activityMap.Insert(L"AdditionalAssigneesCount", box_value(L""));

                activitiesCollection.Append(activityMap.as<IInspectable>());
            }
        }

        sqlite3_finalize(stmt);

        // Asignar la colección al ListView
        ActivitiesListView().ItemsSource(activitiesCollection);

        // Mostrar mensaje si no hay actividades
        if (activitiesCollection.Size() == 0)
        {
            EmptyStatePanel().Visibility(Visibility::Visible);
            ActivitiesListView().Visibility(Visibility::Collapsed);
        }
        else
        {
            EmptyStatePanel().Visibility(Visibility::Collapsed);
            ActivitiesListView().Visibility(Visibility::Visible);
        }
    }

    hstring ActividadesPage::getStatusColor(const std::string& status)
    {
        if (status == "Pendiente") return L"#F57C00";
        if (status == "En Progreso") return L"#0078D4";
        if (status == "Finalizado") return L"#388E3C";
        return L"#666666";
    }

    hstring ActividadesPage::getStatusBackgroundColor(const std::string& status)
    {
        if (status == "Pendiente") return L"#FFF3E0";
        if (status == "En Progreso") return L"#E3F2FD";
        if (status == "Finalizado") return L"#E8F5E8";
        return L"#F5F5F5";
    }

    hstring ActividadesPage::getStatusTextColor(const std::string& status)
    {
        if (status == "Pendiente") return L"#E65100";
        if (status == "En Progreso") return L"#0277BD";
        if (status == "Finalizado") return L"#2E7D32";
        return L"#424242";
    }
}
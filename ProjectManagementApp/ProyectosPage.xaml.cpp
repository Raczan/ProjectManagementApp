#include "pch.h"
#include "ProyectosPage.xaml.h"
#if __has_include("ProyectosPage.g.cpp")
#include "ProyectosPage.g.cpp"
#endif
#include <winrt/Windows.Storage.h>

using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Xaml::Controls;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;

namespace winrt::ProjectManagementApp::implementation
{
    void ProyectosPage::Page_Loaded(IInspectable const& sender, RoutedEventArgs const& e)
    {
        // Solo abrir DB si no está abierta ya
        if (!db)
        {
            openDatabase();
        }

        // Solo cargar si no se ha cargado ya en OnNavigatedTo
        if (currentProjectId > 0 && (!activitiesCollection || activitiesCollection.Size() == 0))
        {
            loadProjectInfo();
            LoadProjectActivities();
        }
    }

    void ProyectosPage::OnNavigatedTo(Microsoft::UI::Xaml::Navigation::NavigationEventArgs const& e)
    {
        if (e.Parameter())
        {
            currentProjectId = winrt::unbox_value<int>(e.Parameter());
        }

        // Abrir base de datos aquí también
        openDatabase();

        // Cargar datos inmediatamente si tenemos el ID
        if (currentProjectId > 0)
        {
            loadProjectInfo();
            LoadProjectActivities();
        }
    }

    void ProyectosPage::SetProjectId(int projectId)
    {
        currentProjectId = projectId;
    }

    void ProyectosPage::openDatabase()
    {
        try
        {
            auto localFolder = Windows::Storage::ApplicationData::Current().LocalFolder();
            auto localFolderPath = localFolder.Path();
            std::wstring dbPath = std::wstring(localFolderPath) + L"\\projectmanagement.db";

            // Convertir a string para SQLite
            std::string dbPathStr(dbPath.begin(), dbPath.end());

            int rc = sqlite3_open(dbPathStr.c_str(), &db);
            if (rc != SQLITE_OK)
            {
                sqlite3_close(db);
                db = nullptr;
            }
        }
        catch (...)
        {
            if (db)
            {
                sqlite3_close(db);
                db = nullptr;
            }
        }
    }

    void ProyectosPage::closeDatabase()
    {
        if (db)
        {
            sqlite3_close(db);
            db = nullptr;
        }
    }

    void ProyectosPage::loadProjectInfo()
    {
        if (!db || currentProjectId <= 0) return;

        const char* sql = R"(
            SELECT 
                p.nombre,
                p.descripcion,
                p.estado,
                p.fecha_fin_prevista,
                COUNT(a.actividad_id) as total_actividades,
                COUNT(CASE WHEN a.estado = 'Finalizado' THEN 1 END) as actividades_completadas
            FROM Proyectos p
            LEFT JOIN Actividades a ON p.proyecto_id = a.proyecto_id
            WHERE p.proyecto_id = ?
            GROUP BY p.proyecto_id
        )";

        sqlite3_stmt* stmt;
        if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK)
        {
            sqlite3_bind_int(stmt, 1, currentProjectId);

            if (sqlite3_step(stmt) == SQLITE_ROW)
            {
                // Nombre del proyecto
                const char* name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
                if (name)
                {
                    projectName = winrt::to_hstring(name);
                    ProjectTitle().Text(projectName);
                    ProjectNameText().Text(projectName);
                }

                // Estado del proyecto
                const char* status = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
                if (status)
                {
                    ProjectStatusText().Text(winrt::to_hstring(status));
                }

                // Fecha límite
                const char* dueDate = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
                if (dueDate)
                {
                    ProjectDueDateText().Text(winrt::to_hstring(dueDate));
                }

                // Estadísticas
                int totalActivities = sqlite3_column_int(stmt, 4);
                int completedActivities = sqlite3_column_int(stmt, 5);

                TotalActivitiesText().Text(winrt::to_hstring(std::to_string(totalActivities)));

                // Calcular porcentaje de completado
                int percentage = totalActivities > 0 ? (completedActivities * 100) / totalActivities : 0;
                CompletedPercentageText().Text(winrt::to_hstring(std::to_string(percentage) + "%"));
            }
        }
        sqlite3_finalize(stmt);
    }

    void ProyectosPage::LoadProjectActivities()
    {
        if (!db || currentProjectId <= 0) return;

        activitiesCollection = winrt::single_threaded_observable_vector<IInspectable>();

        const char* sql = R"(
            SELECT DISTINCT
                a.actividad_id,
                a.nombre as activity_name,
                a.descripcion as activity_description,
                a.fecha_fin_prevista as due_date,
                a.estado as status,
                a.prioridad as priority,
                u.nombre || ' ' || u.apellido as assignee_name,
                u.url_imagen_perfil as assignee_image,
                COUNT(DISTINCT as2.usuario_id) as total_assignees
            FROM Actividades a
            LEFT JOIN Asignaciones as1 ON a.actividad_id = as1.actividad_id
            LEFT JOIN Usuarios u ON as1.usuario_id = u.usuario_id
            LEFT JOIN Asignaciones as2 ON a.actividad_id = as2.actividad_id
            WHERE a.proyecto_id = ?
            GROUP BY a.actividad_id, u.usuario_id
            ORDER BY a.fecha_inicio DESC
        )";

        sqlite3_stmt* stmt;
        if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK)
        {
            sqlite3_bind_int(stmt, 1, currentProjectId);

            std::map<int, PropertySet> activityMap;

            while (sqlite3_step(stmt) == SQLITE_ROW)
            {
                int activityId = sqlite3_column_int(stmt, 0);

                if (activityMap.find(activityId) == activityMap.end())
                {
                    auto activity = PropertySet();

                    // Información básica de la actividad
                    const char* activityName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
                    const char* activityDescription = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
                    const char* dueDate = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
                    const char* status = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
                    const char* priority = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));

                    activity.Insert(L"ActivityName", winrt::box_value(activityName ? winrt::to_hstring(activityName) : L"Sin nombre"));
                    activity.Insert(L"ActivityDescription", winrt::box_value(activityDescription ? winrt::to_hstring(activityDescription) : L""));
                    activity.Insert(L"DueDate", winrt::box_value(dueDate ? winrt::to_hstring(dueDate) : L"Sin fecha"));
                    activity.Insert(L"Status", winrt::box_value(status ? winrt::to_hstring(status) : L"Pendiente"));
                    activity.Insert(L"Priority", winrt::box_value(priority ? winrt::to_hstring(priority) : L"Media"));

                    // Colores de estado
                    std::string statusStr = status ? status : "Pendiente";
                    activity.Insert(L"StatusColor", winrt::box_value(getStatusColor(statusStr)));
                    activity.Insert(L"StatusBackgroundColor", winrt::box_value(getStatusBackgroundColor(statusStr)));
                    activity.Insert(L"StatusTextColor", winrt::box_value(getStatusTextColor(statusStr)));

                    // Colores de prioridad
                    std::string priorityStr = priority ? priority : "Media";
                    activity.Insert(L"PriorityBackgroundColor", winrt::box_value(getPriorityBackgroundColor(priorityStr)));
                    activity.Insert(L"PriorityTextColor", winrt::box_value(getPriorityTextColor(priorityStr)));

                    // Información del asignado principal
                    const char* assigneeName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
                    const char* assigneeImage = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 7));
                    int totalAssignees = sqlite3_column_int(stmt, 8);

                    activity.Insert(L"AssigneeName", winrt::box_value(assigneeName ? winrt::to_hstring(assigneeName) : L"Sin asignar"));
                    activity.Insert(L"AssigneeImageUrl", winrt::box_value(assigneeImage ? winrt::to_hstring(assigneeImage) : L""));

                    // Múltiples asignados
                    if (totalAssignees > 1)
                    {
                        activity.Insert(L"HasMultipleAssignees", winrt::box_value(Visibility::Visible));
                        activity.Insert(L"AdditionalAssigneesCount", winrt::box_value(L"+" + std::to_wstring(totalAssignees - 1)));
                    }
                    else
                    {
                        activity.Insert(L"HasMultipleAssignees", winrt::box_value(Visibility::Collapsed));
                        activity.Insert(L"AdditionalAssigneesCount", winrt::box_value(L""));
                    }

                    activityMap[activityId] = activity;
                }
            }

            // Agregar actividades únicas a la colección
            for (const auto& pair : activityMap)
            {
                activitiesCollection.Append(winrt::box_value(pair.second));
            }
        }
        sqlite3_finalize(stmt);

        // Asignar la colección al ListView
        ActivitiesListView().ItemsSource(activitiesCollection);

        // Mostrar/ocultar panel vacío
        if (activitiesCollection.Size() == 0)
        {
            ActivitiesListView().Visibility(Visibility::Collapsed);
            EmptyStatePanel().Visibility(Visibility::Visible);
        }
        else
        {
            ActivitiesListView().Visibility(Visibility::Visible);
            EmptyStatePanel().Visibility(Visibility::Collapsed);
        }
    }

    winrt::hstring ProyectosPage::getStatusColor(const std::string& status)
    {
        if (status == "Pendiente") return L"#F57C00";
        if (status == "En Progreso") return L"#0078D4";
        if (status == "Finalizado") return L"#388E3C";
        return L"#666666";
    }

    winrt::hstring ProyectosPage::getStatusBackgroundColor(const std::string& status)
    {
        if (status == "Pendiente") return L"#FFF3E0";
        if (status == "En Progreso") return L"#E3F2FD";
        if (status == "Finalizado") return L"#E8F5E8";
        return L"#F5F5F5";
    }

    winrt::hstring ProyectosPage::getStatusTextColor(const std::string& status)
    {
        if (status == "Pendiente") return L"#E65100";
        if (status == "En Progreso") return L"#0277BD";
        if (status == "Finalizado") return L"#2E7D32";
        return L"#424242";
    }

    winrt::hstring ProyectosPage::getPriorityBackgroundColor(const std::string& priority)
    {
        if (priority == "Baja") return L"#E8F5E8";
        if (priority == "Media") return L"#FFF3E0";
        if (priority == "Alta") return L"#FFF3E0";
        if (priority == "Urgente") return L"#FFEBEE";
        return L"#F5F5F5";
    }

    winrt::hstring ProyectosPage::getPriorityTextColor(const std::string& priority)
    {
        if (priority == "Baja") return L"#2E7D32";
        if (priority == "Media") return L"#E65100";
        if (priority == "Alta") return L"#E65100";
        if (priority == "Urgente") return L"#C62828";
        return L"#424242";
    }
}
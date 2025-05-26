#include "pch.h"
#include "MiembrosPage.xaml.h"
#if __has_include("MiembrosPage.g.cpp")
#include "MiembrosPage.g.cpp"
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Xaml::Controls;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::ProjectManagementApp::implementation
{
    void MiembrosPage::Page_Loaded(IInspectable const& sender, RoutedEventArgs const& e)
    {
        openDatabase();
        LoadMembers();
    }

    void MiembrosPage::AddMemberButton_Click(IInspectable const& sender, RoutedEventArgs const& e)
    {
        // Crear el diálogo
        auto dialog = winrt::make<ProjectManagementApp::implementation::AddMemberDialog>();

        // Establecer el XamlRoot - esto es necesario en WinUI 3
        dialog.XamlRoot(this->XamlRoot());

        // Mostrar el diálogo de forma asíncrona
        auto asyncOp = dialog.ShowAsync();
        asyncOp.Completed([this, dialog](auto const& asyncInfo, auto const& status)
            {
                if (status == winrt::Windows::Foundation::AsyncStatus::Completed)
                {
                    auto result = asyncInfo.GetResults();
                    if (result == Controls::ContentDialogResult::Primary)
                    {
                        // El usuario hizo clic en "Agregar"
                        bool success = AddMemberToDatabase(
                            dialog.FirstName(),
                            dialog.LastName(),
                            dialog.Email(),
                            dialog.Phone(),
                            dialog.Department()
                        );

                        if (success)
                        {
                            // Recargar la lista de miembros
                            LoadMembers();
                        }
                    }
                }
            });
    }

    void MiembrosPage::DeleteMember_Click(IInspectable const& sender, RoutedEventArgs const& e)
    {
        // Obtener el miembro del Tag del MenuFlyoutItem
        auto menuItem = sender.as<MenuFlyoutItem>();
        auto memberData = menuItem.Tag().as<Windows::Foundation::Collections::ValueSet>();

        if (memberData)
        {
            auto userId = winrt::unbox_value<int>(memberData.Lookup(L"UserId"));
            auto fullName = winrt::unbox_value<winrt::hstring>(memberData.Lookup(L"FullName"));

            // Mostrar diálogo de confirmación
            winrt::hstring message = L"¿Está seguro que desea eliminar a " + fullName + L"?\n\nEsta acción no se puede deshacer.";

            ShowConfirmationDialog(message, L"Confirmar Eliminación", [this, userId]()
                {
                    // Eliminar de la base de datos
                    bool success = DeleteMemberFromDatabase(userId);
                    if (success)
                    {
                        // Recargar la lista
                        LoadMembers();
                    }
                });
        }
    }

    void MiembrosPage::EditMember_Click(IInspectable const& sender, RoutedEventArgs const& e)
    {
        // Obtener el miembro del Tag del MenuFlyoutItem
        auto menuItem = sender.as<MenuFlyoutItem>();
        auto memberData = menuItem.Tag().as<Windows::Foundation::Collections::ValueSet>();

        if (memberData)
        {
            auto userId = winrt::unbox_value<int>(memberData.Lookup(L"UserId"));
            auto firstName = winrt::unbox_value<winrt::hstring>(memberData.Lookup(L"FirstName"));
            auto lastName = winrt::unbox_value<winrt::hstring>(memberData.Lookup(L"LastName"));
            auto email = winrt::unbox_value<winrt::hstring>(memberData.Lookup(L"Email"));
            auto phone = winrt::unbox_value<winrt::hstring>(memberData.Lookup(L"Phone"));
            auto department = winrt::unbox_value<winrt::hstring>(memberData.Lookup(L"Department"));

            // Crear el diálogo en modo edición
            auto dialog = winrt::make<ProjectManagementApp::implementation::AddMemberDialog>(
                userId, firstName, lastName, email, phone, department);

            // Establecer el XamlRoot
            dialog.XamlRoot(this->XamlRoot());

            // Mostrar el diálogo de forma asíncrona
            auto asyncOp = dialog.ShowAsync();
            asyncOp.Completed([this, dialog](auto const& asyncInfo, auto const& status)
                {
                    if (status == winrt::Windows::Foundation::AsyncStatus::Completed)
                    {
                        auto result = asyncInfo.GetResults();
                        if (result == Controls::ContentDialogResult::Primary)
                        {
                            // El usuario hizo clic en "Guardar Cambios"
                            bool success = UpdateMemberInDatabase(
                                dialog.UserId(),
                                dialog.FirstName(),
                                dialog.LastName(),
                                dialog.Email(),
                                dialog.Phone(),
                                dialog.Department()
                            );

                            if (success)
                            {
                                // Recargar la lista de miembros
                                LoadMembers();
                            }
                        }
                    }
                });
        }
    }

    bool MiembrosPage::UpdateMemberInDatabase(int userId, winrt::hstring const& firstName, winrt::hstring const& lastName,
        winrt::hstring const& email, winrt::hstring const& phone,
        winrt::hstring const& department)
    {
        if (!db) return false;

        const char* sql = R"(
            UPDATE Usuarios 
            SET nombre = ?, apellido = ?, email = ?, telefono = ?, departamento = ?
            WHERE usuario_id = ?
        )";

        sqlite3_stmt* stmt;
        int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

        if (rc != SQLITE_OK)
        {
            return false;
        }

        // Convertir de wide string a UTF-8
        auto wstringToUtf8 = [](const std::wstring& wstr) -> std::string {
            if (wstr.empty()) return std::string();

            int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
            std::string strTo(size_needed, 0);
            WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
            return strTo;
            };

        // Bind de los parámetros
        std::string firstNameUtf8 = wstringToUtf8(firstName.c_str());
        std::string lastNameUtf8 = wstringToUtf8(lastName.c_str());
        std::string emailUtf8 = wstringToUtf8(email.c_str());
        std::string phoneUtf8 = wstringToUtf8(phone.c_str());
        std::string departmentUtf8 = wstringToUtf8(department.c_str());

        sqlite3_bind_text(stmt, 1, firstNameUtf8.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, lastNameUtf8.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, emailUtf8.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 4, phoneUtf8.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 5, departmentUtf8.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 6, userId);

        // Ejecutar la consulta
        rc = sqlite3_step(stmt);
        sqlite3_finalize(stmt);

        return rc == SQLITE_DONE;
    }

    bool MiembrosPage::DeleteMemberFromDatabase(int userId)
    {
        if (!db) return false;

        const char* sql = "DELETE FROM Usuarios WHERE usuario_id = ?";

        sqlite3_stmt* stmt;
        int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

        if (rc != SQLITE_OK)
        {
            return false;
        }

        sqlite3_bind_int(stmt, 1, userId);
        rc = sqlite3_step(stmt);
        sqlite3_finalize(stmt);

        return rc == SQLITE_DONE;
    }

    winrt::Windows::Foundation::IAsyncAction MiembrosPage::ShowConfirmationDialog(winrt::hstring const& message,
        winrt::hstring const& title,
        std::function<void()> onConfirm)
    {
        ContentDialog dialog;
        dialog.XamlRoot(this->XamlRoot());
        dialog.Title(winrt::box_value(title));
        dialog.Content(winrt::box_value(message));
        dialog.PrimaryButtonText(L"Eliminar");
        dialog.SecondaryButtonText(L"Cancelar");
        dialog.DefaultButton(ContentDialogButton::Secondary);

        // Establecer el color del botón primario como destructivo
        dialog.PrimaryButtonStyle(nullptr); // Usar estilo por defecto

        auto result = co_await dialog.ShowAsync();

        if (result == ContentDialogResult::Primary)
        {
            onConfirm();
        }
    }

    void MiembrosPage::MembersList_ItemClick(IInspectable const& sender, ItemClickEventArgs const& e)
    {
        // TODO: Implementar funcionalidad cuando se hace clic en un miembro
        // Por ahora solo mostramos información en debug
        // En el futuro aquí se mostrará el detalle del miembro
    }

    void MiembrosPage::LoadMembers()
    {
        if (!db) return;

        // Crear la colección si no existe
        if (!membersCollection)
        {
            membersCollection = winrt::single_threaded_observable_vector<IInspectable>();
        }
        else
        {
            membersCollection.Clear();
        }

        const char* sql = R"(
            SELECT 
                usuario_id,
                nombre,
                apellido,
                email,
                telefono,
                departamento,
                DATE(fecha_registro) as fecha_registro,
                url_imagen_perfil
            FROM Usuarios 
            ORDER BY apellido, nombre
        )";

        sqlite3_stmt* stmt;
        int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

        if (rc == SQLITE_OK)
        {
            while (sqlite3_step(stmt) == SQLITE_ROW)
            {
                // Crear un objeto ValueSet para almacenar los datos del miembro
                auto member = winrt::Windows::Foundation::Collections::ValueSet();

                // Obtener datos de la base de datos
                int userId = sqlite3_column_int(stmt, 0);

                // Función helper para convertir UTF-8 a wide string
                auto utf8ToWstring = [](const char* utf8Str) -> std::wstring {
                    if (!utf8Str) return L"";

                    int size_needed = MultiByteToWideChar(CP_UTF8, 0, utf8Str, -1, NULL, 0);
                    if (size_needed == 0) return L"";

                    std::wstring wstr(size_needed - 1, L'\0');
                    MultiByteToWideChar(CP_UTF8, 0, utf8Str, -1, &wstr[0], size_needed);
                    return wstr;
                    };

                // Convertir directamente de UTF-8 a wide string
                std::wstring wFirstName = utf8ToWstring((char*)sqlite3_column_text(stmt, 1));
                std::wstring wLastName = utf8ToWstring((char*)sqlite3_column_text(stmt, 2));
                std::wstring wEmail = utf8ToWstring((char*)sqlite3_column_text(stmt, 3));
                std::wstring wPhone = utf8ToWstring((char*)sqlite3_column_text(stmt, 4));
                std::wstring wDepartment = utf8ToWstring((char*)sqlite3_column_text(stmt, 5));
                std::wstring wRegistrationDate = utf8ToWstring((char*)sqlite3_column_text(stmt, 6));
                std::wstring wProfileImageUrl = utf8ToWstring((char*)sqlite3_column_text(stmt, 7));

                // Crear nombre completo
                std::wstring fullName = wFirstName + L" " + wLastName;

                // Agregar datos al ValueSet
                member.Insert(L"UserId", winrt::box_value(userId));
                member.Insert(L"FirstName", winrt::box_value(winrt::hstring(wFirstName)));
                member.Insert(L"LastName", winrt::box_value(winrt::hstring(wLastName)));
                member.Insert(L"FullName", winrt::box_value(winrt::hstring(fullName)));
                member.Insert(L"Email", winrt::box_value(winrt::hstring(wEmail)));
                member.Insert(L"Phone", winrt::box_value(winrt::hstring(wPhone)));
                member.Insert(L"Department", winrt::box_value(winrt::hstring(wDepartment)));
                member.Insert(L"RegistrationDate", winrt::box_value(winrt::hstring(wRegistrationDate)));
                member.Insert(L"ProfileImageUrl", winrt::box_value(winrt::hstring(wProfileImageUrl)));

                // Agregar a la colección
                membersCollection.Append(member);
            }
        }

        sqlite3_finalize(stmt);

        // Asignar la colección al ListView
        MembersList().ItemsSource(membersCollection);
    }

    bool MiembrosPage::AddMemberToDatabase(winrt::hstring const& firstName, winrt::hstring const& lastName,
        winrt::hstring const& email, winrt::hstring const& phone,
        winrt::hstring const& department)
    {
        if (!db) return false;

        const char* sql = R"(
            INSERT INTO Usuarios (nombre, apellido, email, telefono, departamento, fecha_registro, url_imagen_perfil)
            VALUES (?, ?, ?, ?, ?, CURRENT_TIMESTAMP, '')
        )";

        sqlite3_stmt* stmt;
        int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

        if (rc != SQLITE_OK)
        {
            return false;
        }

        // Convertir de wide string a UTF-8
        auto wstringToUtf8 = [](const std::wstring& wstr) -> std::string {
            if (wstr.empty()) return std::string();

            int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
            std::string strTo(size_needed, 0);
            WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
            return strTo;
            };

        // Bind de los parámetros
        std::string firstNameUtf8 = wstringToUtf8(firstName.c_str());
        std::string lastNameUtf8 = wstringToUtf8(lastName.c_str());
        std::string emailUtf8 = wstringToUtf8(email.c_str());
        std::string phoneUtf8 = wstringToUtf8(phone.c_str());
        std::string departmentUtf8 = wstringToUtf8(department.c_str());

        sqlite3_bind_text(stmt, 1, firstNameUtf8.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, lastNameUtf8.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, emailUtf8.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 4, phoneUtf8.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 5, departmentUtf8.c_str(), -1, SQLITE_STATIC);

        // Ejecutar la consulta
        rc = sqlite3_step(stmt);
        sqlite3_finalize(stmt);

        return rc == SQLITE_DONE;
    }

    void MiembrosPage::openDatabase()
    {
        if (db) return; // Ya está abierto

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

    void MiembrosPage::closeDatabase()
    {
        if (db)
        {
            sqlite3_close(db);
            db = nullptr;
        }
    }
}
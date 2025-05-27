#include "pch.h"
#include "AddMemberDialog.xaml.h"
#if __has_include("AddMemberDialog.g.cpp")
#include "AddMemberDialog.g.cpp"
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Xaml::Controls;

namespace winrt::ProjectManagementApp::implementation
{
    AddMemberDialog::AddMemberDialog()
    {
        InitializeComponent();
        PrimaryButtonClick({ this, &AddMemberDialog::ContentDialog_PrimaryButtonClick });

        m_isEditMode = false;
        m_userId = 0;
    }

    AddMemberDialog::AddMemberDialog(int32_t userId, winrt::hstring const& firstName, winrt::hstring const& lastName,
        winrt::hstring const& email, winrt::hstring const& phone, winrt::hstring const& department)
    {
        InitializeComponent();
        PrimaryButtonClick({ this, &AddMemberDialog::ContentDialog_PrimaryButtonClick });

        m_isEditMode = true;
        m_userId = userId;

        FirstNameTextBox().Text(firstName);
        LastNameTextBox().Text(lastName);
        EmailTextBox().Text(email);
        PhoneTextBox().Text(phone);

        for (uint32_t i = 0; i < DepartmentComboBox().Items().Size(); ++i)
        {
            auto item = DepartmentComboBox().Items().GetAt(i).as<Controls::ComboBoxItem>();
            auto content = winrt::unbox_value<winrt::hstring>(item.Content());
            if (content == department)
            {
                DepartmentComboBox().SelectedIndex(i);
                break;
            }
        }

        SetupForEditMode();
    }

    winrt::hstring AddMemberDialog::FirstName()
    {
        return FirstNameTextBox().Text();
    }

    winrt::hstring AddMemberDialog::LastName()
    {
        return LastNameTextBox().Text();
    }

    winrt::hstring AddMemberDialog::Email()
    {
        return EmailTextBox().Text();
    }

    winrt::hstring AddMemberDialog::Phone()
    {
        return PhoneTextBox().Text();
    }

    winrt::hstring AddMemberDialog::Department()
    {
        auto selectedItem = DepartmentComboBox().SelectedItem();
        if (selectedItem)
        {
            auto comboBoxItem = selectedItem.as<ComboBoxItem>();
            return winrt::unbox_value<winrt::hstring>(comboBoxItem.Content());
        }
        return L"";
    }

    int32_t AddMemberDialog::UserId()
    {
        return m_userId;
    }

    bool AddMemberDialog::IsEditMode()
    {
        return m_isEditMode;
    }

    void AddMemberDialog::SetupForEditMode()
    {
        if (m_isEditMode)
        {
            Title(winrt::box_value(L"Editar Miembro"));
            PrimaryButtonText(L"Guardar Cambios");
        }
    }

    void AddMemberDialog::ContentDialog_PrimaryButtonClick(ContentDialog const& sender,
        ContentDialogButtonClickEventArgs const& args)
    {
        if (!ValidateInput())
        {
            args.Cancel(true);
        }
    }

    bool AddMemberDialog::ValidateInput()
    {
        if (FirstNameTextBox().Text().empty())
        {
            ShowErrorMessage(L"El nombre es obligatorio");
            FirstNameTextBox().Focus(FocusState::Keyboard);
            return false;
        }

        if (LastNameTextBox().Text().empty())
        {
            ShowErrorMessage(L"El apellido es obligatorio");
            LastNameTextBox().Focus(FocusState::Keyboard);
            return false;
        }

        if (EmailTextBox().Text().empty())
        {
            ShowErrorMessage(L"El correo electrónico es obligatorio");
            EmailTextBox().Focus(FocusState::Keyboard);
            return false;
        }

        if (DepartmentComboBox().SelectedItem() == nullptr)
        {
            ShowErrorMessage(L"Debe seleccionar un departamento");
            DepartmentComboBox().Focus(FocusState::Keyboard);
            return false;
        }

        return true;
    }

    void AddMemberDialog::ShowErrorMessage(winrt::hstring const& message)
    {
    }
}
#pragma once

#include "AddMemberDialog.g.h"

namespace winrt::ProjectManagementApp::implementation
{
    struct AddMemberDialog : AddMemberDialogT<AddMemberDialog>
    {
        AddMemberDialog();
        AddMemberDialog(int32_t userId, winrt::hstring const& firstName, winrt::hstring const& lastName,
            winrt::hstring const& email, winrt::hstring const& phone, winrt::hstring const& department);

        winrt::hstring FirstName();
        winrt::hstring LastName();
        winrt::hstring Email();
        winrt::hstring Phone();
        winrt::hstring Department();
        int32_t UserId();
        bool IsEditMode();

        void ContentDialog_PrimaryButtonClick(winrt::Microsoft::UI::Xaml::Controls::ContentDialog const& sender,
            winrt::Microsoft::UI::Xaml::Controls::ContentDialogButtonClickEventArgs const& args);

    private:
        bool ValidateInput();
        void ShowErrorMessage(winrt::hstring const& message);
        void SetupForEditMode();

        int32_t m_userId = 0;
        bool m_isEditMode = false;
    };
}

namespace winrt::ProjectManagementApp::factory_implementation
{
    struct AddMemberDialog : AddMemberDialogT<AddMemberDialog, implementation::AddMemberDialog>
    {
    };
}
#pragma once

#include "MiembrosPage.g.h"
#include "AddMemberDialog.xaml.h"
#include "db/sqlite3.h"
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Storage.h>
#include <functional>

namespace winrt::ProjectManagementApp::implementation
{
    struct MiembrosPage : MiembrosPageT<MiembrosPage>
    {
        MiembrosPage()
        {

        }

        void Page_Loaded(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void AddMemberButton_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void MembersList_ItemClick(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Controls::ItemClickEventArgs const& e);
        void DeleteMember_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void EditMember_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void LoadMembers();

    private:
        sqlite3* db = nullptr;
        winrt::Windows::Foundation::Collections::IObservableVector<winrt::Windows::Foundation::IInspectable> membersCollection = nullptr;
        void openDatabase();
        void closeDatabase();
        bool AddMemberToDatabase(winrt::hstring const& firstName, winrt::hstring const& lastName,
            winrt::hstring const& email, winrt::hstring const& phone,
            winrt::hstring const& department);
        bool UpdateMemberInDatabase(int userId, winrt::hstring const& firstName, winrt::hstring const& lastName,
            winrt::hstring const& email, winrt::hstring const& phone,
            winrt::hstring const& department);
        bool DeleteMemberFromDatabase(int userId);
        winrt::Windows::Foundation::IAsyncAction ShowConfirmationDialog(winrt::hstring const& message,
            winrt::hstring const& title,
            std::function<void()> onConfirm);
    };
}

namespace winrt::ProjectManagementApp::factory_implementation
{
    struct MiembrosPage : MiembrosPageT<MiembrosPage, implementation::MiembrosPage>
    {
    };
}
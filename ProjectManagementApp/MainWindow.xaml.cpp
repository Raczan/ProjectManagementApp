#include "pch.h"
#include "MainWindow.xaml.h"
#if __has_include("MainWindow.g.cpp")
#include "MainWindow.g.cpp"
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Xaml::Controls;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::ProjectManagementApp::implementation
{
    void MainWindow::InitializeComponent()
    {
        MainWindowT::InitializeComponent();
        LoadProjects();
    }

    int32_t MainWindow::MyProperty()
    {
        throw hresult_not_implemented();
    }

    void MainWindow::MyProperty(int32_t /* value */)
    {
        throw hresult_not_implemented();
    }

    void MainWindow::LoadProjects()
    {
        try {
            auto projectsNavItem = ProjectsNavItem();

            if (projectsNavItem == nullptr) {
                return;
            }

            projectsNavItem.MenuItems().Clear();

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
}

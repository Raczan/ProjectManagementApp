#pragma once

#include "MainWindow.g.h"

namespace winrt::ProjectManagementApp::implementation
{
    struct MainWindow : MainWindowT<MainWindow>
    {
        MainWindow()
        {
            // Xaml objects should not call InitializeComponent during construction.
            // See https://github.com/microsoft/cppwinrt/tree/master/nuget#initializecomponent
        }

        void InitializeComponent();
        int32_t MyProperty();
        void MyProperty(int32_t value);
        void LoadProjects();

    private:
        std::vector<std::wstring> projectsList = {
            L"Proyecto Interactivo",
            L"Gesti�n de Hotel",
            L"Sistema de Contabilidad",
            L"Administraci�n de Miembros"
        };
    };
}

namespace winrt::ProjectManagementApp::factory_implementation
{
    struct MainWindow : MainWindowT<MainWindow, implementation::MainWindow>
    {
    };
}

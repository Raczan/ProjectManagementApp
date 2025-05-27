#pragma once

#include "MainWindow.g.h"

namespace winrt::ProjectManagementApp::implementation
{
    struct MainWindow : MainWindowT<MainWindow>
    {
        MainWindow()
        {
        }
    };
}

namespace winrt::ProjectManagementApp::factory_implementation
{
    struct MainWindow : MainWindowT<MainWindow, implementation::MainWindow>
    {
    };
}

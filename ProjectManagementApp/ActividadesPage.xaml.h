#pragma once

#include "ActividadesPage.g.h"

namespace winrt::ProjectManagementApp::implementation
{
    struct ActividadesPage : ActividadesPageT<ActividadesPage>
    {
        ActividadesPage()
        {
            // Xaml objects should not call InitializeComponent during construction.
            // See https://github.com/microsoft/cppwinrt/tree/master/nuget#initializecomponent
        }
    };
}

namespace winrt::ProjectManagementApp::factory_implementation
{
    struct ActividadesPage : ActividadesPageT<ActividadesPage, implementation::ActividadesPage>
    {
    };
}

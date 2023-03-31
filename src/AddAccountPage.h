#pragma once

#include "AddAccountPage.g.h"

namespace winrt::coro_cloudbrowser_winrt::implementation {

struct AddAccountPage : AddAccountPageT<AddAccountPage> {};

}  // namespace winrt::coro_cloudbrowser_winrt::implementation

namespace winrt::coro_cloudbrowser_winrt::factory_implementation {
struct AddAccountPage
    : AddAccountPageT<AddAccountPage, implementation::AddAccountPage> {};
}  // namespace winrt::coro_cloudbrowser_winrt::factory_implementation

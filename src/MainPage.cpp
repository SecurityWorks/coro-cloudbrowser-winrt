﻿// clang-format off
#include "pch.h"
#include "MainPage.h"
#include "MainPage.g.cpp"
// clang-format on

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::coro_cloudbrowser_winrt::implementation {

int32_t MainPage::MyProperty() { throw hresult_not_implemented(); }

void MainPage::MyProperty(int32_t /* value */) {
  throw hresult_not_implemented();
}

void MainPage::ClickHandler(IInspectable const&, RoutedEventArgs const&) {
  // myButton().Content(box_value(L"Clicked"));
}

}  // namespace winrt::coro_cloudbrowser_winrt::implementation

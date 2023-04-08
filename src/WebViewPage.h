#pragma once

#include "WebViewPage.g.h"

namespace winrt::coro_cloudbrowser_winrt::implementation {
struct WebViewPage : WebViewPageT<WebViewPage> {
  void OnNavigatedTo(
      const Windows::UI::Xaml::Navigation::NavigationEventArgs& args);
};
}  // namespace winrt::coro_cloudbrowser_winrt::implementation

namespace winrt::coro_cloudbrowser_winrt::factory_implementation {
struct WebViewPage : WebViewPageT<WebViewPage, implementation::WebViewPage> {};
}  // namespace winrt::coro_cloudbrowser_winrt::factory_implementation

#pragma once

#include "FilePreviewPage.g.h"

namespace winrt::coro_cloudbrowser_winrt::implementation {
struct FilePreviewPage : FilePreviewPageT<FilePreviewPage> {
  void OnNavigatedTo(const Windows::UI::Xaml::Navigation::NavigationEventArgs&);

  void OnNavigatingFrom(
      const Windows::UI::Xaml::Navigation::NavigatingCancelEventArgs&);

  void GridKeyDown(const Windows::Foundation::IInspectable&,
                   const Windows::UI::Xaml::Input::KeyRoutedEventArgs&);

  void BackButtonClick(const Windows::Foundation::IInspectable&,
                       const Windows::UI::Xaml::RoutedEventArgs&);
};
}  // namespace winrt::coro_cloudbrowser_winrt::implementation

namespace winrt::coro_cloudbrowser_winrt::factory_implementation {
struct FilePreviewPage
    : FilePreviewPageT<FilePreviewPage, implementation::FilePreviewPage> {};
}  // namespace winrt::coro_cloudbrowser_winrt::factory_implementation

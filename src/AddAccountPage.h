#pragma once

#include "AddAccountPage.g.h"

namespace winrt::coro_cloudbrowser_winrt::implementation {

struct AddAccountPage : AddAccountPageT<AddAccountPage> {
  void OnNavigatedTo(
      const Windows::UI::Xaml::Navigation::NavigationEventArgs& args);

  void ItemListViewItemClick(
      const Windows::Foundation::IInspectable& sender,
      const Windows::UI::Xaml::Controls::ItemClickEventArgs&);
  void ItemListViewKeyDown(const Windows::Foundation::IInspectable& sender,
                           const Windows::UI::Xaml::Input::KeyRoutedEventArgs&);
};

}  // namespace winrt::coro_cloudbrowser_winrt::implementation

namespace winrt::coro_cloudbrowser_winrt::factory_implementation {
struct AddAccountPage
    : AddAccountPageT<AddAccountPage, implementation::AddAccountPage> {};
}  // namespace winrt::coro_cloudbrowser_winrt::factory_implementation

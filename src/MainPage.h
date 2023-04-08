#pragma once

#include <optional>

#include "CloudProviderAccountModel.h"
#include "MainPage.g.h"
#include "MainPageModel.h"

namespace winrt::coro_cloudbrowser_winrt::implementation {

struct MainPage : MainPageT<MainPage> {
  void NavViewLoaded(const Windows::Foundation::IInspectable& sender,
                     const Windows::UI::Xaml::RoutedEventArgs& args);

  void OnNavigatedTo(
      const Windows::UI::Xaml::Navigation::NavigationEventArgs& e);

  void OnNavigatedFrom(
      const Windows::UI::Xaml::Navigation::NavigationEventArgs& e);

  void MenuItemInvoked(
      const Windows::UI::Xaml::Controls::NavigationView& sender,
      const Windows::UI::Xaml::Controls::NavigationViewItemInvokedEventArgs&
          args);

 private:
  void UpdateMenu();

  event_token accounts_changed_;
  std::optional<coro_cloudbrowser_winrt::MainPageModel> model_;
};

}  // namespace winrt::coro_cloudbrowser_winrt::implementation

namespace winrt::coro_cloudbrowser_winrt::factory_implementation {

struct MainPage : MainPageT<MainPage, implementation::MainPage> {};

}  // namespace winrt::coro_cloudbrowser_winrt::factory_implementation

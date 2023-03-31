#pragma once

#include "CloudProviderAccountModel.h"
#include "MainPage.g.h"

namespace winrt::coro_cloudbrowser_winrt::implementation {

struct MainPage : MainPageT<MainPage> {
  void NavViewLoaded(Windows::Foundation::IInspectable const& sender,
                     Windows::UI::Xaml::RoutedEventArgs const& args);

  void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs e);

  void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs e);

  void MenuItemInvoked(
      const Windows::UI::Xaml::Controls::NavigationView& sender,
      const Windows::UI::Xaml::Controls::NavigationViewItemInvokedEventArgs&
          args);

 private:
  void UpdateMenu();

  event_token accounts_changed_;
  Windows::Foundation::Collections::IObservableVector<
      coro_cloudbrowser_winrt::CloudProviderAccountModel>
      accounts_;
};

}  // namespace winrt::coro_cloudbrowser_winrt::implementation

namespace winrt::coro_cloudbrowser_winrt::factory_implementation {

struct MainPage : MainPageT<MainPage, implementation::MainPage> {};

}  // namespace winrt::coro_cloudbrowser_winrt::factory_implementation

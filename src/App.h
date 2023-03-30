#pragma once
#include <coro/promise.h>
#include <coro/util/event_loop.h>

#include <future>

#include "App.xaml.g.h"
#include "CloudProviderAccountModel.h"

namespace winrt::coro_cloudbrowser_winrt::implementation {

struct App : AppT<App> {
  App();
  winrt::Windows::Foundation::IAsyncAction OnLaunched(
      Windows::ApplicationModel::Activation::LaunchActivatedEventArgs const &);
  void OnSuspending(IInspectable const &,
                    Windows::ApplicationModel::SuspendingEventArgs const &);
  void OnNavigationFailed(
      IInspectable const &,
      Windows::UI::Xaml::Navigation::NavigationFailedEventArgs const &);

 private:
  coro::Task<> RunHttpServer();

  std::future<void> thread_;
  coro::Promise<void> semaphore_;
  coro::util::EventLoop event_loop_;
  winrt::Windows::Foundation::Collections::IObservableVector<
      coro_cloudbrowser_winrt::CloudProviderAccountModel>
      accounts_;
};

}  // namespace winrt::coro_cloudbrowser_winrt::implementation

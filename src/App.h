#pragma once
#include <coro/cloudstorage/util/cloud_factory_context.h>
#include <coro/promise.h>
#include <coro/util/event_loop.h>

#include <future>

#include "App.xaml.g.h"
#include "CloudProviderAccountModel.h"
#include "CloudProviderTypeModel.h"

namespace winrt::coro_cloudbrowser_winrt::implementation {

struct App : AppT<App> {
  App();
  winrt::fire_and_forget OnLaunched(
      Windows::ApplicationModel::Activation::LaunchActivatedEventArgs);
  void OnSuspending(const IInspectable &,
                    const Windows::ApplicationModel::SuspendingEventArgs &);
  void OnNavigationFailed(
      const IInspectable &,
      const Windows::UI::Xaml::Navigation::NavigationFailedEventArgs &);

 private:
  coro::Task<> RunHttpServer();

  coro::util::EventLoop event_loop_;
  coro::cloudstorage::util::CloudFactoryContext context_;
  coro::Promise<void> init_semaphore_;
  coro::Promise<void> quit_semaphore_;
  winrt::Windows::Foundation::Collections::IObservableVector<
      coro_cloudbrowser_winrt::CloudProviderAccountModel>
      accounts_;
  winrt::Windows::Foundation::Collections::IVector<
      coro_cloudbrowser_winrt::CloudProviderTypeModel>
      provider_types_;
  std::future<void> thread_;
};

}  // namespace winrt::coro_cloudbrowser_winrt::implementation

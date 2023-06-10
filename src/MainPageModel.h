#pragma once

#include "CloudProviderAccountModel.h"
#include "CloudProviderTypeModel.h"
#include "MainPageModel.g.h"

namespace winrt::coro_cloudbrowser_winrt::implementation {
struct MainPageModel : MainPageModelT<MainPageModel> {
  MainPageModel(
      Windows::Foundation::Collections::IObservableVector<
          coro_cloudbrowser_winrt::CloudProviderAccountModel>
          accounts,
      Windows::Foundation::Collections::IVector<
          coro_cloudbrowser_winrt::CloudProviderTypeModel>
          provider_types,
      winrt::event<Windows::Foundation::EventHandler<Windows::Foundation::Uri>>*
          deep_link_event)
      : accounts_(std::move(accounts)),
        provider_types_(std::move(provider_types)),
        deep_link_event_(deep_link_event) {}

  Windows::Foundation::Collections::IObservableVector<
      coro_cloudbrowser_winrt::CloudProviderAccountModel>
  Accounts() const {
    return accounts_;
  }
  void Accounts(const Windows::Foundation::Collections::IObservableVector<
                coro_cloudbrowser_winrt::CloudProviderAccountModel>&) {
    throw hresult_not_implemented();
  }

  Windows::Foundation::Collections::IVector<
      coro_cloudbrowser_winrt::CloudProviderTypeModel>
  ProviderTypes() const {
    return provider_types_;
  }
  void ProviderTypes(const Windows::Foundation::Collections::IVector<
                     coro_cloudbrowser_winrt::CloudProviderTypeModel>&) {
    throw hresult_not_implemented();
  }

  auto* deep_link_event() const { return deep_link_event_; }

 private:
  Windows::Foundation::Collections::IObservableVector<
      coro_cloudbrowser_winrt::CloudProviderAccountModel>
      accounts_;
  Windows::Foundation::Collections::IVector<
      coro_cloudbrowser_winrt::CloudProviderTypeModel>
      provider_types_;
  winrt::event<Windows::Foundation::EventHandler<Windows::Foundation::Uri>>*
      deep_link_event_;
};
}  // namespace winrt::coro_cloudbrowser_winrt::implementation

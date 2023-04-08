#pragma once

#include "CloudProviderAccountModel.h"
#include "CloudProviderTypeModel.h"
#include "MainPageModel.g.h"

namespace winrt::coro_cloudbrowser_winrt::implementation {
struct MainPageModel : MainPageModelT<MainPageModel> {
  MainPageModel(Windows::Foundation::Collections::IObservableVector<
                    coro_cloudbrowser_winrt::CloudProviderAccountModel>
                    accounts,
                Windows::Foundation::Collections::IVector<
                    coro_cloudbrowser_winrt::CloudProviderTypeModel>
                    provider_types)
      : accounts_(std::move(accounts)),
        provider_types_(std::move(provider_types)) {}

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

 private:
  Windows::Foundation::Collections::IObservableVector<
      coro_cloudbrowser_winrt::CloudProviderAccountModel>
      accounts_;
  Windows::Foundation::Collections::IVector<
      coro_cloudbrowser_winrt::CloudProviderTypeModel>
      provider_types_;
};
}  // namespace winrt::coro_cloudbrowser_winrt::implementation

namespace winrt::coro_cloudbrowser_winrt::factory_implementation {
struct MainPageModel
    : MainPageModelT<MainPageModel, implementation::MainPageModel> {};
}  // namespace winrt::coro_cloudbrowser_winrt::factory_implementation

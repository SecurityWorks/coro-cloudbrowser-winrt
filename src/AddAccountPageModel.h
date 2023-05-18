#pragma once

#include "AddAccountPageModel.g.h"
#include "CloudProviderTypeModel.h"

namespace winrt::coro_cloudbrowser_winrt::implementation {
struct AddAccountPageModel : AddAccountPageModelT<AddAccountPageModel> {
  AddAccountPageModel(Windows::Foundation::Collections::IVector<
                          coro_cloudbrowser_winrt::CloudProviderTypeModel>
                          provider_types,
                      std::function<void()> on_navigated_to)
      : provider_types_(std::move(provider_types)),
        on_navigated_to_(std::move(on_navigated_to)) {}

  auto ProviderTypes() const { return provider_types_; }
  void ProviderTypes(Windows::Foundation::Collections::IVector<
                     coro_cloudbrowser_winrt::CloudProviderTypeModel>) {
    throw hresult_not_implemented();
  }

  auto OnNavigatedTo() { return on_navigated_to_; }

 private:
  Windows::Foundation::Collections::IVector<
      coro_cloudbrowser_winrt::CloudProviderTypeModel>
      provider_types_;
  std::function<void()> on_navigated_to_;
};
}  // namespace winrt::coro_cloudbrowser_winrt::implementation

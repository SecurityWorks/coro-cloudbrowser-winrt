#pragma once

#include <coro/cloudstorage/util/cloud_provider_account.h>
#include <coro/util/event_loop.h>

#include "CloudProviderAccountModel.g.h"

namespace winrt::coro_cloudbrowser_winrt::implementation {

class CloudProviderAccountModel
    : public CloudProviderAccountModelT<CloudProviderAccountModel> {
 public:
  CloudProviderAccountModel() : event_loop_() {}

  CloudProviderAccountModel(
      coro::util::EventLoop* event_loop,
      std::shared_ptr<coro::cloudstorage::util::CloudProviderAccount> account);

  hstring ImageSource() const;
  void ImageSource(hstring);

  hstring Label() const;
  void Label(hstring);

  auto Id() const { return account_->id(); }

  static winrt::fire_and_forget final_release(
      std::unique_ptr<CloudProviderAccountModel> ptr) noexcept;

 private:
  coro::util::EventLoop* event_loop_;
  std::shared_ptr<coro::cloudstorage::util::CloudProviderAccount> account_;
  hstring image_source_;
  hstring label_;
};

}  // namespace winrt::coro_cloudbrowser_winrt::implementation

namespace winrt::coro_cloudbrowser_winrt::factory_implementation {

struct CloudProviderAccountModel
    : CloudProviderAccountModelT<CloudProviderAccountModel,
                                 implementation::CloudProviderAccountModel> {};

}  // namespace winrt::coro_cloudbrowser_winrt::factory_implementation

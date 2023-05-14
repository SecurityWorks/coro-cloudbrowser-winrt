#pragma once

#include <coro/cloudstorage/util/cloud_provider_account.h>
#include <coro/util/event_loop.h>

#include "CloudProviderAccountModel.g.h"

namespace winrt::coro_cloudbrowser_winrt::implementation {

class CloudProviderAccountModel
    : public CloudProviderAccountModelT<CloudProviderAccountModel> {
 public:
  CloudProviderAccountModel(
      coro::util::EventLoop* event_loop,
      coro::cloudstorage::util::CloudProviderAccount account);

  hstring ImageSource() const;
  void ImageSource(hstring);

  hstring Label() const;
  void Label(hstring);

  auto Id() const { return account_->id(); }

  concurrency::task<coro::cloudstorage::util::AbstractCloudProvider::Directory>
      GetRoot(concurrency::cancellation_token) const;

  concurrency::task<coro::cloudstorage::util::AbstractCloudProvider::Item>
  GetItemByPath(std::string path, concurrency::cancellation_token) const;

  concurrency::task<coro::cloudstorage::util::AbstractCloudProvider::PageData>
  ListDirectoryPage(
      coro::cloudstorage::util::AbstractCloudProvider::Directory directory,
      std::optional<std::string> page_token,
      concurrency::cancellation_token) const;

  static winrt::fire_and_forget final_release(
      std::unique_ptr<CloudProviderAccountModel> ptr) noexcept;

 private:
  coro::util::EventLoop* event_loop_;
  std::optional<coro::cloudstorage::util::CloudProviderAccount> account_;
  hstring image_source_;
  hstring label_;
};

}  // namespace winrt::coro_cloudbrowser_winrt::implementation


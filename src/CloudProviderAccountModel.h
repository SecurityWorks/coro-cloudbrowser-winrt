#pragma once

#include <coro/cloudstorage/util/cache_manager.h>
#include <coro/cloudstorage/util/clock.h>
#include <coro/cloudstorage/util/cloud_provider_account.h>
#include <coro/cloudstorage/util/cloud_provider_utils.h>
#include <coro/util/event_loop.h>

#include "CloudProviderAccountModel.g.h"

namespace winrt::coro_cloudbrowser_winrt::implementation {

hstring ToString(const coro::cloudstorage::util::CloudProviderAccount::Id&);

class CloudProviderAccountModel
    : public CloudProviderAccountModelT<CloudProviderAccountModel> {
 public:
  CloudProviderAccountModel(
      coro::util::EventLoop* event_loop,
      const coro::cloudstorage::util::Clock* clock,
      coro::cloudstorage::util::CloudProviderCacheManager cache_manager,
      coro::cloudstorage::util::CloudProviderAccount account);

  hstring ImageSource() const;
  void ImageSource(hstring);

  hstring Label() const;
  void Label(hstring);

  auto Id() const { return account_->id(); }
  const auto* Provider() const { return account_->provider().get(); }

  concurrency::task<coro::cloudstorage::util::AbstractCloudProvider::Item>
  GetItemById(std::string id, concurrency::cancellation_token) const;

  coro::Task<coro::cloudstorage::util::VersionedDirectoryContent> ListDirectory(
      coro::cloudstorage::util::AbstractCloudProvider::Directory directory,
      concurrency::cancellation_token);

  concurrency::task<coro::cloudstorage::util::AbstractCloudProvider::PageData>
  ListDirectoryPage(
      coro::cloudstorage::util::AbstractCloudProvider::Directory directory,
      std::optional<std::string> page_token,
      concurrency::cancellation_token) const;

  static winrt::fire_and_forget final_release(
      std::unique_ptr<CloudProviderAccountModel> ptr) noexcept;

 private:
  coro::util::EventLoop* event_loop_;
  const coro::cloudstorage::util::Clock* clock_;
  std::optional<coro::cloudstorage::util::CloudProviderAccount> account_;
  std::optional<coro::cloudstorage::util::CloudProviderCacheManager>
      cache_manager_;
  hstring image_source_;
  hstring label_;
};

}  // namespace winrt::coro_cloudbrowser_winrt::implementation

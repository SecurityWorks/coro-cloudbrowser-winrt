#include "CloudProviderAccountModel.h"
#if __has_include("CloudProviderAccountModel.g.cpp")
#include "CloudProviderAccountModel.g.cpp"
#endif

#include <coro/cloudstorage/util/cloud_provider_utils.h>
#include <coro/cloudstorage/util/string_utils.h>
#include <coro/http/http_parse.h>
#include <coro/promise.h>
#include <coro/util/raii_utils.h>
#include <fmt/format.h>

#include <nlohmann/json.hpp>

#include "EventLoopUtils.h"

namespace winrt::coro_cloudbrowser_winrt::implementation {

namespace {

using ::coro::cloudstorage::util::AbstractCloudProvider;
using ::coro::cloudstorage::util::CloudProviderAccount;
using ::coro::cloudstorage::util::GetItemByPathComponents;
using ::coro::cloudstorage::util::SplitString;
using ::coro::http::DecodeUri;
using ::coro::util::AtScopeExit;

template <typename T, typename F>
concurrency::task<T> RunWinRt(coro::util::EventLoop* event_loop,
                              concurrency::cancellation_token token,
                              CloudProviderAccount p, F func) {
  winrt::apartment_context ui_thread;
  std::optional<std::variant<T, std::exception_ptr>> result;
  co_await coro::cloudbrowser::util::SwitchTo(event_loop);
  {
    coro::stdx::stop_source stop_source;
    auto token_registration =
        token.register_callback([event_loop, stop_source]() mutable {
          event_loop->RunOnEventLoop(
              [stop_source = std::move(stop_source)]() mutable {
                stop_source.request_stop();
              });
        });
    auto token_deregistration =
        AtScopeExit([&] { token.deregister_callback(token_registration); });
    coro::stdx::stop_callback stop_callback(
        p.stop_token(), [&] { stop_source.request_stop(); });
    try {
      result.emplace(co_await func(&p, stop_source.get_token()));
    } catch (...) {
      result.emplace(std::current_exception());
    }
  }
  co_await ui_thread;
  if (std::holds_alternative<std::exception_ptr>(*result)) {
    std::rethrow_exception(std::get<std::exception_ptr>(*result));
  }
  co_return std::get<T>(std::move(*result));
}

template <typename T, typename... FArgs, typename... Args>
concurrency::task<T> RunWinRt(
    coro::util::EventLoop* event_loop, concurrency::cancellation_token token,
    CloudProviderAccount p,
    coro::Task<T> (AbstractCloudProvider::*func)(FArgs...) const,
    Args... args) {
  return RunWinRt<T>(
      event_loop, std::move(token), std::move(p),
      [... args = std::move(args), func](CloudProviderAccount* account,
                                         coro::stdx::stop_token stop_token) {
        return std::invoke(func, account->provider().get(), std::move(args)...,
                           std::move(stop_token));
      });
}

std::vector<std::string> GetPathComponents(std::string path) {
  std::stringstream stream;
  std::vector<std::string> components;
  for (std::string_view component : SplitString(path, '/')) {
    components.push_back(DecodeUri(component));
  }
  return components;
}

}  // namespace

hstring ToString(const coro::cloudstorage::util::CloudProviderAccount::Id& id) {
  nlohmann::json json;
  json["type"] = id.type;
  json["username"] = id.username;
  return to_hstring(json.dump());
}

CloudProviderAccountModel::CloudProviderAccountModel(
    coro::util::EventLoop* event_loop,
    coro::cloudstorage::util::CloudProviderCacheManager cache_manager,
    coro::cloudstorage::util::CloudProviderAccount account)
    : event_loop_(event_loop),
      account_(std::move(account)),
      cache_manager_(std::move(cache_manager)),
      label_(to_hstring(account_->username())),
      image_source_(to_hstring(fmt::format("ms-appx:///Assets/providers/{}.png",
                                           account_->type()))) {}

hstring CloudProviderAccountModel::ImageSource() const { return image_source_; }

void CloudProviderAccountModel::ImageSource(hstring image_source) {
  image_source_ = std::move(image_source);
}

hstring CloudProviderAccountModel::Label() const { return label_; }

void CloudProviderAccountModel::Label(hstring label) {
  label_ = std::move(label);
}

winrt::fire_and_forget CloudProviderAccountModel::final_release(
    std::unique_ptr<CloudProviderAccountModel> d) noexcept {
  co_await coro::cloudbrowser::util::SwitchTo(d->event_loop_);
  d->account_.reset();
  d->cache_manager_.reset();
}

concurrency::task<AbstractCloudProvider::PageData>
CloudProviderAccountModel::ListDirectoryPage(
    AbstractCloudProvider::Directory directory,
    std::optional<std::string> page_token,
    concurrency::cancellation_token token) const {
  return RunWinRt(event_loop_, std::move(token), *account_,
                  &AbstractCloudProvider::ListDirectoryPage,
                  std::move(directory), std::move(page_token));
}

concurrency::task<std::optional<std::vector<AbstractCloudProvider::Item>>>
CloudProviderAccountModel::GetDirectoryListCache(
    coro::cloudstorage::util::AbstractCloudProvider::Directory directory,
    concurrency::cancellation_token token) const {
  return RunWinRt<std::optional<std::vector<AbstractCloudProvider::Item>>>(
      event_loop_, std::move(token), *account_,
      [directory = std::move(directory), cache_manager = *cache_manager_](
          CloudProviderAccount*, coro::stdx::stop_token stop_token) mutable
      -> coro::Task<std::optional<std::vector<AbstractCloudProvider::Item>>> {
        return cache_manager.Get(std::move(directory), std::move(stop_token));
      });
}

concurrency::task<AbstractCloudProvider::Item>
CloudProviderAccountModel::GetItemByPath(
    std::string path, concurrency::cancellation_token token) const {
  return RunWinRt<AbstractCloudProvider::Item>(
      event_loop_, std::move(token), *account_,
      [path = std::move(path)](CloudProviderAccount* account,
                               coro::stdx::stop_token stop_token) mutable {
        return GetItemByPathComponents(account->provider().get(),
                                       GetPathComponents(std::move(path)),
                                       std::move(stop_token));
      });
}

concurrency::task<std::optional<AbstractCloudProvider::Item>>
CloudProviderAccountModel::GetItemByPathCache(
    std::string path, concurrency::cancellation_token token) const {
  return RunWinRt<std::optional<AbstractCloudProvider::Item>>(
      event_loop_, std::move(token), *account_,
      [path = std::move(path), cache_manager = *cache_manager_](
          CloudProviderAccount*, coro::stdx::stop_token stop_token) mutable {
        return cache_manager.Get(GetPathComponents(std::move(path)),
                                 std::move(stop_token));
      });
}

concurrency::task<void> CloudProviderAccountModel::PutItemByPath(
    std::string path, AbstractCloudProvider::Item item,
    concurrency::cancellation_token token) {
  co_await RunWinRt<std::monostate>(
      event_loop_, std::move(token), *account_,
      [path = std::move(path), cache_manager = *cache_manager_,
       item = std::move(item)](CloudProviderAccount* account,
                               coro::stdx::stop_token stop_token) mutable
      -> coro::Task<std::monostate> {
        co_await cache_manager.Put(GetPathComponents(std::move(path)),
                                   std::move(item), std::move(stop_token));
        co_return std::monostate();
      });
}

concurrency::task<void> CloudProviderAccountModel::PutDirectoryList(
    AbstractCloudProvider::Directory directory,
    std::vector<AbstractCloudProvider::Item> items,
    concurrency::cancellation_token token) const {
  co_await RunWinRt<std::monostate>(
      event_loop_, std::move(token), *account_,
      [directory = std::move(directory), items = std::move(items),
       cache_manager = *cache_manager_](
          CloudProviderAccount* account,
          coro::stdx::stop_token stop_token) mutable
      -> coro::Task<std::monostate> {
        co_await cache_manager.Put(std::move(directory), std::move(items),
                                   std::move(stop_token));
        co_return std::monostate();
      });
}

}  // namespace winrt::coro_cloudbrowser_winrt::implementation

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

concurrency::task<coro::cloudstorage::util::AbstractCloudProvider::Item>
CloudProviderAccountModel::GetItemById(
    std::string id, concurrency::cancellation_token token) const {
  return RunWinRt<AbstractCloudProvider::Item>(
      event_loop_, std::move(token), *account_,
      [id = std::move(id), cache_manager = *cache_manager_](
          CloudProviderAccount* account,
          coro::stdx::stop_token stop_token) mutable {
        return ::coro::cloudstorage::util::GetItemById(
            account->provider().get(), std::move(cache_manager),
            /*updated=*/nullptr, std::move(id), std::move(stop_token));
      });
}

coro::Generator<coro::cloudstorage::util::AbstractCloudProvider::PageData>
CloudProviderAccountModel::ListDirectory(
    AbstractCloudProvider::Directory directory,
    std::shared_ptr<
        coro::Promise<std::optional<std::vector<AbstractCloudProvider::Item>>>>
        updated,
    concurrency::cancellation_token token) {
  auto* event_loop = event_loop_;
  auto account = *account_;
  auto cache_manager = *cache_manager_;

  winrt::apartment_context ui_thread;
  co_await coro::cloudbrowser::util::SwitchTo(event_loop);

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
  coro::stdx::stop_callback stop_callback(account.stop_token(),
                                          [&] { stop_source.request_stop(); });
  auto updated_internal = std::make_shared<
      coro::Promise<std::optional<std::vector<AbstractCloudProvider::Item>>>>();

  std::exception_ptr exception;
  try {
    FOR_CO_AWAIT(
        auto page_data,
        coro::cloudstorage::util::ListDirectory(
            cache_manager, updated_internal, account_->provider().get(),
            std::move(directory), stop_source.get_token())) {
      co_await ui_thread;
      co_yield page_data;
      co_await coro::cloudbrowser::util::SwitchTo(event_loop);
    }
  } catch (...) {
    exception = std::current_exception();
  }

  coro::RunTask([ui_thread, updated_internal = std::move(updated_internal),
                 updated = std::move(updated)]() mutable -> coro::Task<> {
    std::exception_ptr exception;
    try {
      auto items = co_await *updated_internal;
      co_await ui_thread;
      updated->SetValue(std::move(items));
      co_return;
    } catch (...) {
      exception = std::current_exception();
    }
    co_await ui_thread;
    updated->SetException(exception);
  });

  co_await ui_thread;
  if (exception) {
    std::rethrow_exception(exception);
  }
}

}  // namespace winrt::coro_cloudbrowser_winrt::implementation

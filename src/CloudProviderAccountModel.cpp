#include "CloudProviderAccountModel.h"
#if __has_include("CloudProviderAccountModel.g.cpp")
#include "CloudProviderAccountModel.g.cpp"
#endif

#include <coro/promise.h>
#include <fmt/format.h>

namespace winrt::coro_cloudbrowser_winrt::implementation {

CloudProviderAccountModel::CloudProviderAccountModel(
    coro::util::EventLoop* event_loop,
    std::shared_ptr<coro::cloudstorage::util::CloudProviderAccount> account)
    : event_loop_(event_loop),
      account_(std::move(account)),
      label_(to_hstring(account_->username())),
      image_source_(to_hstring(fmt::format(
          "http://localhost:12345/static/{}.png", account_->type()))) {}

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
  if (d->event_loop_ == nullptr) {
    co_return;
  }
  coro::Promise<void> promise;
  d->event_loop_->RunOnEventLoop(
      [account = std::move(d->account_), &promise]() mutable {
        account = nullptr;
        promise.SetValue();
      });
  co_await promise;
}

}  // namespace winrt::coro_cloudbrowser_winrt::implementation

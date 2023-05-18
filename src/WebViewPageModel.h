#pragma once

#include "WebViewPageModel.g.h"

namespace winrt::coro_cloudbrowser_winrt::implementation {
struct WebViewPageModel : WebViewPageModelT<WebViewPageModel> {
  WebViewPageModel(Windows::Foundation::Uri source_uri,
                   std::function<void()> on_navigated_to)
      : source_uri_(std::move(source_uri)),
        on_navigated_to_(std::move(on_navigated_to)) {}

  Windows::Foundation::Uri SourceUri() { return source_uri_; }
  void SourceUri(Windows::Foundation::Uri) { throw hresult_not_implemented(); }

  auto OnNavigatedTo() const { return on_navigated_to_; }

 private:
  Windows::Foundation::Uri source_uri_;
  std::function<void()> on_navigated_to_;
};
}  // namespace winrt::coro_cloudbrowser_winrt::implementation

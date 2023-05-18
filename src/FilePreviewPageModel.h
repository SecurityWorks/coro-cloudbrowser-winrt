#pragma once

#include "FilePreviewPageModel.g.h"

namespace winrt::coro_cloudbrowser_winrt::implementation {
struct FilePreviewPageModel : FilePreviewPageModelT<FilePreviewPageModel> {
  FilePreviewPageModel(hstring filename, hstring uri,
                       std::function<void()> on_navigated_to)
      : filename_(std::move(filename)),
        uri_(std::move(uri)),
        on_navigated_to_(std::move(on_navigated_to)) {}

  hstring Filename() const { return filename_; }
  void Filename(const hstring&) { throw hresult_not_implemented(); }

  hstring Uri() const { return uri_; }
  void Uri(const hstring&) { throw hresult_not_implemented(); }

  auto OnNavigatedTo() const { return on_navigated_to_; }

 private:
  hstring filename_;
  hstring uri_;
  std::function<void()> on_navigated_to_;
};
}  // namespace winrt::coro_cloudbrowser_winrt::implementation

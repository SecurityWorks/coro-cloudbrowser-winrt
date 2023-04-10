#pragma once

#include "FilePreviewPageModel.g.h"

namespace winrt::coro_cloudbrowser_winrt::implementation {
struct FilePreviewPageModel : FilePreviewPageModelT<FilePreviewPageModel> {
  FilePreviewPageModel(hstring filename, hstring uri)
      : filename_(std::move(filename)), uri_(std::move(uri)) {}

  hstring Filename() const { return filename_; }
  void Filename(const hstring&) { throw hresult_not_implemented(); }

  hstring Uri() const { return uri_; }
  void Uri(const hstring&) { throw hresult_not_implemented(); }

 private:
  hstring filename_;
  hstring uri_;
};
}  // namespace winrt::coro_cloudbrowser_winrt::implementation

namespace winrt::coro_cloudbrowser_winrt::factory_implementation {
struct FilePreviewPageModel
    : FilePreviewPageModelT<FilePreviewPageModel,
                            implementation::FilePreviewPageModel> {};
}  // namespace winrt::coro_cloudbrowser_winrt::factory_implementation

#pragma once

#include "CloudProviderAccountModel.h"
#include "FileListPageModel.g.h"

namespace winrt::coro_cloudbrowser_winrt::implementation {
struct FileListPageModel : FileListPageModelT<FileListPageModel> {
  FileListPageModel(coro_cloudbrowser_winrt::CloudProviderAccountModel account,
                    hstring path)
      : account_(std::move(account)), path_(std::move(path)) {}

  coro_cloudbrowser_winrt::CloudProviderAccountModel Account() const {
    return account_;
  }
  void Account(coro_cloudbrowser_winrt::CloudProviderAccountModel account) {
    account_ = std::move(account);
  }

  hstring Path() const { return path_; }
  void Path(hstring path) { path_ = std::move(path); }

 private:
  coro_cloudbrowser_winrt::CloudProviderAccountModel account_;
  hstring path_;
};
}  // namespace winrt::coro_cloudbrowser_winrt::implementation

namespace winrt::coro_cloudbrowser_winrt::factory_implementation {
struct FileListPageModel
    : FileListPageModelT<FileListPageModel, implementation::FileListPageModel> {
};
}  // namespace winrt::coro_cloudbrowser_winrt::factory_implementation

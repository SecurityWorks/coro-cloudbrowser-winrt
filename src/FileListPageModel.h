#pragma once

#include "CloudProviderAccountModel.h"
#include "FileListPageModel.g.h"

namespace winrt::coro_cloudbrowser_winrt::implementation {
struct FileListPageModel : FileListPageModelT<FileListPageModel> {
  FileListPageModel(
      coro_cloudbrowser_winrt::CloudProviderAccountModel account, hstring path,
      Windows::Foundation::EventHandler<hstring> on_item_click,
      Windows::Foundation::EventHandler<Windows::UI::Xaml::RoutedEventArgs>
          on_back_click)
      : account_(std::move(account)),
        path_(std::move(path)),
        on_item_click_(std::move(on_item_click)),
        on_back_click_(std::move(on_back_click)) {}

  coro_cloudbrowser_winrt::CloudProviderAccountModel Account() const {
    return account_;
  }
  void Account(coro_cloudbrowser_winrt::CloudProviderAccountModel account) {
    throw hresult_not_implemented();
  }

  hstring Path() const { return path_; }
  void Path(hstring path) { throw hresult_not_implemented(); }

  Windows::Foundation::EventHandler<hstring> OnItemClick() {
    return on_item_click_;
  }
  void OnItemClick(Windows::Foundation::EventHandler<hstring>) {
    throw hresult_not_implemented();
  }

  Windows::Foundation::EventHandler<Windows::UI::Xaml::RoutedEventArgs>
  OnBackClick() {
    return on_back_click_;
  }
  void OnBackClick(
      Windows::Foundation::EventHandler<Windows::UI::Xaml::RoutedEventArgs>) {
    throw hresult_not_implemented();
  }

 private:
  coro_cloudbrowser_winrt::CloudProviderAccountModel account_;
  hstring path_;
  Windows::Foundation::EventHandler<hstring> on_item_click_;
  Windows::Foundation::EventHandler<Windows::UI::Xaml::RoutedEventArgs>
      on_back_click_;
};
}  // namespace winrt::coro_cloudbrowser_winrt::implementation

namespace winrt::coro_cloudbrowser_winrt::factory_implementation {
struct FileListPageModel
    : FileListPageModelT<FileListPageModel, implementation::FileListPageModel> {
};
}  // namespace winrt::coro_cloudbrowser_winrt::factory_implementation

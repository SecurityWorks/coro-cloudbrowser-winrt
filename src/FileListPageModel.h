#pragma once

#include "CloudProviderAccountModel.h"
#include "FileListEntryModel.h"
#include "FileListPageModel.g.h"

namespace winrt::coro_cloudbrowser_winrt::implementation {
struct FileListPageModel : FileListPageModelT<FileListPageModel> {
  FileListPageModel(coro_cloudbrowser_winrt::CloudProviderAccountModel account,
                    std::function<void()> on_navigated_to, hstring path)
      : account_(std::move(account)),
        on_navigated_to_(std::move(on_navigated_to)),
        path_(std::move(path)) {}

  coro_cloudbrowser_winrt::CloudProviderAccountModel Account() const {
    return account_;
  }
  void Account(coro_cloudbrowser_winrt::CloudProviderAccountModel account) {
    throw hresult_not_implemented();
  }

  hstring Path() const { return path_; }
  void Path(hstring path) { throw hresult_not_implemented(); }

  hstring ScrollPosition() const { return scroll_position_; }
  void ScrollPosition(hstring scroll_position) {
    scroll_position_ = std::move(scroll_position);
  }

  winrt::Windows::Foundation::Collections::IObservableVector<
      coro_cloudbrowser_winrt::FileListEntryModel>
  Items() const {
    return items_;
  }
  void Items(winrt::Windows::Foundation::Collections::IObservableVector<
             coro_cloudbrowser_winrt::FileListEntryModel>) {
    throw hresult_not_implemented();
  }

  auto OnNavigatedTo() const { return on_navigated_to_; }

 private:
  coro_cloudbrowser_winrt::CloudProviderAccountModel account_;
  std::function<void()> on_navigated_to_;
  hstring path_;
  hstring scroll_position_;
  winrt::Windows::Foundation::Collections::IObservableVector<
      coro_cloudbrowser_winrt::FileListEntryModel>
      items_ = winrt::single_threaded_observable_vector<
          coro_cloudbrowser_winrt::FileListEntryModel>();
};
}  // namespace winrt::coro_cloudbrowser_winrt::implementation

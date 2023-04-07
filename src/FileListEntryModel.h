#pragma once

#include <coro/cloudstorage/util/abstract_cloud_provider.h>
#include <coro/cloudstorage/util/cloud_provider_account.h>

#include "FileListEntryModel.g.h"

namespace winrt::coro_cloudbrowser_winrt::implementation {
struct FileListEntryModel : FileListEntryModelT<FileListEntryModel> {
  FileListEntryModel(
      const coro::cloudstorage::util::CloudProviderAccount::Id& account_id,
      std::string_view directory,
      coro::cloudstorage::util::AbstractCloudProvider::Item item);

  void Size(int64_t);
  int64_t Size() const;

  void Id(hstring);
  hstring Id() const;

  void Filename(hstring);
  hstring Filename() const;

  void Thumbnail(hstring);
  hstring Thumbnail() const;

  void ThumbnailVisibility(Windows::UI::Xaml::Visibility);
  Windows::UI::Xaml::Visibility ThumbnailVisibility() const;

  hstring Icon() const;
  void Icon(hstring);

  void IconVisibility(Windows::UI::Xaml::Visibility);
  Windows::UI::Xaml::Visibility IconVisibility() const;

  winrt::event_token PropertyChanged(
      const Windows::UI::Xaml::Data::PropertyChangedEventHandler& value);
  void PropertyChanged(const winrt::event_token& token);

 private:
  hstring thumbnail_uri_;
  coro::cloudstorage::util::AbstractCloudProvider::Item item_;
  Windows::UI::Xaml::Visibility thumbnail_visibility_ =
      Windows::UI::Xaml::Visibility::Collapsed;
  Windows::UI::Xaml::Visibility icon_visibility_ =
      Windows::UI::Xaml::Visibility::Visible;
  winrt::event<Windows::UI::Xaml::Data::PropertyChangedEventHandler>
      property_changed_;
};
}  // namespace winrt::coro_cloudbrowser_winrt::implementation

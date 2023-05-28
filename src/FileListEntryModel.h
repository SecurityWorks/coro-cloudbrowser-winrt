#pragma once

#include <coro/cloudstorage/util/abstract_cloud_provider.h>
#include <coro/cloudstorage/util/cloud_provider_account.h>

#include "FileListEntryModel.g.h"

namespace winrt::coro_cloudbrowser_winrt::implementation {
struct FileListEntryModel : FileListEntryModelT<FileListEntryModel> {
  FileListEntryModel(
      const coro::cloudstorage::util::CloudProviderAccount::Id& account_id,
      const coro::cloudstorage::util::AbstractCloudProvider::Item& item);

  void Size(hstring);
  hstring Size() const;

  void Id(hstring);
  hstring Id() const;

  void Filename(hstring);
  hstring Filename() const;

  void Timestamp(hstring);
  hstring Timestamp() const;

  void Thumbnail(hstring);
  hstring Thumbnail() const;

  void ThumbnailVisibility(Windows::UI::Xaml::Visibility);
  Windows::UI::Xaml::Visibility ThumbnailVisibility() const;

  hstring Icon() const;
  void Icon(hstring);

  void IconVisibility(Windows::UI::Xaml::Visibility);
  Windows::UI::Xaml::Visibility IconVisibility() const;

  FileType Type() const;
  void Type(FileType);

  hstring Uri() const;
  void Uri(hstring);

  winrt::event_token PropertyChanged(
      const Windows::UI::Xaml::Data::PropertyChangedEventHandler& value);
  void PropertyChanged(const winrt::event_token& token);

  void Update(const coro_cloudbrowser_winrt::FileListEntryModel&);

 private:
  hstring size_;
  hstring id_;
  hstring filename_;
  hstring timestamp_;
  Windows::UI::Xaml::Visibility thumbnail_visibility_ =
      Windows::UI::Xaml::Visibility::Collapsed;
  Windows::UI::Xaml::Visibility icon_visibility_ =
      Windows::UI::Xaml::Visibility::Visible;
  FileType type_;
  hstring uri_;
  hstring thumbnail_uri_;
  winrt::event<Windows::UI::Xaml::Data::PropertyChangedEventHandler>
      property_changed_;
};
}  // namespace winrt::coro_cloudbrowser_winrt::implementation

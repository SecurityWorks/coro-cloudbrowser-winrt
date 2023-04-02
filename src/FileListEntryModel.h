#pragma once

#include <coro/cloudstorage/util/abstract_cloud_provider.h>

#include "FileListEntryModel.g.h"

namespace winrt::coro_cloudbrowser_winrt::implementation {
struct FileListEntryModel : FileListEntryModelT<FileListEntryModel> {
  FileListEntryModel() = default;
  FileListEntryModel(hstring thumbnail_uri,
                     coro::cloudstorage::util::AbstractCloudProvider::Item item)
      : thumbnail_uri_(std::move(thumbnail_uri)), item_(std::move(item)) {}

  void Size(int64_t);
  int64_t Size() const;

  void Id(hstring);
  hstring Id() const;

  void Filename(hstring);
  hstring Filename() const;

  void Thumbnail(hstring);
  hstring Thumbnail() const;

 private:
  hstring thumbnail_uri_;
  coro::cloudstorage::util::AbstractCloudProvider::Item item_;
};
}  // namespace winrt::coro_cloudbrowser_winrt::implementation

namespace winrt::coro_cloudbrowser_winrt::factory_implementation {
struct FileListEntryModel
    : FileListEntryModelT<FileListEntryModel,
                          implementation::FileListEntryModel> {};
}  // namespace winrt::coro_cloudbrowser_winrt::factory_implementation

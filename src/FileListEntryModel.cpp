#include "FileListEntryModel.h"

#if __has_include("FileListEntryModel.g.cpp")
#include "FileListEntryModel.g.cpp"
#endif

namespace winrt::coro_cloudbrowser_winrt::implementation {

void FileListEntryModel::Size(int64_t) { throw hresult_not_implemented(); }

int64_t FileListEntryModel::Size() const {
  return std::visit([](const auto& d) { return d.size.value_or(-1); }, item_);
}

void FileListEntryModel::Id(hstring) { throw hresult_not_implemented(); }

hstring FileListEntryModel::Id() const {
  return winrt::to_hstring(
      std::visit([](const auto& d) { return d.id; }, item_));
}

void FileListEntryModel::Filename(hstring) { throw hresult_not_implemented(); }

hstring FileListEntryModel::Filename() const {
  return winrt::to_hstring(
      std::visit([](const auto& d) { return d.name; }, item_));
}

void FileListEntryModel::Thumbnail(hstring) { throw hresult_not_implemented(); }

hstring FileListEntryModel::Thumbnail() const { return thumbnail_uri_; }

}  // namespace winrt::coro_cloudbrowser_winrt::implementation

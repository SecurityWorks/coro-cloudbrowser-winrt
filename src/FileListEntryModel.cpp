#include "FileListEntryModel.h"

#if __has_include("FileListEntryModel.g.cpp")
#include "FileListEntryModel.g.cpp"
#endif

#include <coro/cloudstorage/util/abstract_cloud_provider.h>
#include <coro/cloudstorage/util/cloud_provider_account.h>
#include <coro/cloudstorage/util/cloud_provider_utils.h>
#include <coro/cloudstorage/util/string_utils.h>
#include <coro/http/http_parse.h>
#include <fmt/format.h>

namespace winrt::coro_cloudbrowser_winrt::implementation {

namespace {

using ::coro::cloudstorage::util::AbstractCloudProvider;
using ::coro::cloudstorage::util::CloudProviderAccount;
using ::coro::cloudstorage::util::GetFileType;
using ::coro::cloudstorage::util::StrCat;
using ::coro::http::EncodeUri;
using ::winrt::Windows::UI::Xaml::Visibility;
using ::winrt::Windows::UI::Xaml::Data::PropertyChangedEventArgs;
using ::winrt::Windows::UI::Xaml::Data::PropertyChangedEventHandler;

std::string GetFileUri(const CloudProviderAccount::Id& id,
                       std::string_view path,
                       const AbstractCloudProvider::Item& item) {
  if (const auto* file = std::get_if<AbstractCloudProvider::File>(&item);
      file && [&] {
        switch (GetFileType(file->mime_type)) {
          case coro::cloudstorage::util::FileType::kImage:
          case coro::cloudstorage::util::FileType::kVideo:
            return true;
          default:
            return false;
        }
      }()) {
    return fmt::format(CORO_CLOUDSTORAGE_REDIRECT_URI "/list/{}/{}{}", id.type,
                       EncodeUri(id.username),
                       StrCat(path, EncodeUri(file->name)));
  } else {
    return "";
  }
}

hstring GetTimestamp(const AbstractCloudProvider::Item& item) {
  std::optional<int64_t> timestamp =
      std::visit([](const auto& d) { return d.timestamp; }, item);
  if (!timestamp) {
    return L"";
  }
  Windows::Globalization::DateTimeFormatting::DateTimeFormatter formatter(
      L"{year.full}.{month.integer(2)}.{day.integer(2)} "
      L"{hour.integer(2)}:{minute.integer(2)}",
      Windows::Globalization::ApplicationLanguages::Languages(),
      Windows::Globalization::GeographicRegion().CodeTwoLetter(),
      Windows::Globalization::CalendarIdentifiers::Gregorian(),
      Windows::Globalization::ClockIdentifiers::TwentyFourHour());
  return formatter.Format(winrt::clock::from_time_t(*timestamp));
}

}  // namespace

FileListEntryModel::FileListEntryModel(
    const coro::cloudstorage::util::CloudProviderAccount::Id& account_id,
    std::string_view directory,
    coro::cloudstorage::util::AbstractCloudProvider::Item item)
    : uri_(to_hstring(GetFileUri(account_id, directory, item))),
      item_(std::move(item)),
      timestamp_(GetTimestamp(item_)) {}

void FileListEntryModel::Size(hstring) { throw hresult_not_implemented(); }

hstring FileListEntryModel::Size() const {
  int64_t bytes =
      std::visit([](const auto& d) { return d.size.value_or(-1); }, item_);
  if (bytes < 0) {
    return L"";
  } else if (bytes < 1024) {
    return to_hstring(fmt::format("{} B", bytes));
  } else if (bytes < 1024 * 1024) {
    return to_hstring(fmt::format("{:.2f} KB", bytes / 1024.f));
  } else if (bytes < 1024 * 1024 * 1024) {
    return to_hstring(fmt::format("{:.2f} MB", bytes / 1024.f / 1024.f));
  } else {
    return to_hstring(
        fmt::format("{:.2f} GB", bytes / 1024.f / 1024.f / 1024.f));
  }
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

void FileListEntryModel::Timestamp(hstring) { throw hresult_not_implemented(); }

hstring FileListEntryModel::Timestamp() const { return timestamp_; }

void FileListEntryModel::Thumbnail(hstring) { throw hresult_not_implemented(); }

hstring FileListEntryModel::Thumbnail() const {
  switch (Type()) {
    case FileType::kImage:
    case FileType::kVideo:
      return uri_ + L"?thumbnail=true";
    default:
      return L"";
  }
}

void FileListEntryModel::ThumbnailVisibility(Visibility visibility) {
  if (thumbnail_visibility_ != visibility) {
    thumbnail_visibility_ = visibility;
    property_changed_(*this, PropertyChangedEventArgs(L"ThumbnailVisibility"));
  }
}

Visibility FileListEntryModel::ThumbnailVisibility() const {
  return thumbnail_visibility_;
}

hstring FileListEntryModel::Icon() const {
  switch (Type()) {
    case FileType::kDirectory:
      return L"Folder";
    case FileType::kAudio:
      return L"Audio";
    case FileType::kVideo:
      return L"Video";
    case FileType::kImage:
      return L"Camera";
    default:
      return L"Document";
  }
}

void FileListEntryModel::Icon(hstring) { throw hresult_not_implemented(); }

void FileListEntryModel::IconVisibility(Visibility visibility) {
  if (icon_visibility_ != visibility) {
    icon_visibility_ = visibility;
    property_changed_(*this, PropertyChangedEventArgs(L"IconVisibility"));
  }
}

Visibility FileListEntryModel::IconVisibility() const {
  return icon_visibility_;
}

FileType FileListEntryModel::Type() const {
  if (std::holds_alternative<AbstractCloudProvider::Directory>(item_)) {
    return FileType::kDirectory;
  } else {
    switch (
        GetFileType(std::get<AbstractCloudProvider::File>(item_).mime_type)) {
      case coro::cloudstorage::util::FileType::kAudio:
        return FileType::kAudio;
      case coro::cloudstorage::util::FileType::kVideo:
        return FileType::kVideo;
      case coro::cloudstorage::util::FileType::kImage:
        return FileType::kImage;
      default:
        return FileType::kUnknown;
    }
  }
}

void FileListEntryModel::Type(FileType) { throw hresult_not_implemented(); }

hstring FileListEntryModel::Uri() const { return uri_; }

void FileListEntryModel::Uri(hstring) { throw hresult_not_implemented(); }

winrt::event_token FileListEntryModel::PropertyChanged(
    const PropertyChangedEventHandler& value) {
  return property_changed_.add(value);
}

void FileListEntryModel::PropertyChanged(const winrt::event_token& token) {
  property_changed_.remove(token);
}

}  // namespace winrt::coro_cloudbrowser_winrt::implementation

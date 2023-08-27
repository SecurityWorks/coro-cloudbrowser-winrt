#include "CloudProviderTypeModel.h"

#if __has_include("CloudProviderTypeModel.g.cpp")
#include "CloudProviderTypeModel.g.cpp"
#endif

#include <coro/cloudstorage/util/abstract_cloud_provider.h>
#include <fmt/format.h>

namespace winrt::coro_cloudbrowser_winrt::implementation {

namespace {

using ::coro::cloudstorage::util::AbstractCloudProvider;

std::string GetCloudProviderName(AbstractCloudProvider::Type type) {
  switch (type) {
    case AbstractCloudProvider::Type::kAmazonS3:
      return "Amazon S3";
    case AbstractCloudProvider::Type::kBox:
      return "Box";
    case AbstractCloudProvider::Type::kDropbox:
      return "Dropbox";
    case AbstractCloudProvider::Type::kGoogleDrive:
      return "Google Drive";
    case AbstractCloudProvider::Type::kOpenStack:
      return "OpenStack";
    case AbstractCloudProvider::Type::kLocalFileSystem:
      return "Local Drive";
    case AbstractCloudProvider::Type::kMega:
      return "Mega";
    case AbstractCloudProvider::Type::kOneDrive:
      return "OneDrive";
    case AbstractCloudProvider::Type::kPCloud:
      return "pCloud";
    case AbstractCloudProvider::Type::kWebDAV:
      return "WebDAV";
    case AbstractCloudProvider::Type::kYandexDisk:
      return "Yandex Disk";
    case AbstractCloudProvider::Type::kYouTube:
      return "YouTube";
    default:
      return "Unknown";
  }
}

}  // namespace

CloudProviderTypeModel::CloudProviderTypeModel(
    const coro::cloudstorage::util::AbstractCloudFactory& factory,
    coro::cloudstorage::util::AbstractCloudProvider::Type type)
    : name_(to_hstring(GetCloudProviderName(type))),
      image_source_(to_hstring(fmt::format("ms-appx:///Assets/providers/{}.png",
                                           factory.GetAuth(type).GetId()))),
      authorization_url_(
          to_hstring(factory.GetAuth(type).GetAuthorizationUrl().value_or(
              fmt::format(CORO_CLOUDSTORAGE_REDIRECT_URI "/auth/{}",
                          factory.GetAuth(type).GetId())))) {}

}  // namespace winrt::coro_cloudbrowser_winrt::implementation

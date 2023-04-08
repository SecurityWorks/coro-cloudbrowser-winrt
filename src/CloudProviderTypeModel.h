#pragma once

#include <coro/cloudstorage/util/abstract_cloud_factory.h>

#include "CloudProviderTypeModel.g.h"

namespace winrt::coro_cloudbrowser_winrt::implementation {
struct CloudProviderTypeModel
    : CloudProviderTypeModelT<CloudProviderTypeModel> {
  CloudProviderTypeModel(
      const coro::cloudstorage::util::AbstractCloudFactory&,
      coro::cloudstorage::util::AbstractCloudProvider::Type type);

  hstring Name() const { return name_; }
  void Name(hstring) { throw hresult_not_implemented(); }

  hstring ImageSource() const { return image_source_; }
  void ImageSource(hstring) { throw hresult_not_implemented(); }

  hstring AuthorizationUrl() const { return authorization_url_; }
  void AuthorizationUrl(hstring) { throw hresult_not_implemented(); }

 private:
  hstring name_;
  hstring image_source_;
  hstring authorization_url_;
};
}  // namespace winrt::coro_cloudbrowser_winrt::implementation

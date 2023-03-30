#pragma once

#include "CloudProviderAccountModel.g.h"

namespace winrt::coro_cloudbrowser_winrt::implementation {

class CloudProviderAccountModel
    : public CloudProviderAccountModelT<CloudProviderAccountModel> {
 public:
  hstring ImageSource() const;
  void ImageSource(hstring);

  hstring Label() const;
  void Label(hstring);

 private:
  hstring image_source_;
  hstring label_;
};

}  // namespace winrt::coro_cloudbrowser_winrt::implementation

namespace winrt::coro_cloudbrowser_winrt::factory_implementation {

struct CloudProviderAccountModel
    : CloudProviderAccountModelT<CloudProviderAccountModel,
                                implementation::CloudProviderAccountModel> {};

}  // namespace winrt::coro_cloudbrowser_winrt::factory_implementation

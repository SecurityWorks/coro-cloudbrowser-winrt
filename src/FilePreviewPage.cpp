#include "FilePreviewPage.h"
#if __has_include("FilePreviewPage.g.cpp")
#include "FilePreviewPage.g.cpp"
#endif

namespace winrt::coro_cloudbrowser_winrt::implementation {

using ::winrt::Windows::Foundation::IInspectable;
using ::winrt::Windows::Foundation::Uri;
using ::winrt::Windows::Media::Core::MediaSource;
using ::winrt::Windows::Media::Playback::MediaPlaybackItem;
using ::winrt::Windows::UI::Xaml::RoutedEventArgs;
using ::winrt::Windows::UI::Xaml::Input::KeyRoutedEventArgs;
using ::winrt::Windows::UI::Xaml::Navigation::NavigatingCancelEventArgs;
using ::winrt::Windows::UI::Xaml::Navigation::NavigationEventArgs;

void FilePreviewPage::GridKeyDown(const IInspectable&,
                                  const KeyRoutedEventArgs&) {}

void FilePreviewPage::BackButtonClick(const IInspectable&,
                                      const RoutedEventArgs&) {
  if (Frame().CanGoBack()) {
    Frame().GoBack();
  }
}

void FilePreviewPage::OnNavigatedTo(const NavigationEventArgs& args) {
  auto data = args.Parameter().as<FilePreviewPageModel>();
  Filename().Text(data.Filename());
  MediaPlayer().Source(
      MediaPlaybackItem(MediaSource::CreateFromUri(Uri(data.Uri()))));
}

void FilePreviewPage::OnNavigatingFrom(const NavigatingCancelEventArgs&) {
  MediaPlayer().MediaPlayer().SystemMediaTransportControls().IsEnabled(false);
  MediaPlayer().Source(nullptr);
}

}  // namespace winrt::coro_cloudbrowser_winrt::implementation

#pragma once

#include "FileListPage.g.h"
#include "FileListPageModel.h"

namespace winrt::coro_cloudbrowser_winrt::implementation {

struct FileListPage : FileListPageT<FileListPage> {
  Windows::Foundation::IAsyncAction OnNavigatedTo(
      Windows::UI::Xaml::Navigation::NavigationEventArgs e);

  void OnNavigatedFrom(
      const Windows::UI::Xaml::Navigation::NavigationEventArgs& e);

  void OnGettingFocus(const Windows::UI::Xaml::UIElement&,
                      const Windows::UI::Xaml::Input::GettingFocusEventArgs&);

  void OnKeyDown(const Windows::Foundation::IInspectable&,
                 const Windows::UI::Xaml::Input::KeyRoutedEventArgs&);
  void OnKeyDown(const Windows::UI::Xaml::Input::KeyRoutedEventArgs&);

  void FileListEntryClick(
      const Windows::Foundation::IInspectable&,
      const Windows::UI::Xaml::Controls::ItemClickEventArgs&);

  void FileListEntryCheckboxChecked(const Windows::Foundation::IInspectable&,
                                    const Windows::UI::Xaml::RoutedEventArgs&);
  void FileListEntryCheckboxUnchecked(
      const Windows::Foundation::IInspectable&,
      const Windows::UI::Xaml::RoutedEventArgs&);

  void FileListEntryThumbnailImageOpened(
      const Windows::Foundation::IInspectable&,
      const Windows::UI::Xaml::RoutedEventArgs&);

  void FileListEntryThumbnailImageFailed(
      const Windows::Foundation::IInspectable&,
      const Windows::UI::Xaml::ExceptionRoutedEventArgs&);

  void FileListEntryThumbnailImageLoaded(
      const Windows::Foundation::IInspectable&,
      const Windows::UI::Xaml::RoutedEventArgs&);

  void BackButtonClick(const Windows::Foundation::IInspectable&,
                       const Windows::UI::Xaml::RoutedEventArgs&);

  void CreateDirectoryClick(const Windows::Foundation::IInspectable&,
                            const Windows::UI::Xaml::RoutedEventArgs&);

  void UploadClick(const Windows::Foundation::IInspectable&,
                   const Windows::UI::Xaml::RoutedEventArgs&);

  void DeleteClick(const Windows::Foundation::IInspectable&,
                   const Windows::UI::Xaml::RoutedEventArgs&);

  void RenameClick(const Windows::Foundation::IInspectable&,
                   const Windows::UI::Xaml::RoutedEventArgs&);

  void MoveClick(const Windows::Foundation::IInspectable&,
                 const Windows::UI::Xaml::RoutedEventArgs&);
  void MoveCancelClick(const Windows::Foundation::IInspectable&,
                       const Windows::UI::Xaml::RoutedEventArgs&);

  void DownloadClick(const Windows::Foundation::IInspectable&,
                     const Windows::UI::Xaml::RoutedEventArgs&);

  void RefreshClick(const Windows::Foundation::IInspectable&,
                    const Windows::UI::Xaml::RoutedEventArgs&);

 private:
  Windows::Foundation::IAsyncAction RefreshContent(
      coro_cloudbrowser_winrt::FileListPageModel page_model,
      std::optional<coro::cloudstorage::util::AbstractCloudProvider::Directory>
          cached_parent,
      std::optional<
          std::vector<coro::cloudstorage::util::AbstractCloudProvider::Item>>
          cached_item_list,
      concurrency::cancellation_token);

  coro_cloudbrowser_winrt::FileListPageModel page_model_{nullptr};
};

}  // namespace winrt::coro_cloudbrowser_winrt::implementation

namespace winrt::coro_cloudbrowser_winrt::factory_implementation {
struct FileListPage
    : FileListPageT<FileListPage, implementation::FileListPage> {};
}  // namespace winrt::coro_cloudbrowser_winrt::factory_implementation

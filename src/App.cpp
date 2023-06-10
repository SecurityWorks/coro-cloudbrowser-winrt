#include "App.h"

#include <coro/cloudstorage/util/cache_manager.h>
#include <coro/cloudstorage/util/cloud_factory_context.h>
#include <coro/promise.h>
#include <direct.h>
#include <fmt/format.h>

#include <nlohmann/json.hpp>

#include "EventLoopUtils.h"
#include "MainPage.h"
#include "MainPageModel.h"

namespace winrt::coro_cloudbrowser_winrt::implementation {

namespace {

using ::coro::RunTask;
using ::coro::Task;
using ::coro::cloudstorage::CloudFactory;
using ::coro::cloudstorage::util::AuthData;
using ::coro::cloudstorage::util::CloudFactoryConfig;
using ::coro::cloudstorage::util::CloudFactoryContext;
using ::coro::cloudstorage::util::CloudProviderAccount;
using ::coro::cloudstorage::util::CloudProviderCacheManager;
using ::coro::cloudstorage::util::StrCat;
using ::coro::http::EncodeUri;
using ::coro::util::EventLoop;
using ::winrt::Windows::ApplicationModel::SuspendingEventArgs;
using ::winrt::Windows::ApplicationModel::Activation::ActivationKind;
using ::winrt::Windows::ApplicationModel::Activation::ApplicationExecutionState;
using ::winrt::Windows::ApplicationModel::Activation::IActivatedEventArgs;
using ::winrt::Windows::ApplicationModel::Activation::LaunchActivatedEventArgs;
using ::winrt::Windows::ApplicationModel::Activation::
    ProtocolActivatedEventArgs;
using ::winrt::Windows::ApplicationModel::Core::CoreApplication;
using ::winrt::Windows::Foundation::IAsyncAction;
using ::winrt::Windows::Foundation::Uri;
using ::winrt::Windows::Foundation::Collections::IObservableVector;
using ::winrt::Windows::UI::Core::CoreDispatcherPriority;
using ::winrt::Windows::UI::Popups::MessageDialog;
using ::winrt::Windows::UI::Xaml::UnhandledExceptionEventArgs;
using ::winrt::Windows::UI::Xaml::Window;
using ::winrt::Windows::UI::Xaml::Controls::Frame;
using ::winrt::Windows::UI::Xaml::Navigation::NavigationFailedEventArgs;

constexpr std::string_view kAuthData = R"js({
    "google": {
        "client_id": "459729347355-ubp1a4mvkrfqanpd05h1l1md5le868r6.apps.googleusercontent.com",
        "client_secret": "",
        "redirect_uri": "cloudbrowser.oauth:/google"
    },
    "box": {
        "client_id": "8v59eo0k7cslcwilw4lu6ks53iga804i",
        "client_secret": "nFQhObfGRzVjDZxXFooSNJ8TZykHBC5m",
        "redirect_uri": "cloudbrowser.oauth://box"
    },
    "dropbox": {
        "client_id": "archooga4lwron4",
        "client_secret": "3at3evf2066tzmm",
        "code_verifier": "thaiwarydnftbtnfapsfbrshnlczrqwxiirwtaiwkcaubyqazeazfuiodmqqvrou",
        "redirect_uri": "cloudbrowser.oauth:/dropbox"
    },
    "hubic": {
        "client_id": "api_hubic_Z32tg7yWDuJdyvgXnLtgJy2tRSYvjycR",
        "client_secret": "h2xCpeHq3PCYdyTZty8PBKEUbFbDSslycIimwhE5XbfFGuS4HYU93OiRQX5P40EO"
    },
    "mega": {
        "api_key": "ZVhB0Czb",
        "app_name": "coro-cloudstorage"
    },
    "onedrive": {
        "client_id": "a09406c3-33da-4b91-b24b-aad3c5d118d5",
        "client_secret": "",
        "redirect_uri": "cloudbrowser.oauth://onedrive"
    },
    "pcloud": {
        "client_id": "OwbUz84peXz",
        "client_secret": "imi0pm1eigjQ3OgqfoJSoHS7bqgX",
        "redirect_uri": "cloudbrowser.oauth:/pcloud"
    },
    "yandex": {
        "client_id" : "d8e5d26d6097441bbd8c3eeff7772643",
        "client_secret" : "3aac1fc76ee44f4586b1cb878111465f",
        "redirect_uri": "cloudbrowser.oauth:/yandex"
    }
  })js";

std::string GetPostAuthRedirectUri(std::string_view account_type,
                                   std::string_view username) {
  return StrCat("cloudbrowser:/list-items/", account_type, '/',
                EncodeUri(username), "?path=/");
}

class AccountListener {
 public:
  AccountListener(
      coro::util::EventLoop* event_loop,
      const coro::cloudstorage::util::Clock* clock,
      coro::cloudstorage::util::CacheManager* cache_manager,
      IObservableVector<coro_cloudbrowser_winrt::CloudProviderAccountModel>
          accounts)
      : event_loop_(event_loop),
        clock_(clock),
        cache_manager_(cache_manager),
        accounts_(std::move(accounts)) {}

  winrt::fire_and_forget OnCreate(CloudProviderAccount account) {
    CloudProviderCacheManager cache_manager(account, cache_manager_);
    co_await CoreApplication::MainView().Dispatcher().RunAsync(
        CoreDispatcherPriority::Normal,
        [event_loop = this->event_loop_, clock = this->clock_,
         accounts = this->accounts_, cache_manager = std::move(cache_manager),
         account = std::move(account)]() mutable {
          accounts.Append(make<CloudProviderAccountModel>(
              event_loop, clock, std::move(cache_manager), std::move(account)));
        });
  }

  winrt::fire_and_forget OnDestroy(CloudProviderAccount account) {
    co_await CoreApplication::MainView().Dispatcher().RunAsync(
        CoreDispatcherPriority::Normal,
        [accounts = this->accounts_, account = std::move(account)] {
          for (uint32_t i = 0; i < accounts.Size();) {
            auto entry = accounts.GetAt(i).as<CloudProviderAccountModel>();
            if (entry->Id() == account.id()) {
              accounts.RemoveAt(i);
            } else {
              i++;
            }
          }
        });
  }

 private:
  coro::util::EventLoop* event_loop_;
  const coro::cloudstorage::util::Clock* clock_;
  coro::cloudstorage::util::CacheManager* cache_manager_;
  IObservableVector<coro_cloudbrowser_winrt::CloudProviderAccountModel>
      accounts_;
};

std::vector<coro_cloudbrowser_winrt::CloudProviderTypeModel>
CreateCloudProviderTypes(const CloudFactory& factory) {
  std::vector<coro_cloudbrowser_winrt::CloudProviderTypeModel> types;
  for (auto type : factory.GetSupportedCloudProviders()) {
    types.push_back(winrt::make<CloudProviderTypeModel>(factory, type));
  }
  return types;
}

}  // namespace

Task<> App::RunHttpServer() {
  try {
    auto http_server = context_.CreateHttpServer(
        AccountListener(&event_loop_, &clock_, context_.cache(), accounts_));
    init_semaphore_.SetValue();
    co_await quit_semaphore_;
    co_await http_server.Quit();
  } catch (const coro::Exception& exception) {
    std::stringstream stream;
    stream << exception.what() << '\n';
    OutputDebugStringA(std::move(stream).str().c_str());
  }
}

/// <summary>
/// Creates the singleton application object.  This is the first line of
/// authored code executed, and as such is the logical equivalent of main() or
/// WinMain().
/// </summary>
App::App()
    : context_(&event_loop_,
               CloudFactoryConfig{
                   .post_auth_redirect_uri = GetPostAuthRedirectUri,
                   .auth_data = AuthData(CORO_CLOUDSTORAGE_REDIRECT_URI,
                                         nlohmann::json::parse(kAuthData))}),
      accounts_(single_threaded_observable_vector<
                coro_cloudbrowser_winrt::CloudProviderAccountModel>()),
      provider_types_(single_threaded_observable_vector<
                      coro_cloudbrowser_winrt::CloudProviderTypeModel>(
          CreateCloudProviderTypes(*context_.factory()))) {
  Suspending({this, &App::OnSuspending});

  RequiresPointerMode(
      Windows::UI::Xaml::ApplicationRequiresPointerMode::WhenRequested);

#if defined _DEBUG && \
    !defined DISABLE_XAML_GENERATED_BREAK_ON_UNHANDLED_EXCEPTION
  UnhandledException(
      [this](IInspectable const&, UnhandledExceptionEventArgs const& e) {
        if (IsDebuggerPresent()) {
          auto error_message = e.Message();
          __debugbreak();
        }
      });
#endif
}

/// <summary>
/// Invoked when the application is launched normally by the end user.  Other
/// entry points will be used such as when the application is launched to open a
/// specific file.
/// </summary>
/// <param name="e">Details about the launch request and process.</param>
winrt::fire_and_forget App::OnLaunched(LaunchActivatedEventArgs e) {
  thread_ = std::async(std::launch::async, [&] {
    RunTask(RunHttpServer());
    event_loop_.EnterLoop();
  });

  winrt::apartment_context ui_thread;
  co_await coro::cloudbrowser::util::SwitchTo(&event_loop_);
  co_await init_semaphore_;
  co_await ui_thread;

  Frame root_frame{nullptr};
  auto content = Window::Current().Content();
  if (content) {
    root_frame = content.try_as<Frame>();
  }

  // Do not repeat app initialization when the Window already has content,
  // just ensure that the window is active
  if (root_frame == nullptr) {
    // Create a Frame to act as the navigation context and associate it with
    // a SuspensionManager key
    root_frame = Frame();

    root_frame.NavigationFailed({this, &App::OnNavigationFailed});

    if (e.PreviousExecutionState() == ApplicationExecutionState::Terminated) {
      // Restore the saved session state only when appropriate, scheduling the
      // final launch steps after the restore is complete
    }

    if (e.PrelaunchActivated() == false) {
      if (root_frame.Content() == nullptr) {
        // When the navigation stack isn't restored navigate to the first page,
        // configuring the new page by passing required information as a
        // navigation parameter
        root_frame.Navigate(xaml_typename<coro_cloudbrowser_winrt::MainPage>(),
                            winrt::make<MainPageModel>(
                                accounts_, provider_types_, &deep_link_event_));
      }
      // Place the frame in the current Window
      Window::Current().Content(root_frame);
      // Ensure the current window is active
      Window::Current().Activate();
    }
  } else {
    if (e.PrelaunchActivated() == false) {
      if (root_frame.Content() == nullptr) {
        // When the navigation stack isn't restored navigate to the first page,
        // configuring the new page by passing required information as a
        // navigation parameter
        root_frame.Navigate(xaml_typename<coro_cloudbrowser_winrt::MainPage>(),
                            winrt::make<MainPageModel>(
                                accounts_, provider_types_, &deep_link_event_));
      }
      // Ensure the current window is active
      Window::Current().Activate();
    }
  }
}

/// <summary>
/// Invoked when application execution is being suspended.  Application state is
/// saved without knowing whether the application will be terminated or resumed
/// with the contents of memory still intact.
/// </summary>
/// <param name="sender">The source of the suspend request.</param>
/// <param name="e">Details about the suspend request.</param>
void App::OnSuspending([[maybe_unused]] const IInspectable& sender,
                       [[maybe_unused]] const SuspendingEventArgs& e) {
  // Save application state and stop any background activity
}

/// <summary>
/// Invoked when Navigation to a certain page fails
/// </summary>
/// <param name="sender">The Frame which failed navigation</param>
/// <param name="e">Details about the navigation failure</param>
void App::OnNavigationFailed(const IInspectable&,
                             const NavigationFailedEventArgs& e) {
  throw hresult_error(
      E_FAIL, hstring(L"Failed to load Page ") + e.SourcePageType().Name);
}

winrt::fire_and_forget App::OnActivated(const IActivatedEventArgs& args) {
  if (args.Kind() == ActivationKind::Protocol) {
    auto protocol_event_args = args.as<ProtocolActivatedEventArgs>();
    auto uri = protocol_event_args.Uri();
    if (uri.SchemeName() == L"cloudbrowser.oauth") {
      auto response = co_await http_.GetAsync(Uri(to_hstring(fmt::format(
          CORO_CLOUDSTORAGE_REDIRECT_URI "/auth{}{}",
          to_string(!uri.Host().empty() ? (L"/" + uri.Host()) : uri.Path()),
          to_string(uri.Query())))));
      if (auto location = response.Headers().Location()) {
        co_await Windows::System::Launcher::LaunchUriAsync(location);
      } else {
        auto body = co_await response.Content().ReadAsStringAsync();
        co_await MessageDialog{body, L"Failed to add a new account."}
            .ShowAsync();
      }
    } else if (uri.SchemeName() == L"cloudbrowser") {
      deep_link_event_(*this, uri);
    }
  }
}

}  // namespace winrt::coro_cloudbrowser_winrt::implementation
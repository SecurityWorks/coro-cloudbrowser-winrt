#include "App.h"

#include <coro/cloudstorage/util/cloud_factory_context.h>
#include <coro/promise.h>
#include <direct.h>

#include "MainPage.h"

namespace winrt::coro_cloudbrowser_winrt::implementation {

namespace {

using ::coro::RunTask;
using ::coro::Task;
using ::coro::cloudstorage::util::CloudFactoryConfig;
using ::coro::cloudstorage::util::CloudFactoryContext;
using ::coro::cloudstorage::util::CloudProviderAccount;
using ::coro::util::EventLoop;
using ::winrt::Windows::ApplicationModel::SuspendingEventArgs;
using ::winrt::Windows::ApplicationModel::Activation::ApplicationExecutionState;
using ::winrt::Windows::ApplicationModel::Activation::LaunchActivatedEventArgs;
using ::winrt::Windows::ApplicationModel::Core::CoreApplication;
using ::winrt::Windows::Foundation::IAsyncAction;
using ::winrt::Windows::Foundation::Collections::IObservableVector;
using ::winrt::Windows::UI::Core::CoreDispatcherPriority;
using ::winrt::Windows::UI::Xaml::UnhandledExceptionEventArgs;
using ::winrt::Windows::UI::Xaml::Window;
using ::winrt::Windows::UI::Xaml::Controls::Frame;
using ::winrt::Windows::UI::Xaml::Navigation::NavigationFailedEventArgs;

class AccountListener {
 public:
  AccountListener(
      coro::util::EventLoop* event_loop,
      IObservableVector<coro_cloudbrowser_winrt::CloudProviderAccountModel>
          accounts)
      : event_loop_(event_loop), accounts_(std::move(accounts)) {}

  winrt::fire_and_forget OnCreate(
      std::shared_ptr<CloudProviderAccount> account) {
    co_await CoreApplication::MainView().Dispatcher().RunAsync(
        CoreDispatcherPriority::Normal,
        [event_loop = this->event_loop_, accounts = this->accounts_,
         account = std::move(account)] {
          accounts.Append(
              make<CloudProviderAccountModel>(event_loop, std::move(account)));
        });
  }

  winrt::fire_and_forget OnDestroy(
      std::shared_ptr<CloudProviderAccount> account) {
    co_await CoreApplication::MainView().Dispatcher().RunAsync(
        CoreDispatcherPriority::Normal,
        [accounts = this->accounts_, account = std::move(account)] {
          for (uint32_t i = 0; i < accounts.Size();) {
            auto entry = accounts.GetAt(i).as<CloudProviderAccountModel>();
            if (entry->Id() == account->id()) {
              accounts.RemoveAt(i);
            } else {
              i++;
            }
          }
        });
  }

 private:
  coro::util::EventLoop* event_loop_;
  IObservableVector<coro_cloudbrowser_winrt::CloudProviderAccountModel>
      accounts_;
};

}  // namespace

Task<> App::RunHttpServer() {
  try {
    auto http_server =
        context_.CreateHttpServer(AccountListener(&event_loop_, accounts_));
    co_await semaphore_;
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
    : context_(&event_loop_, CloudFactoryConfig()),
      accounts_(single_threaded_observable_vector<
                coro_cloudbrowser_winrt::CloudProviderAccountModel>()) {
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
void App::OnLaunched(LaunchActivatedEventArgs const& e) {
  thread_ = std::async(std::launch::async, [&] {
    RunTask(RunHttpServer());
    event_loop_.EnterLoop();
  });

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
                            accounts_);
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
                            accounts_);
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
void App::OnSuspending([[maybe_unused]] IInspectable const& sender,
                       [[maybe_unused]] SuspendingEventArgs const& e) {
  // Save application state and stop any background activity
}

/// <summary>
/// Invoked when Navigation to a certain page fails
/// </summary>
/// <param name="sender">The Frame which failed navigation</param>
/// <param name="e">Details about the navigation failure</param>
void App::OnNavigationFailed(IInspectable const&,
                             NavigationFailedEventArgs const& e) {
  throw hresult_error(
      E_FAIL, hstring(L"Failed to load Page ") + e.SourcePageType().Name);
}

}  // namespace winrt::coro_cloudbrowser_winrt::implementation
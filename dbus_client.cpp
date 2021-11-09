#include <giomm.h>
#include <iostream>

Glib::RefPtr<Glib::MainLoop> loop;

// A main loop idle callback to quit when the main loop is idle.
bool on_main_loop_idle() {
  std::cout << "loop_idle\n";
  loop->quit();
  return false;
}

void on_dbus_proxy_available(Glib::RefPtr<Gio::AsyncResult>& result)
{
  auto proxy = Gio::DBus::Proxy::create_finish(result);

  if(!proxy) {
    std::cerr << "The proxy to the user's session bus was not successfully "
      "created." << std::endl;
    loop->quit();
    return;
  }

  try {
    std::cout << "Calling...\n";

    const auto msg = Glib::Variant<Glib::ustring>::create("hello from client");
    Glib::VariantContainerBase request = Glib::VariantContainerBase::create_tuple(msg);

    auto ret = proxy->call_sync("Method", request);
    Glib::Variant<bool> param;
    ret.get_child(param);

    std::cout << "from server " << param.get() << "\n";
  }
  catch(const Glib::Error& error) {
    std::cerr << "Got an error: '" << error.what() << "'." << std::endl;
  }

  // Connect an idle callback to the main loop to quit when the main loop is
  // idle now that the method call is finished.
  Glib::signal_idle().connect(sigc::ptr_fun(&on_main_loop_idle));
}

int client_main() {
  loop = Glib::MainLoop::create();

  auto connection =
    Gio::DBus::Connection::get_sync(Gio::DBus::BUS_TYPE_SESSION);

  if(!connection) {
    std::cerr << "The user's session bus is not available." << std::endl;
    return 1;
  }

  // Create the proxy to the bus asynchronously.
  Gio::DBus::Proxy::create(connection, "org.glibmm.DBusExample",
    "/org/glibmm/DBusExample", "org.glibmm.DBusExample",
    sigc::ptr_fun(&on_dbus_proxy_available));

  loop->run();

  return EXIT_SUCCESS;
}

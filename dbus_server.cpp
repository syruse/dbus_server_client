#include <giomm.h>
#include <iostream>

namespace {
static Glib::RefPtr<Gio::DBus::NodeInfo> introspection_data;

//https://opensourcelibs.com/lib/gdbus-codegen-glibmm
//you can use gdbus-codegen-glibmm proxy\stub cpp generator instead this
static Glib::ustring introspection_xml =
  "<node name='/org/glibmm/DBusExample'>"
  "  <interface name='org.glibmm.DBusExample'>"
  "    <method name='Method'>"
  "      <arg type='s' name='msg' direction='in'/>"
  "      <arg type='b' name='ret' direction='out'/>"
  "    </method>"
  "  </interface>"
  "</node>";

guint registered_id = 0;
}

static void on_method_call(const Glib::RefPtr<Gio::DBus::Connection>&  connection,
  const Glib::ustring& /* sender */, const Glib::ustring& /* object_path */,
  const Glib::ustring& /* interface_name */, const Glib::ustring& method_name,
  const Glib::VariantContainerBase& parameters,
  const Glib::RefPtr<Gio::DBus::MethodInvocation>& invocation)
{
  if(method_name == "Method") {

    Glib::Variant<Glib::ustring> param;
    parameters.get_child(param);
    const Glib::ustring msg = param.get();
    std::cout << "msg from client "<< msg.c_str();

    const auto reply = Glib::Variant<bool>::create(true);
    Glib::VariantContainerBase response = Glib::VariantContainerBase::create_tuple(reply);
    invocation->return_value(response);
    std::cout << "Method was called\n";
  }
}

const Gio::DBus::InterfaceVTable interface_vtable(sigc::ptr_fun(&on_method_call));

void on_bus_acquired(const Glib::RefPtr<Gio::DBus::Connection>& connection, const Glib::ustring& /* name */)
{
  std::cout << "on_bus_acquired\n";
  try {
    registered_id = connection->register_object("/org/glibmm/DBusExample",
      introspection_data->lookup_interface(),
      interface_vtable);
  }
  catch(const Glib::Error& ex) {
    std::cerr << "Registration of object failed." << std::endl;
  }

  return;
}

void on_name_acquired(const Glib::RefPtr<Gio::DBus::Connection>& /* connection */, const Glib::ustring& /* name */)
{}

void on_name_lost(const Glib::RefPtr<Gio::DBus::Connection>& connection, const Glib::ustring& /* name */) {
  connection->unregister_object(registered_id);
}

int server_main()
{
  try {
    introspection_data = Gio::DBus::NodeInfo::create_for_xml(introspection_xml);
  }
  catch(const Glib::Error& ex) {
    std::cerr << "Unable to create introspection data: " << ex.what() <<
      "." << std::endl;
    return 1;
  }

  const guint id = Gio::DBus::own_name(Gio::DBus::BUS_TYPE_SESSION,
    "org.glibmm.DBusExample",
    sigc::ptr_fun(&on_bus_acquired),
    sigc::ptr_fun(&on_name_acquired),
    sigc::ptr_fun(&on_name_lost));

  //Keep the service running
  auto loop = Glib::MainLoop::create();
  loop->run();

  Gio::DBus::unown_name(id);

  return EXIT_SUCCESS;
}

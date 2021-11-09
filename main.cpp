#include <giomm.h>
#include <iostream>

int server_main();
int client_main();

int main() {
  Gio::init();
  auto pid = getpid();
  fork();
  if (pid == getpid()) {
      std::cout << "server started\n";
      server_main();
  } else {
      sleep(1);
      std::cout << "client started\n";
      client_main();
  }
}

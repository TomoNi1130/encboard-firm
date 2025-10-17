#include "AMT21.hpp"
#include "QEI.hpp"
#include "mbed.h"

BufferedSerial pc{USBTX, USBRX, 9200};

Timer timer;
// DigitalOut pin(PA_8);

AMT21 amt(PA_9, PA_10, (int)2e6, PA_7);

std::vector<uint8_t> addresses;

int main() {
  std::map<uint8_t, int32_t> real_pos;  // pos + turn*rotate
  std::map<uint8_t, int32_t> offset;
  int row = 0;
  int col = 0;
  uint8_t address = 0x00;
  timer.start();
  auto pre = timer.elapsed_time();
  // pin = 0;
  printf("start\r\n");
  while (true) {
    auto now = timer.elapsed_time();
    if (now - pre > 100ms) {
      // pin = !pin;
      amt.request_all(addresses);
      for (uint8_t address : addresses) {
        real_pos[address] = amt.pos_[address] + amt.turn_[address] * AMT21::rotate;
        printf("[0x%x]: %ld ", address, real_pos[address]);
        printf("\n");
      }
      printf("...\r\n");
      address = (row * 4) | (col << 4);
      if (amt.request_pos(address) && amt.request_turn(address)) {
        if (std::find(addresses.begin(), addresses.end(), address) == addresses.end())
          addresses.push_back(address);
      } else if (std::find(addresses.begin(), addresses.end(), address) != addresses.end()) {
        addresses.erase(std::remove(addresses.begin(), addresses.end(), address), addresses.end());
      }
      if (++col >= 16) {
        col = 0;
        if (++row >= 8) {
          row = 0;
        }
      }
      pre = now;
    }
  }
  return 0;
}
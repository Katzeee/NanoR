#include <iostream>

#include "../src/ecs/world.hpp"

struct Position {
  int x;
  int y;
  int z;
};
struct Acc {
  int x;
  int y;
  int z;
};

using namespace xac;
using MyComponents = mpu::type_list<Position, Acc>;
using MySettings = ecs::Settings<MyComponents>;

static ecs::World<MySettings> world;

auto main() -> int {
  auto e = world.create();
  auto e1 = world.create();
  world.assign<Position>(e, 1, 2, 3);
  world.assign<Acc>(e, 1, 2, 3);
  world.assign<Acc>(e1, 1, 2, 3);
  world.each([](auto &&e) {
    std::cout << "id: " << e.id_.id;
    std::cout << ", version: " << e.id_.version;
    std::cout << ", world: " << e.world_;
    std::cout << ", bitset: " << e.components_mask_.to_string();
    std::cout << std::endl;
  });
}
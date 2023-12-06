#include "../src/ecs/world.hpp"

using namespace xac;
using MyComponents = mpu::type_list<int, double>;
using MySettings = ecs::Settings<MyComponents>;

static ecs::World<MySettings> world;

int main() {
  auto e = world.create();
  auto e1 = world.create();
}
// This file is part of multiplayer-game_server <https://github.com/harryjduke/multiplayer-game_server>.
// Copyright (c) 2024 Harry Duke <harryjduke@gmail.com>
// This file includes modifications made by Harry Duke.
//
// This program is distributed under the terms of the GNU General Public License version 2.
// You should have received a copy of the GNU General Public License along with this program.
// If not, see <https://github.com/harryjduke/multiplayer-game_server/blob/main/LICENSE> or <https://www.gnu.org/licenses/>.

#include <gtest/gtest.h>

#include "NetworkEngine.h"

class NetworkEngineTest : public ::testing::Test {
protected:
    void SetUp() override {
        networkEngine = std::make_unique<NetworkEngine>();
    }

    void TearDown() override {
        networkEngine.reset();
    }

    std::unique_ptr<NetworkEngine> networkEngine;
};

class TestObject {
public:
  explicit TestObject(NetworkEngine &networkEngine)
    : registeredNetworkEngine(networkEngine) {
    networkID = registeredNetworkEngine.registerReplicatedObject(std::vector<void *>{&testBool});
  }

  ~TestObject() {
    registeredNetworkEngine.unregisterReplicatedObject(networkID);
  }

  void setTestBool(const bool newTestBool) {
    testBool = newTestBool;
  }

private:
  bool testBool{};
  NetworkEngine &registeredNetworkEngine;
  uint64_t networkID;
};

TEST_F(NetworkEngineTest, CreateReplicatedObect) {
  auto testObject = std::make_unique<TestObject>(*networkEngine);

  ASSERT_FALSE(*static_cast<bool*>(networkEngine->getReplicatedObjects()[0][0]));
}

TEST_F(NetworkEngineTest, CreateTwoReplicatedObjects) {
  auto testObject0 = std::make_unique<TestObject>(*networkEngine);
  auto testObject1 = std::make_unique<TestObject>(*networkEngine);
  testObject1->setTestBool(true);

  ASSERT_FALSE(*static_cast<bool*>(networkEngine->getReplicatedObjects()[0][0]));
  ASSERT_TRUE(*static_cast<bool*>(networkEngine->getReplicatedObjects()[1][0]));

  testObject0.reset();

  ASSERT_EQ(networkEngine->getReplicatedObjects()[0].size(), 0);
}

TEST_F(NetworkEngineTest, DestroyReplicatedObject) {
  auto testObject = std::make_unique<TestObject>(*networkEngine);

  ASSERT_EQ(networkEngine->getReplicatedObjects()[0].size(), 1);

  testObject.reset();

  ASSERT_EQ(networkEngine->getReplicatedObjects()[0].size(), 0);
}
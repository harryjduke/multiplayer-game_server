// This file is part of multiplayer-game_server <https://github.com/harryjduke/multiplayer-game_server>.
// Copyright (c) 2024 Harry Duke <harryjduke@gmail.com>
//
// This program is distributed under the terms of the GNU General Public License version 2.
// You should have received a copy of the GNU General Public License along with this program.
// If not, see <https://github.com/harryjduke/multiplayer-game_server/blob/main/LICENSE> or <https://www.gnu.org/licenses/>.

#include <gtest/gtest.h>
#include <msgpack.hpp>
#include <queue>

#include "NetworkEngine.h"
#include "Replicated.h"

class EmptyNetworkAdaptor final : public INetworkProtocol {
public:
    std::optional<Message> recieve() override { return std::nullopt; }

    void send(Message message) override {};
};

class NetworkSerializationTest : public testing::Test {
protected:
    void SetUp() override {
        networkEngine = std::make_unique<NetworkEngine>(std::make_unique<EmptyNetworkAdaptor>());
    }

    void TearDown() override {
        networkEngine.reset();
    }

    std::unique_ptr<NetworkEngine> networkEngine;
};

class TestObject final : public Replicated<TestObject> {
public:
    explicit TestObject(NetworkEngine &networkEngine)
        : Replicated(networkEngine) {
    }

    void setTestBool(const bool newTestBool) {
        testBool = newTestBool;
    }

    [[nodiscard]] bool getTestBool() const {
        return testBool;
    }

    static constexpr TypeId typeId{"TestObject"};
    MSGPACK_DEFINE(testBool);

private:
    bool testBool{};
};

class TestObjectInt final : public Replicated<TestObjectInt> {
public:
    explicit TestObjectInt(NetworkEngine &networkEngine)
        : Replicated(networkEngine) {
    }

    void setTestInt(const int newTestInt) {
        testInt = newTestInt;
    }

    [[nodiscard]] int getTestInt() const {
        return testInt;
    }

    static constexpr TypeId typeId{"TestObjectInt"};
    MSGPACK_DEFINE(testInt);

private:
    int testInt{};
};

TEST_F(NetworkSerializationTest, CreateReplicatedObect) {
    auto testObject = std::make_unique<TestObject>(*networkEngine);

    std::vector<IReplicatable*> replicatedTestObjects;
    ASSERT_NO_THROW(replicatedTestObjects = networkEngine->getReplicatedObjects().at(Replicated<TestObject>::typeId()););
    ASSERT_EQ(replicatedTestObjects.size(), 1);
    const auto testObjectCast = dynamic_cast<TestObject*>(replicatedTestObjects[0]);
    ASSERT_NE(testObjectCast, nullptr);
    ASSERT_FALSE(testObjectCast->getTestBool());
}

TEST_F(NetworkSerializationTest, CreateTwoReplicatedObjects) {
    auto testObject0 = std::make_unique<TestObject>(*networkEngine);
    auto testObject1 = std::make_unique<TestObjectInt>(*networkEngine);
    testObject1->setTestInt(1);

    auto replicatedObjects = networkEngine->getReplicatedObjects();

    std::vector<IReplicatable*> replicatedTestObjects;
    ASSERT_NO_THROW(replicatedTestObjects = replicatedObjects.at(Replicated<TestObject>::typeId()););
    const auto testObject0Cast = dynamic_cast<TestObject*>(replicatedTestObjects[0]);
    ASSERT_NE(testObject0Cast, nullptr);
    ASSERT_FALSE(testObject0Cast->getTestBool());

    std::vector<IReplicatable*> replicatedTestObjectInts;
    ASSERT_NO_THROW(replicatedTestObjectInts = replicatedObjects.at(Replicated<TestObjectInt>::typeId()););
    const auto testObject1Cast = dynamic_cast<TestObjectInt*>(replicatedTestObjectInts[0]);
    ASSERT_NE(testObject1Cast, nullptr);
    ASSERT_EQ(testObject1Cast->getTestInt(), 1);
}

TEST_F(NetworkSerializationTest, DestroyReplicatedObject) {
    auto testObject = std::make_unique<TestObject>(*networkEngine);

    ASSERT_EQ(networkEngine->getReplicatedObjects().size(), 1);

    testObject.reset();

    ASSERT_EQ(networkEngine->getReplicatedObjects().size(), 0);
}

TEST_F(NetworkSerializationTest, SerializeReplicatedObjects) {
    const auto testObject = std::make_unique<TestObject>(*networkEngine);
    testObject->setTestBool(true);

    std::vector<uint8_t> replicatedTestObjectsSerialized;
    ASSERT_NO_THROW(replicatedTestObjectsSerialized = networkEngine->getReplicatedObjectsSerialized(););

    const unsigned char expected[] = {
        0x81,                                                            // map of 1 element
        0xaa,                                                            // string of length 10
        0x54, 0x65, 0x73, 0x74, 0x4f, 0x62, 0x6a, 0x65, 0x63, 0x74,      // typeId: "TestObject"
        0x81,                                                            // map of 1 element
        0x01,                                                            // instanceId: 1
        0x91,                                                            // array of 1 element
        0xc3                                                             // testBool: "true"
    };

    ASSERT_EQ(replicatedTestObjectsSerialized.size(), sizeof(expected));

    ASSERT_EQ(
        std::memcmp(
            replicatedTestObjectsSerialized.data(),
            expected,
            sizeof(expected)
        ),
        0
    );
}

class MockNetworkAdaptor final : public INetworkProtocol {
public:
    std::optional<Message> recieve() override {
        if (messageQueue_.empty()) {
            return std::nullopt;
        }
        auto message = messageQueue_.front();
        messageQueue_.pop();
        return message;
    }

    void send(Message message) override {
        sentMessages.push_back(message);
    }

    void queueMessage(const Message& response) {
        messageQueue_.push(response);
    }

    std::vector<Message> sentMessages;
private:
    std::queue<Message> messageQueue_;
};

class NetworkRecieveTest : public testing::Test {
protected:
    void SetUp() override {
        auto newNetworkAdaptorMock = std::make_unique<MockNetworkAdaptor>();
        // ReSharper disable once CppDFALocalValueEscapesFunction
        networkAdaptorMock = newNetworkAdaptorMock.get();
        networkEngine = std::make_unique<NetworkEngine>(std::move(newNetworkAdaptorMock));
    }

    void TearDown() override {
        networkEngine.reset();
    }

    std::unique_ptr<NetworkEngine> networkEngine;
    MockNetworkAdaptor* networkAdaptorMock{};
};


TEST_F(NetworkRecieveTest, MessageFromNewClient) {
    networkAdaptorMock->queueMessage({0, {}});
    networkEngine->update();
    ASSERT_EQ(networkEngine->getPlayers().size(), 1);
    ASSERT_EQ(networkAdaptorMock->sentMessages.size(), 1);
    networkEngine->update();
    ASSERT_EQ(networkAdaptorMock->sentMessages.size(), 2);
}

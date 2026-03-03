/**
 * @file object_manager_tests.cpp
 * @brief Unit tests for ObjectManager.
 * @author
 * @date
 */

#include "model/object_manager.h"

#include <gtest/gtest.h>

TEST(ObjectManagerTest, CreateInst) {
    ObjectManager mgr;

    auto inst = mgr.createInst(1, "test_inst");
    ASSERT_NE(inst, nullptr);
    EXPECT_EQ(inst->getId(), 1);
    EXPECT_EQ(inst->getName(), "test_inst");
}

TEST(ObjectManagerTest, FindInst) {
    ObjectManager mgr;

    mgr.createInst(1, "test_inst");
    auto found = mgr.findInst(1);

    ASSERT_TRUE(found.has_value());
    EXPECT_EQ((*found)->getName(), "test_inst");
}

TEST(ObjectManagerTest, FindInstNotFound) {
    ObjectManager mgr;

    auto found = mgr.findInst(999);
    EXPECT_FALSE(found.has_value());
}

TEST(ObjectManagerTest, RemoveInst) {
    ObjectManager mgr;

    mgr.createInst(1, "test_inst");
    mgr.removeInst(1);

    auto found = mgr.findInst(1);
    EXPECT_FALSE(found.has_value());
}

TEST(ObjectManagerTest, CreatePin) {
    ObjectManager mgr;

    auto pin = mgr.createPin(100, "test_pin");
    ASSERT_NE(pin, nullptr);
    EXPECT_EQ(pin->getId(), 100);
    EXPECT_EQ(pin->getName(), "test_pin");
}

TEST(ObjectManagerTest, CreateNet) {
    ObjectManager mgr;

    auto net = mgr.createNet(1000, "test_net");
    ASSERT_NE(net, nullptr);
    EXPECT_EQ(net->getId(), 1000);
    EXPECT_EQ(net->getName(), "test_net");
}

TEST(ObjectManagerTest, GetPinsByInst) {
    ObjectManager mgr;

    mgr.createInst(1, "test_inst");
    mgr.createPin(100, "pin1");
    mgr.createPin(101, "pin2");

    // Set parent inst
    auto pin1 = mgr.findPin(100).value();
    auto pin2 = mgr.findPin(101).value();
    pin1->setParentInstId(1);
    pin2->setParentInstId(1);

    auto pins = mgr.getPinsByInst(1);
    EXPECT_EQ(pins.size(), 2);
}

TEST(ObjectManagerTest, Clear) {
    ObjectManager mgr;

    mgr.createInst(1, "test_inst");
    mgr.createPin(100, "test_pin");
    mgr.createNet(1000, "test_net");

    mgr.clear();

    EXPECT_EQ(mgr.getAllInsts().size(), 0);
    EXPECT_EQ(mgr.getAllPins().size(), 0);
    EXPECT_EQ(mgr.getAllNets().size(), 0);
}

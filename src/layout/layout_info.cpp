/**
 * @file layout_info.cpp
 * @brief Implementation of layout information classes.
 * @author
 * @date
 */

#include "layout/layout_info.h"

InstLayout::InstLayout(ObjectID instId) : LayoutInfo(instId) {}

PinLayout::PinLayout(ObjectID pinId) : LayoutInfo(pinId), side_(PinSide::LEFT), offset_(0.0) {}

NetLayout::NetLayout(ObjectID netId) : LayoutInfo(netId) {}

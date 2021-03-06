/*
 * Copyright 2016 Freeman Zhang <zhanggyb@gmail.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <skland/gui/abstract-button.hpp>

#include <skland/core/numeric.hpp>
#include <skland/gui/key-event.hpp>
#include <skland/gui/mouse-event.hpp>

namespace skland {

AbstractButton::AbstractButton()
    : AbstractWidget(),
      flags_(0x1) {

}

AbstractButton::~AbstractButton() {

}

void AbstractButton::OnMouseEnter(MouseEvent *event) {
  Bit::Set<uint32_t>(flags_, kFlagIndexHovered);
  if (IsSensitive()) {
    Update();
  }
  event->Accept();
}

void AbstractButton::OnMouseLeave(MouseEvent *event) {
  Bit::Clear<uint32_t>(flags_, kFlagIndexHovered | kFlagIndexClicked | kFlagIndexPressed);
  if (IsSensitive()) {
    Update();
  }
  event->Accept();
}

void AbstractButton::OnMouseButton(MouseEvent *event) {
  if (event->state() == kMouseButtonPressed) {
    Bit::Set<uint32_t>(flags_, kFlagIndexPressed);
    Update();
  } else {
    if (IsPressed()) {
      Bit::Set<uint32_t>(flags_, kFlagIndexClicked);
    }
    Bit::Clear<uint32_t>(flags_, kFlagIndexPressed);
    Update();
  }

  if (flags_ & kFlagIndexClicked) {
    Bit::Clear<uint32_t>(flags_, kFlagIndexClicked);
    event->Accept();
    clicked_();
    return;
  }
}

void AbstractButton::OnMouseMove(MouseEvent *event) {
  event->Accept();
}

void AbstractButton::OnKeyboardKey(KeyEvent *event) {
  event->Accept();
}

void AbstractButton::SetSensitive(bool sensitive) {
  if (sensitive) {
    Bit::Set<uint32_t>(flags_, kFlagIndexSensitive);
  } else {
    Bit::Clear<uint32_t>(flags_, kFlagIndexSensitive);
  }
}

}

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

#include <skland/gui/abstract-window-frame.hpp>
#include <skland/gui/abstract-window.hpp>
#include <skland/gui/push-button.hpp>

namespace skland {

const Margin AbstractWindowFrame::kResizingMargin(5, 5, 5, 5);

AbstractWindowFrame::AbstractWindowFrame()
    : Trackable(),
      window_(nullptr),
      border_(0),
      title_bar_size_(0),
      title_bar_position_(kTitleBarNone) {
}

AbstractWindowFrame::~AbstractWindowFrame() {
}

Rect AbstractWindowFrame::GetClientGeometry() const {
  int x = border_,
      y = border_,
      w = window()->width() - 2 * border_,
      h = window()->height() - 2 * border_;

  switch (title_bar_position_) {
    case kTitleBarLeft: {
      x += title_bar_size_ - border_;
      break;
    }
    case kTitleBarRight: {
      w -= title_bar_size_ + border_;
      break;
    }
    case kTitleBarBottom: {
      h -= title_bar_size_ + border_;
      break;
    }
    case kTitleBarTop:
    default: {
      y += title_bar_size_ - border_;
      h -= title_bar_size_ - border_;
      break;
    }
  }

  return Rect::FromXYWH(x, y, w, h);
}

void AbstractWindowFrame::AddWidget(AbstractWidget *widget, int pos) {
  window_->AddSubView(widget, pos);
}

}

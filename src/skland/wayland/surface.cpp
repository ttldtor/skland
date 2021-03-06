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

#include "skland/wayland/surface.hpp"

namespace skland {
namespace wayland {

const struct wl_surface_listener Surface::kListener = {
    OnEnter,
    OnLeave
};

void Surface::OnEnter(void *data, struct wl_surface *wl_surface, struct wl_output *wl_output) {
  Surface *_this = static_cast<Surface *>(data);
  if (_this->enter_) {
    _this->enter_.Invoke(wl_output);
  }
}

void Surface::OnLeave(void *data, struct wl_surface *wl_surface, struct wl_output *wl_output) {
  Surface *_this = static_cast<Surface *>(data);
  if (_this->leave_) {
    _this->leave_.Invoke(wl_output);
  }
}

}
}

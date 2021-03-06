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

#include "xdg-popup-meta.hpp"

#include <skland/wayland/xdg-popup.hpp>

namespace skland {
namespace wayland {

const struct zxdg_popup_v6_listener XdgPopupMeta::kListener = {
    OnConfigure,
    OnPopupDone
};

void XdgPopupMeta::OnConfigure(void *data,
                               struct zxdg_popup_v6 *zxdg_popup_v6,
                               int32_t x,
                               int32_t y,
                               int32_t width,
                               int32_t height) {
  XdgPopup *_this = static_cast<XdgPopup *>(data);
  if (_this->configure_)
    _this->configure_(x, y, width, height);
}

void XdgPopupMeta::OnPopupDone(void *data, struct zxdg_popup_v6 *zxdg_popup_v6) {
  XdgPopup *_this = static_cast<XdgPopup *>(data);
  if (_this->done_)
    _this->done_();
}

}
}

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

#include <skland/gui/egl-surface.hpp>
#include <skland/gui/display.hpp>
#include <skland/gui/abstract-view.hpp>

namespace skland {

EGLSurface::EGLSurface(AbstractView *view)
    : AbstractSurface(view) {
  egl_surface_.Setup(Display::egl_display(),
                     wl_surface(),
                     static_cast<int>(view->geometry().width()),
                     static_cast<int>(view->geometry().height()));
}

EGLSurface::~EGLSurface() {

}

std::shared_ptr<Canvas> EGLSurface::GetCanvas() const {
  // TODO: render to canvas
  return nullptr;
}

}
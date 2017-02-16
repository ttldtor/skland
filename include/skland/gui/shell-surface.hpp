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

#ifndef SKLAND_GUI_SHELL_SURFACE_HPP_
#define SKLAND_GUI_SHELL_SURFACE_HPP_

#include "surface-holder.hpp"
#include "../wayland/xdg-surface.hpp"

namespace skland {

/**
 * @ingroup gui
 * @brief Shell surface role
 */
class ShellSurface : public Trackable {

  ShellSurface() = delete;
  ShellSurface(const ShellSurface &) = delete;
  ShellSurface &operator=(const ShellSurface &) = delete;

 public:

  ShellSurface(AbstractView *view, const Margin &margin = Margin());

  virtual ~ShellSurface();

  void ResizeWindow(int width, int height) const {
    xdg_surface_.SetWindowGeometry(surface()->margin().l,
                                   surface()->margin().t,
                                   width, height);
  }

  void AckConfigure(uint32_t serial) const {
    xdg_surface_.AckConfigure(serial);
  }

  DelegateRef<void(uint32_t)> surface_configure() { return xdg_surface_.configure(); }

  Surface *surface() const { return surface_holder_.surface(); }

 protected:

  const SurfaceHolder &surface_holder() const { return surface_holder_; }

  const wayland::XdgSurface &xdg_surface() const { return xdg_surface_; }

  SignalRef<> destroying() { return destroying_; }

 private:

  void OnViewSurfaceDestroying(__SLOT__);

  SurfaceHolder surface_holder_;

  wayland::XdgSurface xdg_surface_;

  Signal<> destroying_;

};

}

#endif // SKLAND_GUI_SHELL_SURFACE_HPP_

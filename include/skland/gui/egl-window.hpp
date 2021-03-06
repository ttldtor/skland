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

#ifndef SKLAND_GUI_EGL_WINDOW_HPP_
#define SKLAND_GUI_EGL_WINDOW_HPP_

#include "abstract-window.hpp"

#include "../wayland/callback.hpp"

#include "memory-pool.hpp"
#include "buffer.hpp"

namespace skland {

class Surface;
class EGLSurface;
class Canvas;

/**
 * @ingroup gui
 * @brief A window contains an EGL surface for 3D scene
 *
 * @example simple-egl.cpp
 */
class EGLWindow : public AbstractWindow {

  EGLWindow() = delete;
  EGLWindow(const EGLWindow &) = delete;
  EGLWindow &operator=(const EGLWindow &) = delete;

 public:

  EGLWindow(const char *title, AbstractWindowFrame *frame = Theme::CreateWindowFrame());

  EGLWindow(int width, int height, const char *title,
            AbstractWindowFrame *frame = Theme::CreateWindowFrame());

  virtual ~EGLWindow();

 protected:

  virtual void OnShown() final;

  virtual void OnUpdate(AbstractView *view) final;

  virtual Surface *OnGetSurface(const AbstractView *view) const final;

  virtual void OnSizeChanged(int width, int height) final;

  virtual void OnDraw(const Context *context) final;

  virtual void OnInitializeEGL();

  virtual void OnResizeEGL(int width, int height);

  virtual void OnRenderEGL();

  bool MakeCurrent();

  void SwapBuffers();

 private:

  void OnFrame(uint32_t serial);

  void OnRelease();

  /* Properties for frame surface, JUST experimental */
  MemoryPool frame_pool_;
  Buffer frame_buffer_;
  std::shared_ptr<Canvas> frame_canvas_;

  /* Properties for main surface, JUST experimental */
  MemoryPool main_pool_;
  Buffer main_buffer_;
  std::shared_ptr<Canvas> main_canvas_;

  Surface *main_surface_;

  Surface *sub_surface_;

  EGLSurface *egl_surface_;

  wayland::Callback frame_callback_;

  bool animating_;

};

}

#endif // SKLAND_GUI_EGL_WINDOW_HPP_

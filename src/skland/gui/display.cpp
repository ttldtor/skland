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

#include <skland/core/exceptions.hpp>

#include <skland/gui/display.hpp>
#include <skland/gui/output.hpp>
#include <skland/gui/input.hpp>
#include <skland/gui/abstract-window.hpp>
#include <skland/gui/surface.hpp>

#include "internal/display-private.hpp"

#include <iostream>

using std::cout;
using std::cerr;
using std::endl;

namespace skland {

Display *Display::kDisplay = nullptr;

Display::Display()
    : Object(),
//      xkb_context_(nullptr),
      display_fd_(0),
      first_output_(nullptr),
      last_output_(nullptr),
      outputs_count_(0),
      first_input_(nullptr),
      last_input_(nullptr),
      inputs_count_(0) {
  data_.reset(new Private);
  cursors_.resize(kCursorBlank, nullptr);
  AbstractView::InitializeRedrawTaskList();
  Surface::InitializeCommitTaskList();
}

Display::~Display() {
  AbstractView::ClearRedrawTaskList();
  Surface::ClearCommitTaskList();
}

void Display::Connect(const char *name) {
  if (data_->wl_display.IsValid()) return;

  data_->wl_display.error().Set(this, &Display::OnError);
  data_->wl_display.delete_id().Set(this, &Display::OnDeleteId);
  data_->wl_display.Connect(name);

  if (!data_->wl_display.IsValid()) {
    throw std::runtime_error("FATAL! Cannot connect to Wayland compositor!");
  }

  data_->xkb_context = xkb_context_new(XKB_CONTEXT_NO_FLAGS);
  if (data_->xkb_context == NULL) {
    throw std::runtime_error("FATAL! Cannot create xkb_context!");
  }

  display_fd_ = data_->wl_display.GetFd();
  data_->egl_display.Setup(data_->wl_display);
  fprintf(stdout, "Use EGL version: %d.%d\n", data_->egl_display.major(), data_->egl_display.minor());

  data_->wl_registry.global().Set(this, &Display::OnGlobal);
  data_->wl_registry.global_remove().Set(this, &Display::OnGlobalRemove);
  data_->wl_registry.Setup(data_->wl_display);

  data_->wl_display.Dispatch();
  if (data_->wl_display.Roundtrip() < 0) {
    Disconnect();
    throw CompositorError("Failed to process Wayland connection!");
  }

  data_->wl_display.Roundtrip();
  /*
   * Why do we need two roundtrips here?
   *
   * wl_display_get_registry() sends a request to the server, to which
   * the server replies by emitting the wl_registry.global events.
   * The first wl_display_roundtrip() sends wl_display.sync. The server
   * first processes the wl_display.get_registry which includes sending
   * the global events, and then processes the sync. Therefore when the
   * sync (roundtrip) returns, we are guaranteed to have received and
   * processed all the global events.
   *
   * While we are inside the first wl_display_roundtrip(), incoming
   * events are dispatched, which causes registry_handle_global() to
   * be called for each global. One of these globals is wl_shm.
   * registry_handle_global() sends wl_registry.bind request for the
   * wl_shm global. However, wl_registry.bind request is sent after
   * the first wl_display.sync, so the reply to the sync comes before
   * the initial events of the wl_shm object.
   *
   * The initial events that get sent as a reply to binding to wl_shm
   * include wl_shm.format. These tell us which pixel formats are
   * supported, and we need them before we can create buffers. They
   * don't change at runtime, so we receive them as part of init.
   *
   * When the reply to the first sync comes, the server may or may not
   * have sent the initial wl_shm events. Therefore we need the second
   * wl_display_roundtrip() call here.
   *
   * The server processes the wl_registry.bind for wl_shm first, and
   * the second wl_display.sync next. During our second call to
   * wl_display_roundtrip() the initial wl_shm events are received and
   * processed. Finally, when the reply to the second wl_display.sync
   * arrives, it guarantees we have processed all wl_shm initial events.
   *
   * This sequence contains two examples on how wl_display_roundtrip()
   * can be used to guarantee, that all reply events to a request
   * have been received and processed. This is a general Wayland
   * technique.
   */

  // TODO: more operations
}

void Display::Disconnect() noexcept {
  if (!data_->wl_display.IsValid()) return;

  xkb_context_unref(data_->xkb_context);

  // TODO: other operations

  ClearManagedObject(this, &first_output_, &last_output_, outputs_count_);
  ClearManagedObject(this, &first_input_, &last_input_, inputs_count_);
//  ClearManagedObject(this, &first_window_, &last_window_, windows_count_);
  Surface::Clear();

  data_->wl_data_device_manager.Destroy();
  if (data_->wl_cursor_theme.IsValid()) {
    ReleaseCursors();
    data_->wl_cursor_theme.Destroy();
  }
  data_->wl_shell.Destroy();
  data_->xdg_shell.Destroy();
  data_->wl_shm.Destroy();
  data_->wl_subcompositor.Destroy();
  data_->wl_compositor.Destroy();
  data_->wl_registry.Destroy();

  data_->egl_display.Destroy();
  data_->wl_display.Disconnect();
}

const Output *Display::GetOutputAt(int index) {
  return kDisplay->GetManagedObjectAt(index,
                                      kDisplay->first_output_,
                                      kDisplay->last_output_,
                                      kDisplay->outputs_count_);
}

void Display::AddOutput(Output *output, int index) {
  InsertManagedObject(this,
                      output,
                      &output->display_,
                      &first_output_,
                      &last_output_,
                      outputs_count_,
                      index);
}

void Display::DestroyOutput(uint32_t id) {
  for (Output *output = first_output_; output; output = static_cast<Output *>(output->next())) {
    if (output->server_output_id_ == id) {
      delete output;
      break;
    }
  }
}

void Display::AddInput(Input *input, int index) {
  InsertManagedObject(this,
                      input,
                      &input->display_,
                      &first_input_,
                      &last_input_,
                      inputs_count_,
                      index);
}

void Display::OnError(void *object_id, uint32_t code, const char *message) {

  const char *object_name = "Unknown object";

  if (data_->wl_display.Equal(object_id)) {
    object_name = "wl_display";
  } else if (data_->wl_compositor.Equal(object_id)) {
    object_name = "wl_compositor";
  } else if (data_->wl_registry.Equal(object_id)) {
    object_name = "wl_registry";
  } else if (data_->wl_subcompositor.Equal(object_id)) {
    object_name = "wl_subcompositor";
  } else if (data_->wl_shm.Equal(object_id)) {
    object_name = "wl_shm";
  } else if (data_->wl_shell.Equal(object_id)) {
    object_name = "wl_shell";
  } else if (data_->xdg_shell.Equal(object_id)) {
    object_name = "xdg_shell";
  }

  DBG_PRINT_MSG("Error from %s: %s (%d)\n", object_name, message, code);
}

void Display::OnDeleteId(uint32_t id) {

}

void Display::OnGlobal(uint32_t id,
                       const char *interface,
                       uint32_t version) {
  using wayland::XdgShell;

  struct Global *global = new Global;
  global->id = id;
  global->interface = interface;
  global->version = version;
  globals_.push_back(global);

  if (strcmp(interface, wl_compositor_interface.name) == 0) {
    data_->wl_compositor.Setup(data_->wl_registry, id, version);
  } else if (strcmp(interface, wl_subcompositor_interface.name) == 0) {
    data_->wl_subcompositor.Setup(data_->wl_registry, id, version);
  } else if (strcmp(interface, wl_shm_interface.name) == 0) {
    data_->wl_shm.format().Set(this, &Display::OnFormat);
    data_->wl_shm.Setup(data_->wl_registry, id, version);
    data_->wl_cursor_theme.Load(NULL, 24, data_->wl_shm);
    InitializeCursors();
  } else if (strcmp(interface, wl_output_interface.name) == 0) {
    Output *output = new Output(data_->wl_registry, id, version);
    AddOutput(output);
  } else if (strcmp(interface, XdgShell::GetInterface()->name) == 0) {
    data_->xdg_shell.ping().Set(this, &Display::OnXdgShellPing);
    data_->xdg_shell.Setup(data_->wl_registry, id, version);
  } else if (strcmp(interface, wl_shell_interface.name) == 0) {
    data_->wl_shell.Setup(data_->wl_registry, id, version);
  } else if (strcmp(interface, wl_seat_interface.name) == 0) {
    Input *input = new Input(id, version);
    AddInput(input);
  } else if (strcmp(interface, wl_data_device_manager_interface.name) == 0) {
    data_->wl_data_device_manager.Setup(data_->wl_registry, id, version);
  }
}

void Display::OnGlobalRemove(uint32_t name) {
  for (std::list<Global *>::iterator it = globals_.begin(); it != globals_.end();) {
    if ((*it)->id != name) {
      it++;
      continue;
    }

    if ((*it)->interface == "wl_output") {
      DestroyOutput(name);
    }

    unregister_.Emit(**it);

    delete (*it);
    it = globals_.erase(it);
  }
}

void Display::OnFormat(uint32_t format) {
  const char *text = nullptr;

  pixel_formats_.insert(format);

  switch (format) {
    case WL_SHM_FORMAT_ARGB8888: {
      text = "ARGB8888";
      break;
    }
    case WL_SHM_FORMAT_XRGB8888: {
      text = "XRGB8888";
      break;
    }
    default: {
      text = "other format";
      break;
    }
  }

  fprintf(stderr, "Possible shmem format %s\n", text);
}

void Display::OnXdgShellPing(uint32_t serial) {
  data_->xdg_shell.Pong(serial);
}

void Display::InitializeCursors() {
  cursors_[kCursorBottomLeft] = Cursor::Create(data_->wl_cursor_theme.GetCursor("bottom_left_corner"));
  cursors_[kCursorBottomRight] = Cursor::Create(data_->wl_cursor_theme.GetCursor("bottom_right_corner"));
  cursors_[kCursorBottom] = Cursor::Create(data_->wl_cursor_theme.GetCursor("bottom_side"));
  cursors_[kCursorDragging] = Cursor::Create(data_->wl_cursor_theme.GetCursor("grabbing"));
  cursors_[kCursorLeftPtr] = Cursor::Create(data_->wl_cursor_theme.GetCursor("left_ptr"));
  cursors_[kCursorLeft] = Cursor::Create(data_->wl_cursor_theme.GetCursor("left_side"));
  cursors_[kCursorRight] = Cursor::Create(data_->wl_cursor_theme.GetCursor("right_side"));
  cursors_[kCursorTopLeft] = Cursor::Create(data_->wl_cursor_theme.GetCursor("top_left_corner"));
  cursors_[kCursorTopRight] = Cursor::Create(data_->wl_cursor_theme.GetCursor("top_right_corner"));
  cursors_[kCursorTop] = Cursor::Create(data_->wl_cursor_theme.GetCursor("top_side"));
  cursors_[kCursorIbeam] = Cursor::Create(data_->wl_cursor_theme.GetCursor("xterm"));
  cursors_[kCursorHand1] = Cursor::Create(data_->wl_cursor_theme.GetCursor("hand1"));
  cursors_[kCursorWatch] = Cursor::Create(data_->wl_cursor_theme.GetCursor("watch"));
  cursors_[kCursorDndMove] = Cursor::Create(data_->wl_cursor_theme.GetCursor("left_ptr"));
  cursors_[kCursorDndCopy] = Cursor::Create(data_->wl_cursor_theme.GetCursor("left_ptr"));
  cursors_[kCursorDndForbidden] = Cursor::Create(data_->wl_cursor_theme.GetCursor("left_ptr"));
}

void Display::ReleaseCursors() {
  for (size_t i = 0; i < cursors_.size(); i++) {
    delete cursors_[i];
    cursors_[i] = nullptr;
  }
}

}

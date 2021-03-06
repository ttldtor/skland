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

#ifndef SKLAND_GUI_ABSTRACT_VIEW_HPP_
#define SKLAND_GUI_ABSTRACT_VIEW_HPP_

#include "../core/defines.hpp"
#include "../core/sigcxx.hpp"
#include "../core/size.hpp"
#include "../core/rect.hpp"

#include "task.hpp"

#include <memory>

namespace skland {

// Forward declarations
class Application;
class Display;
class Input;
class KeyEvent;
class MouseEvent;
class TouchEvent;
class Surface;
class Context;

/**
 * @ingroup gui
 * @brief An abstract base class for view object
 *
 * AbstractView is an abstract base class for view object. A view object in
 * SkLand is a rectangle area on screen and handle events. It can be a window,
 * popup menu, or widgets in a window. You typically don't use this class
 * directly. Instead, you use or create a subclass.
 *
 * A view can have parent and subviews, when you create a GUI
 * application, it generates a view hierachy.
 *
 * A view object can have arbitrary number of surfaces. A view which
 * does not have a parent must contains a shell surface. A view
 * without parent usually is a window or a popup menu.
 *
 * If a view does not have its own wayland surface (most widgets),
 * then it shares the surface with others which is managed in one of
 * parent views.
 *
 * @see AbstractWindow
 * @see Surface
 */
SKLAND_EXPORT class AbstractView : public Trackable {

  friend class Application;
  friend class Display;
  friend class Input;

  friend struct RedrawTask;
  friend class RedrawTaskProxy;
  friend class MouseTaskProxy;

  AbstractView(const AbstractView &) = delete;
  AbstractView &operator=(const AbstractView &) = delete;

 public:

  class Iterator;
  class ConstIterator;

  /**
   * @brief Default constructor
   *
   * This create a 400 x 300 view.
   */
  AbstractView();

  /**
   * @brief Create a view by given size
   */
  AbstractView(int width, int height);

  /**
   * @brief Destructor
   */
  virtual ~AbstractView();

  void MoveTo(int x, int y);

  void Resize(int width, int height);

  int x() const { return static_cast<int>(geometry_.x()); }

  int y() const { return static_cast<int>(geometry_.y()); }

  int width() const { return static_cast<int>(geometry_.width()); }

  int height() const { return static_cast<int>(geometry_.height()); }

  float center_x() const { return geometry_.center_x(); }

  float center_y() const { return geometry_.center_y(); }

  const Rect &geometry() const { return geometry_; }

  bool visible() const { return visible_; }

  /**
   * @brief Update the display of this widget
   */
  void Update();

  virtual bool Contain(int x, int y) const;

  virtual bool IsExpandX() const;

  virtual bool IsExpandY() const;

  virtual Size GetMinimalSize() const = 0;

  virtual Size GetPreferredSize() const = 0;

  virtual Size GetMaximalSize() const = 0;

 protected:

  AbstractView *GetChildAt(int index) const;

  /**
   * @brief Push a child object to the front
   * @param child
   *
   * @warning This method does not check if child is nullptr
   */
  void PushFrontChild(AbstractView *child);

  /**
   * @brief Insert a child object at the given index
   * @param child
   * @param index The position to be inserted before, default is 0, same as push front
   *
   * @warning This method does not check if child is nullptr
   */
  void InsertChild(AbstractView *child, int index = 0);

  /**
   * @brief Push a child object to the back
   * @param child
   *
   * @warning This method does not check if child is nullptr
   */
  void PushBackChild(AbstractView *child);

  /**
   * @brief Remove a child object from the children list
   * @param child
   * @return nullptr if the object is not a child, or the removed object
   *
   * @warning This method does not check if the param is nullptr
   */
  AbstractView *RemoveChild(AbstractView *child);

  void ClearChildren();

  virtual void OnAddedToParent();

  virtual void OnRemovedFromParent(AbstractView *original_parent);

  static bool SwapIndex(AbstractView *object1, AbstractView *object2);

  static bool InsertSiblingBefore(AbstractView *src, AbstractView *dst);

  static bool InsertSiblingAfter(AbstractView *src, AbstractView *dst);

  static void MoveToFirst(AbstractView *object);

  static void MoveToLast(AbstractView *object);

  static void MoveForward(AbstractView *object);

  static void MoveBackward(AbstractView *object);

  /**
   * @brief Update this view and all sub views
   */
  void UpdateAll();

  /**
   * @brief A view request an update
   * @param view This view or a sub view in hierachy
   */
  virtual void OnUpdate(AbstractView *view);

  /**
   * @brief Get surface for the given view
   * @param view A view object, it is always this view or a sub view in hierachy
   * @return A pointer to a surface or nullptr
   */
  virtual Surface *OnGetSurface(const AbstractView *view) const;

  virtual void OnSizeChanged(int width, int height) = 0;

  virtual void OnMouseEnter(MouseEvent *event) = 0;

  virtual void OnMouseLeave(MouseEvent *event) = 0;

  virtual void OnMouseMove(MouseEvent *event) = 0;

  virtual void OnMouseButton(MouseEvent *event) = 0;

  virtual void OnKeyboardKey(KeyEvent *event) = 0;

  virtual void OnDraw(const Context *context) = 0;

  void TrackMouseMotion(MouseEvent *event);

  void UntrackMouseMotion();

  void set_visible(bool visible) {
    visible_ = visible;
  }

  void set_position(int x, int y) {
    geometry_.MoveTo(x, y);
  }

  void resize(int width, int height) {
    geometry_.Resize(width, height);
  }

  /**
   * @brief Get the surface on which this view renders
   * @return A surface object or nullptr
   */
  static Surface *GetSurface(const AbstractView *view);

  static void Damage(AbstractView *view, int surface_x, int surface_y, int width, int height);

 private:

  struct Private;

  bool visible_;

  Rect geometry_;

  std::unique_ptr<Private> p_;

  /**
   * @brief Initialize the idle task list
   */
  static void InitializeRedrawTaskList();

  /**
   * @brief Destroy the redraw task list
   */
  static void ClearRedrawTaskList();

  static Task kRedrawTaskHead;
  static Task kRedrawTaskTail;

};

}

#endif // SKLAND_GUI_ABSTRACT_VIEW_HPP_

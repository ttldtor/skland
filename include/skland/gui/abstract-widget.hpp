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

#ifndef SKLAND_GUI_ABSTRACT_WIDGET_HPP_
#define SKLAND_GUI_ABSTRACT_WIDGET_HPP_

#include "abstract-view.hpp"

namespace skland {

class AbstractWindow;

/**
 * @brief The base class for widgets
 *
 * Widgets are the main GUI elements in SkLand to display something in a window.
 *
 * A widget in skland does not contain a root surface (the surface which have no
 * parent surface), it always use Skia to draw itself on a raster surface
 * provided by a window.
 */
class AbstractWidget : public AbstractView {

  friend class AbstractWindow;

 public:

  AbstractWidget();

  AbstractWidget(int width, int height);

  virtual ~AbstractWidget();

  virtual Size GetMinimalSize() const override;

  virtual Size GetPreferredSize() const override;

  virtual Size GetMaximalSize() const override;

};

class Widget : public AbstractWidget {

 public:

  using AbstractWidget::AbstractWidget;

  virtual ~Widget();

 protected:

  virtual void OnSizeChanged(int width, int height) override;

  virtual void OnMouseEnter(MouseEvent *event) override;

  virtual void OnMouseLeave(MouseEvent *event) override;

  virtual void OnMouseMove(MouseEvent *event) override;

  virtual void OnMouseButton(MouseEvent *event) override;

  virtual void OnKeyboardKey(KeyEvent *event) override;

  virtual void OnDraw(const Context *context) override;

};

}

#endif // SKLAND_GUI_ABSTRACT_WIDGET_HPP_

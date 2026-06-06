//
// Created by code on 5/2/26.
//

#ifndef TEST_IOS_APP_IGRIDLAYOUT_H
#define TEST_IOS_APP_IGRIDLAYOUT_H
#include <kromakit/layout/ILayout.h>


class IGridLayout : public ILayout {
public:
  IGridLayout() = default;
  ~IGridLayout() override = default;

  void SetPadding(const DUIInsets& value);

  void SetRows(const std::vector<LayoutLength>& value);
  void SetColumns(const std::vector<LayoutLength>& value);

  void SetRowSpacing(float value);
  void SetColumnSpacing(float value);
  void SetSpacing(float value);

  std::vector<LayoutLength>& GetRows();
  std::vector<LayoutLength>& GetColumns();

  const std::vector<LayoutLength>& GetRows() const;
  const std::vector<LayoutLength>& GetColumns() const;

  DUISize Measure(
      const std::vector<LayoutItem>& children,
      const DUISize& available
  ) override;

  void Arrange(
      std::vector<LayoutItem>& children,
      const DUIRect& rect
  ) override;

protected:
  DUIInsets layoutPadding{};
  std::vector<LayoutLength> rows;
  std::vector<LayoutLength> columns;

  float rowSpacing = 0, columnSpacing = 0;
};


#endif //TEST_IOS_APP_IGRIDLAYOUT_H

/**
 * @file cell_renderer.cpp
 * @author Jerome Wei (jeromejwei@gmail.com)
 * @brief Implementation for cell renderer
 * @version 0.1
 * @date 2022-04-11
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "widgets/main_window.hpp"

using namespace crossword_backend;

/**
 * @brief Custom grid cell drawing method.
 *
 * Ideally should be called when the grid is refreshed.
 * It may be the case that this is called more often than that.
 * 
 * TODO: optimize so we aren't doing too much work here; no more heap allocations; etc.
 *
 * @param grid
 * @param attr
 * @param dc
 * @param rect
 * @param row
 * @param col
 * @param isSelected
 */
void CellRenderer::Draw(wxGrid &grid, wxGridCellAttr &attr, wxDC &dc, const wxRect &rect, int row, int col,
                        bool isSelected) {
  Coord cell_coord = Coord(row, col);
  Crossword *crossword = &app->crossword;
  Cell cell = crossword->Get(cell_coord);

  if (cell.IsBarrier()) {
    grid.SetCellValue(
            wxGridCellCoords(row, col),
            "");
  } else {
    grid.SetCellValue(
            wxGridCellCoords(row, col),
            cell.ToString());
  }

  wxColour bg_save = attr.GetBackgroundColour();
  if (!isSelected) {
    if (cell.IsBarrier()) {
      attr.SetBackgroundColour(kBLACK);
    } else {
      attr.SetBackgroundColour(COLOR_MAP[app->grid_colors[row][col]]);
    }
  }

  base_cell_renderer.Draw(grid, attr, dc, rect, row, col, isSelected);

  int cell_number = crossword->GetClueNumber(cell_coord);
  wxRect abs_rect = grid.BlockToDeviceRect(
          wxGridCellCoords(row, col),
          wxGridCellCoords(row, col));
  if (cell_number != kNO_NUMBER) {
    dc.SetFont(cell_number_font);
    dc.DrawText(std::to_string(cell_number), abs_rect.GetLeft() + (cell_number_font_size * .2), abs_rect.GetTop());
  }

  // Draw in locked symbol
  // TODO: this
  if (cell.IsLocked()) {
    int num_font_size = std::max(1.0, ((double) app->CellSize()) * .3);
    dc.SetFont(
            wxFont(
                    wxSize(0, num_font_size),
                    wxFONTFAMILY_SWISS,
                    wxFONTSTYLE_NORMAL,
                    wxFONTWEIGHT_NORMAL,
                    false,
                    "",
                    wxFONTENCODING_SYSTEM));
    dc.DrawText("L", abs_rect.GetRight() - (num_font_size * .6), abs_rect.GetTop());
  }

  // restore old attributes
  attr.SetBackgroundColour(bg_save);
}

wxSize CellRenderer::GetBestSize(wxGrid &grid, wxGridCellAttr &attr, wxDC &dc, int row, int col) {
  return base_cell_renderer.GetBestSize(grid, attr, dc, row, col);
}

/**
 * @brief Make a copy of cell renderer
 *
 * @return wxGridCellRenderer*
 */
wxGridCellRenderer *CellRenderer::Clone() const {
  return new CellRenderer(app, cell_number_font, cell_number_font_size);
}

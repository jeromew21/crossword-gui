/**
 * @file grid.cpp
 * @author Jerome Wei (jeromejwei@gmail.com)
 * @brief Methods related to the grid widget.
 * @version 0.1
 * @date 2022-04-09
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "widgets/main_window.hpp"
#include "widgets/dialog.hpp"

using namespace crossword_backend;

/**
 * @brief Initialize a new grid.
 *
 * Called upon initialization and whenever user resizes main window.
 *
 */
void CrosswordApp::InitGrid() {
  int client_width;
  int client_height;
  GetClientSize(&client_width, &client_height);

  int square_size = CellSize();

  int w = square_size * crossword.GetWidth();
  int h = square_size * crossword.GetHeight();

  w = std::max(w, 0);
  h = std::max(h, 1);

  if (client_width / 2 - w / 2 <= 0) {
    grid = new wxGrid(this, -1, wxPoint(0, 0), wxSize(w, h));
  } else {
    grid = new wxGrid(this, -1, wxPoint(client_width / 2 - w / 2, 0), wxSize(w, h));
  }
  grid->DisableCellEditControl();
  grid->EnableEditing(false);
  grid->HideColLabels();
  grid->HideRowLabels();
  grid->EnableDragCell(false);
  grid->EnableDragGridSize(false);
  grid->SetDefaultCellBackgroundColour(kWHITE);
  grid->SetDefaultCellTextColour(kBLACK);
  grid->SetDefaultCellAlignment(wxALIGN_CENTRE, wxALIGN_CENTRE);
  grid->SetGridLineColour(kBLACK);
  int num_font_size = std::max(1.0, square_size * .3);
  grid->SetDefaultRenderer(new CellRenderer(this,
                                            wxFont(wxSize(0, num_font_size), wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL,
                                                   wxFONTWEIGHT_NORMAL, false, "", wxFONTENCODING_SYSTEM),
                                            num_font_size));
  grid->CreateGrid(crossword.GetHeight(), crossword.GetWidth());

  // Bind dynamic events
  grid->GetGridWindow()->Bind(wxEVT_MOTION, &CrosswordApp::OnGridDrag, this);

  if (square_size > 0) {
    grid->SetDefaultColSize(square_size);
    grid->SetDefaultRowSize(square_size);

    int font_size = std::max(1.0, (double) square_size * .9);
    grid->SetDefaultCellFont(wxFont(
            wxSize(0, font_size),
            wxFONTFAMILY_SWISS,
            wxFONTSTYLE_NORMAL,
            wxFONTWEIGHT_NORMAL,
            false,
            "",
            wxFONTENCODING_SYSTEM));
  }
  grid->SetFocus();
  if (user_selection && crossword.InBounds(selected) && GetCurrentClue().GetSize() > 0) {
    SetGridCursor(selected);
    Clue clue = GetCurrentClue();
    SelectWordBlock(
            Coord(
                    clue.coord_list_.front().row,
                    clue.coord_list_.front().col),
            Coord(
                    clue.coord_list_.back().row,
                    clue.coord_list_.back().col));
  }
}

/**
 * @brief Set new grid dimensions.
 *
 * @param height
 * @param width
 */
void CrosswordApp::ResizeGrid(int height, int width) {
  crossword.SetDimensions(height, width);
  UpdateGrid();
}

/**
 * @brief Get the on-screen size of each square cell.
 *
 * @return int
 */
int CrosswordApp::CellSize() {
  int client_width;
  int client_height;
  GetClientSize(&client_width, &client_height);

  int square_size = std::min(client_width / crossword.GetWidth(),
                             client_height / crossword.GetHeight());

  return square_size;
}

/**
 * @brief Refresh state of grid to reflect underlying crossword state.
 *
 */
void CrosswordApp::UpdateGrid() {
  // Calls custom cell renderer draw method
  grid->ForceRefresh();

  if (crossword.GetWidth() != static_cast<std::size_t>(grid->GetNumberCols()) ||
      crossword.GetWidth() != static_cast<std::size_t>(grid->GetNumberRows())) {
    delete grid;
    InitGrid();
    SelectFirstClue();
  }

  ResetGridColors();

  // Grey out invalid states
  auto all_clues = crossword.Clues();
  if (!crossword.IsValidPattern() || (crossword.IsInvalidPartial(all_clues, db, 1) == Solvability::Invalid)) {
    // TODO: figure out why invalid
    for (std::size_t r = 0; r < crossword.GetHeight(); r++) {
      for (std::size_t c = 0; c < crossword.GetWidth(); c++) {
        grid_colors[r][c] = 2;
      }
    }
  }

  // Red for invalid clues
  if (GetSpellcheck()) // spellcheck
  {
    std::vector<Clue> clues = crossword.Clues();
    for (auto it = std::begin(clues); it != std::end(clues); ++it) {
      if (it->IsValid(db))
        continue;
      for (auto c = std::begin(it->coord_list_); c != std::end(it->coord_list_); ++c) {
        // 1 is red
        grid_colors[c->row][c->col] = 1;
      }
    }
  }

  // Calls custom cell renderer draw method
  grid->ForceRefresh();
}

/**
 * @brief Set selected cell to coordinate.
 *
 * @param c
 */
void CrosswordApp::SetGridCursor(Coord c) {
  selected.row = c.row;
  selected.col = c.col;
  grid->SetGridCursor(c.row, c.col);
}

/**
 * @brief Get the selected grid coordinate.
 *
 * @return Coord
 */
Coord CrosswordApp::GetGridCursor() {
  assert(selected == Coord(grid->GetGridCursorRow(), grid->GetGridCursorCol()));
  return selected;
}

/**
 * @brief Clear the on-screen grid selection.
 *
 * TODO: hide grid cursor as well
 *
 */
void CrosswordApp::ClearGridSelection() {
  user_selection = false;
  grid->ClearSelection();
}

/**
 * @brief Selects a rectangular region of the grid UI element.
 *
 * @param c0 upper-left coordinate
 * @param c1 lower-right coordinate
 */
void CrosswordApp::SelectWordBlock(Coord c0, Coord c1) {
  user_selection = true;
  grid->SelectBlock(c0.row, c0.col, c1.row, c1.col);
}

/**
 * @file main_window.cpp
 * @author Jerome Wei (jeromejwei@gmail.com)
 * @brief Main window implementation
 * @version 0.1
 * @date 2022-04-11
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "widgets/main_window.hpp"
#include "widgets/dialog.hpp"

#include "widgets/icon.xpm"

#include <thread>

using namespace crossword_backend;

/**
 * @brief Show an error message to the user.
 *
 * @param message
 */
void CrosswordApp::ErrorDialog(std::string const &message) {
  wxMessageBox(message,
               "Error", wxOK | wxICON_ERROR);
}

/**
 * @brief Load database from a CSV
 *
 * Adds to the current database.
 *
 * @param filename
 */
void CrosswordApp::LoadDatabaseFromCSV(const std::string &filename) {
  crossword.logger.Log("Loading database from file \"" + filename + "\"...");
  db.LoadDeferred(filename);
  std::thread db_load_callback([&] {
    if (!db.IsFinishedLoading()) {
      db.WaitForLock();
    }
    crossword.logger.Log("Done loading database.");
    UpdateGrid();
  });
  db_load_callback.detach();
}

void CrosswordApp::SaveToFile(const std::string &filename) {
  std::ofstream f;
  f.open(filename);
  auto lines = crossword.Serialize();
  for (std::string const &line: lines) {
    f << line << "\n";
  }
  crossword.logger.Log("Wrote out to file \"" + filename + "\"");
  open_file = filename;
}

/**
 * @brief App main window constructor
 *
 * Program starting state.
 *
 */
CrosswordApp::CrosswordApp(MainWindowOptions const &options)
        : wxFrame(
        nullptr,
        wxID_ANY,
        "Crossword Editor",
        wxPoint(-1, -1),
        wxSize(600, 800)),
          crossword{}, selected{0, 0},
          current_clue{kACROSS, Coord{0, 0}, 0, std::vector<Atom>{}, std::vector<Coord>{}},
          user_selection(false), is_searching(false) {
  if (options.silent) {
    crossword.logger.Silence();
  }

  wxIcon icon(icon_xpm);
  SetIcon(icon);
  CentreOnScreen();

  if (options.db) {
    LoadDatabaseFromCSV(options.db_filename);
  }

  SetMinSize(wxSize(200, 200));
  InitMenuBar();
  InitStatusBar();
  InitGrid();
  SelectFirstClue();
  UpdateGrid();
  InitBindings();
}

/**
 * @brief Set the default background color of all grid cells to white.
 *
 */
void CrosswordApp::ResetGridColors() {
  for (std::size_t i = 0; i < kMAX_DIM; i++) {
    for (std::size_t k = 0; k < kMAX_DIM; k++) {
      grid_colors[i][k] = 0;
    }
  }
}

/**
 * @brief Initializes status bar.
 *
 */
void CrosswordApp::InitStatusBar() {
  CreateStatusBar();
  SetStatusText("Ready");
}

/**
 * @brief Gets whether the rotational symmetry checkbox is checked.
 *
 */
bool CrosswordApp::GetRotationalSymmetry() {
  return menuEdit->IsChecked(ID_Enforce_Symmetry);
}

/**
 * @brief Get whether the current puzzle is enforcing spellcheck.
 *
 */
bool CrosswordApp::GetSpellcheck() {
  return menuEdit->IsChecked(ID_Spellcheck);
}

/**
 * @brief The act of clicking on a cell.
 *
 * Changes the current_clue
 * to the ACROSS or DOWN clue corresponding to the cell.
 * Rotates between the two.
 *
 * @param click_coords the coordinates of the clicked cell.
 */
void CrosswordApp::ClickCell(Coord click_coords) {
  SetGridCursor(click_coords);
  Clue saved_clue = current_clue;
  std::vector<Clue> clues = crossword.CluesStartingAt(click_coords);
  if (clues.empty())
    return;
  Clue clue = clues[0];
  if (clues.size() == 2 && clue.SameCoords(saved_clue)) {
    clue = clues[1];
  }
  current_clue = clue;
  SelectWordBlock(
          Coord(
                  clue.coord_list_.front().row,
                  clue.coord_list_.front().col),
          Coord(
                  clue.coord_list_.back().row,
                  clue.coord_list_.back().col));
}

/**
 * @brief Selects the upper-left-most clue.
 *
 * Called upon initialization and when the barrier pattern is changed.
 * No guarantees on whether this is ACROSS or DOWN.
 *
 */
void CrosswordApp::SelectFirstClue() {
  std::vector<Clue> clues = crossword.Clues();
  assert(!clues.empty()); // should fail when we have too restrictive a pattern
  ClickCell(clues[0].GetStart());
}

/**
 * @brief Returns a reference to the user-selected clue.
 *
 * Side effects: Changes the current_clue member to be in sync
 * with any changes to the crossword state.
 *
 * @return Clue&
 */
Clue &CrosswordApp::GetCurrentClue() {
  if (crossword.InBounds(current_clue.GetStart())) {
    std::vector<Clue> clues = crossword.CluesStartingAt(current_clue.GetStart());
    assert(!clues.empty());

    for (auto const &cl : clues) {
      if (current_clue.SameCoords(cl)) {
        current_clue = cl;
        break;
      }
    }
  }
  return current_clue;
}

/**
 * @brief Deletes the value of the currently selected cell.
 *
 */
void CrosswordApp::DeleteOne() {
  crossword.Set(Atom(), selected);
  Clue clue = GetCurrentClue();
  int i = clue.IndexOfCoord(selected);
  if (i > 0) {
    SetGridCursor(clue.coord_list_[i - 1]);
  }
  UpdateGrid();
}

/**
 * @brief Sets the value of the currently selected cell to a
 * particular character.
 *
 * @param c
 */
void CrosswordApp::EnterCharacter(char c) {
  std::string s(1, c);
  Atom new_value = Atom(s);
  crossword.Set(new_value, selected);
  UpdateGrid(); // call here to make the UI snappier
  Clue clue = GetCurrentClue();
  int i = clue.IndexOfCoord(selected);
  if (i != kNO_NUMBER && (std::size_t) i < clue.GetSize() - 1) {
    SetGridCursor(clue.coord_list_[i + 1]);
  }
  UpdateGrid();
}

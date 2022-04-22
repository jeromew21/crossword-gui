/**
 * @file event_handlers.cpp
 * @author Jerome Wei
 * @brief Implementation for bindings and event handlers.
 * @version 0.1
 * @date 2022-04-09
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "widgets/main_window.hpp"
#include "widgets/dialog.hpp"

#include <thread>
#include <chrono>
#include <atomic>
#include <fstream>
#include <sstream>

#include <wx/aboutdlg.h>
#include <wx/filedlg.h>

using namespace crossword_backend;

/**
 * @brief Initializes menu bar items.
 *
 */
void CrosswordApp::InitMenuBar() {
  wxMenu *menuFile = new wxMenu;
  menuFile->Append(
          ID_Export,
          "&Export...", "Export to ???");
  menuFile->AppendSeparator();
  menuFile->Append(
          ID_Open,
          "&Open...", "Open a file");
  menuFile->Append(
          ID_Save,
          "&Save", "Save changes");
  menuFile->Append(
          ID_Save_As,
          "&Save As...", "Save to a file");
  menuFile->AppendSeparator();
  menuFile->Append(
          ID_Load_Database,
          "&Load Database...", "Add entries from CSV database");
  menuFile->AppendSeparator();
  menuFile->Append(wxID_EXIT);

  menuEdit = new wxMenu;
  menuEdit->Append(
          ID_Undo,
          "&Undo...\tCtrl-Z", "Undo previous operation");
  menuEdit->Append(
          ID_Redo,
          "&Redo...\tCtrl-Y", "Redo operation");
  menuEdit->AppendSeparator();
  menuEdit->Append(
          ID_Resize_Grid,
          "&Set Grid Dimension...", "Set the crossword width and height");
  menuEdit->AppendSeparator();
  menuEdit->Append(
          ID_Reset,
          "&Clear All", "Clear all letters");
  menuEdit->AppendSeparator();
  menuEdit->Append(
          ID_Autofill,
          "&Attempt Autofill...", "Search for a solution");
  menuEdit->Append(
          ID_Stop_Autofill,
          "&Stop Autofill", "Stop the current autofill");
  menuEdit->AppendSeparator();
  menuEdit->AppendCheckItem(
          ID_Enforce_Symmetry,
          "&Enforce Rotational Symmetry",
          "Option to make the puzzle rotationally symmetric");
  menuEdit->Check(ID_Enforce_Symmetry, true);
  menuEdit->AppendCheckItem(
          ID_Spellcheck,
          "&Enforce Spell Check");
  menuEdit->Check(ID_Spellcheck, true);

  wxMenu *menuHelp = new wxMenu;
  menuHelp->Append(wxID_ABOUT);

  wxMenu *menuView = new wxMenu;
  menuView->Append(
          ID_Show_Puzzle_Info,
          "&Puzzle Info...", "Show info for current puzzle");
  menuView->Append(
          ID_Show_Logs,
          "&Show Logs...", "Show program logs");

  wxMenu *menuWord = new wxMenu;
  menuWord->Append(
          ID_Fill_Word,
          "&Pick Word...", "Select a word from list");
  menuWord->Append(
          ID_Clear_Word,
          "&Delete Word", "Delete the currently selected letters");
  menuWord->Append(
          ID_Set_Clue,
          "&Set Clue...", "Set a clue for this word");
  menuWord->Append(
          ID_Add_To_Dictionary,
          "&Add To Dictionary", "Add this word to the dictionary");
  menuWord->Append(
          ID_Word_Info,
          "&Word Info...", "Get info about selected word");

  wxMenu *menuCell = new wxMenu;
  menuCell->Append(
          ID_Lock_Cell,
          "&Lock/Unlock Cell",
          "Toggle locking for a single cell");

  menuBar = new wxMenuBar;
  menuBar->Append(menuFile, "&File");
  menuBar->Append(menuEdit, "&Edit");
  menuBar->Append(menuCell, "&Cell");
  menuBar->Append(menuWord, "&Word");
  menuBar->Append(menuView, "&View");
  menuBar->Append(menuHelp, "&Help");

  SetMenuBar(menuBar);
}

/**
 * @brief Bind static event handlers
 *
 */
void CrosswordApp::InitBindings() {
  Bind(wxEVT_MENU, &CrosswordApp::OnFillWord, this, ID_Fill_Word);
  Bind(wxEVT_MENU, &CrosswordApp::OnClearWord, this, ID_Clear_Word);
  Bind(wxEVT_MENU, &CrosswordApp::OnAbout, this, wxID_ABOUT);
  Bind(wxEVT_MENU, &CrosswordApp::OnExit, this, wxID_EXIT);
  Bind(wxEVT_MENU, &CrosswordApp::OnResizeGrid, this, ID_Resize_Grid);
  Bind(wxEVT_MENU, &CrosswordApp::OnReset, this, ID_Reset);
  Bind(wxEVT_MENU, &CrosswordApp::OnSetClue, this, ID_Set_Clue);
  Bind(wxEVT_MENU, &CrosswordApp::OnAutofill, this, ID_Autofill);
  Bind(wxEVT_MENU, &CrosswordApp::OnAddToDictionary, this, ID_Add_To_Dictionary);
  Bind(wxEVT_MENU, &CrosswordApp::OnUndo, this, ID_Undo);
  Bind(wxEVT_MENU, &CrosswordApp::OnRedo, this, ID_Redo);
  Bind(wxEVT_MENU, &CrosswordApp::OnShowLogs, this, ID_Show_Logs);
  Bind(wxEVT_MENU, &CrosswordApp::OnWordInfo, this, ID_Word_Info);
  Bind(wxEVT_MENU, &CrosswordApp::OnLockCell, this, ID_Lock_Cell);
  Bind(wxEVT_MENU, &CrosswordApp::OnExport, this, ID_Export);
  Bind(wxEVT_MENU, &CrosswordApp::OnOpen, this, ID_Open);
  Bind(wxEVT_MENU, &CrosswordApp::OnSave, this, ID_Save);
  Bind(wxEVT_MENU, &CrosswordApp::OnSaveAs, this, ID_Save_As);
  Bind(wxEVT_MENU, &CrosswordApp::OnStopAutofill, this, ID_Stop_Autofill);
  Bind(wxEVT_MENU, &CrosswordApp::OnLoadDatabase, this, ID_Load_Database);

  /* Custom events */
  Bind(GRID_REFRESH, &CrosswordApp::OnGridRefresh, this);
  Bind(SEARCHING, &CrosswordApp::OnSearching, this);
  Bind(DONE_SEARCHING, &CrosswordApp::OnDoneSearching, this);

  /* Grid related */
  Bind(wxEVT_GRID_CELL_LEFT_CLICK, &CrosswordApp::OnGridCellLeftClick, this);
  Bind(wxEVT_GRID_CELL_RIGHT_CLICK, &CrosswordApp::OnGridCellRightClick, this);
  // Bind(wxEVT_GRID_RANGE_SELECT, &CrosswordApp::OnGridRangeSelect, this);
  Bind(wxEVT_KEY_DOWN, &CrosswordApp::OnKeyDown, this);
  Bind(wxEVT_SIZE, &CrosswordApp::OnResize, this);
}

/**
 * @brief Handler for keyboard events.
 *
 * TODO: fix keyboard shortcut handling.
 *
 * @param event
 */
void CrosswordApp::OnKeyDown(wxKeyEvent &event) {
  if (is_searching)
    return;

  // TODO: preserve default event for shortcuts
  // event.Skip();

  char code = event.GetUnicodeKey();
  if (code == 27) // ESC
  {
    ClearGridSelection();
  } else if (code == 13) // ENTER
  {
    // select next
    std::vector<Clue> clues = crossword.Clues();
    Clue cur = GetCurrentClue();
    assert(clues.size() > 0); // should fail when we have too restrictive a pattern
    bool n = false;
    auto it = std::begin(clues);
    for (it = std::begin(clues); it != std::end(clues); ++it) {
      if (n) {
        ClickCell(it->GetStart());
        if (it->GetDirection() == kDOWN) {
          ClickCell(it->GetStart());
        }
        break;
      }
      if (it->SameCoords(cur)) {
        n = true;
      }
    }
    if (it == std::end(clues)) {
      ClickCell(clues.front().GetStart());
    }
  } else if (code == 32) // SPACE
  {
    // move forward one
    Clue clue = GetCurrentClue();
    int i = clue.IndexOfCoord(selected);
    if (i != kNO_NUMBER && (std::size_t) i < clue.GetSize() - 1) {
      SetGridCursor(clue.coord_list_[i + 1]);
    }
    UpdateGrid();
  } else if (code == 127 || code == 8) // DEL, BACKSPACE
  {
    DeleteOne();
  } else if (std::isalpha(code)) {
    EnterCharacter(toupper(code));
  }
}

/**
 * @brief Event handler for left-click on grid cell
 *
 * @param event
 */
void CrosswordApp::OnGridCellLeftClick(wxGridEvent &event) {
  event.Skip(false);
  Coord click_coords = Coord(event.GetRow(), event.GetCol());
  if (!crossword.Get(click_coords).IsBarrier())
    ClickCell(click_coords);
}

/**
 * @brief Event handler for right-click on grid cell
 *
 * @param event
 */
void CrosswordApp::OnGridCellRightClick(wxGridEvent &event) {
  if (is_searching)
    return;

  crossword.ToggleBarrier(Coord(event.GetRow(), event.GetCol()), GetRotationalSymmetry());
  UpdateGrid();
  SelectFirstClue();
  ClearGridSelection();
}

/**
 * @brief Handler for set clue menu item
 *
 * @param event
 */
void CrosswordApp::OnSetClue(wxCommandEvent &) {
  if (is_searching)
    return;
  std::string hint = crossword.GetHint(GetCurrentClue());
  std::string direction = "Across";
  Clue clue = GetCurrentClue();
  if (clue.GetDirection() == kDOWN) {
    direction = "Down";
  }
  std::string message =
          "Enter clue for " + std::to_string(clue.GetNumber()) + "-" + direction + ":";
  wxTextEntryDialog *dialog = new wxTextEntryDialog(
          this, message, "Enter Clue", hint, wxOK | wxCANCEL);
  if (dialog->ShowModal() == wxID_OK) {
    crossword.SetHint(clue, dialog->GetValue().ToStdString());
  }
  dialog->Destroy();
}

/**
 * @brief Handler for grid range select
 *
 * Should do nothing; if it interacts with grid selection this gets called recursively
 * and we can stack overflow.
 *
 * @param event
 */
void CrosswordApp::OnGridRangeSelect(wxGridRangeSelectEvent &event) {
  event.Skip(false);
}

/**
 * @brief Handler for grid drag (bound to mouse move)
 *
 * Calls this very often, maybe need to rein this in.
 *
 * @param event
 */
void CrosswordApp::OnGridDrag(wxMouseEvent &event) {
  event.Skip(false);
}

/**
 * @brief Handler for exit menu item
 *
 * @param event
 */
void CrosswordApp::OnExit(wxCommandEvent &) {
  crossword.StopAutofill();
  Close(true);
}

/**
 * @brief Handler for about menu item
 *
 * @param event
 */
void CrosswordApp::OnAbout(wxCommandEvent &) {
  wxAboutDialogInfo info;
  info.SetName(_("Crossword Editor"));
  info.SetVersion(_("0.0"));
  info.SetDescription(_("Cross platform crossword editing tool. \n https://github.com/jeromew21/crossword-gui"));
  info.SetCopyright(wxT("(C) 2022 Jerome Wei <jeromejwei@gmail.com>"));

  wxAboutBox(info, this);
}

/**
 * @brief Handler for clear word menu item
 *
 * @param event
 */
void CrosswordApp::OnClearWord(wxCommandEvent &) {
  if (is_searching)
    return;

  if (GetCurrentClue().GetSize() > 0) {
    crossword.ClearClue(GetCurrentClue());
  } else {
    ErrorDialog("No word selected.");
  }
  UpdateGrid();
}

/**
 * @brief Handler for fill word menu item
 *
 * @param event
 */
void CrosswordApp::OnFillWord(wxCommandEvent &) {
  if (is_searching)
    return;

  if (GetCurrentClue().IsFilled()) {
    ErrorDialog("Word is already filled.");
    return;
  }
  int clue_size = GetCurrentClue().GetSize();
  if (clue_size > 0) {
    Clue clue = GetCurrentClue();
    std::vector<DatabaseEntry> solutions = db.GetSolutions(clue, kNO_NUMBER, 1);
    if (solutions.size() > 0) {
      // sort choices descending
      std::sort(solutions.begin(), solutions.end(), [](const DatabaseEntry &lhs, const DatabaseEntry &rhs) {
        return lhs.frequency_score > rhs.frequency_score;
      });

      WordListDialog *dialog = new WordListDialog(solutions);
      if (dialog->ShowModal() == wxID_OK) {
        int sel = dialog->GetSelection();
        if (sel != wxNOT_FOUND) {
          crossword.SetClue(GetCurrentClue(), solutions[sel].entry);
        }
      }
      dialog->Destroy();
    } else {
      ErrorDialog("No words found.");
    }
  } else {
    ErrorDialog("No word selected.");
  }
  UpdateGrid();
}

/**
 * @brief Called when user elects to resize the grid.
 *
 * @param event
 */
void CrosswordApp::OnResizeGrid(wxCommandEvent &) {
  if (is_searching)
    return;

  ResizeGridDialog *dialog = new ResizeGridDialog(
          crossword.GetWidth(),
          crossword.GetHeight());
  if (dialog->ShowModal() == wxID_OK) {
    ResizeGrid(dialog->GetHeight(), dialog->GetWidth());
  }
  dialog->Destroy();
  UpdateGrid();
  ClearGridSelection();
}

/**
 * @brief Called when user clicks on "Reset" object.
 *
 * @param event
 */
void CrosswordApp::OnReset(wxCommandEvent &) {
  if (is_searching)
    return;

  crossword.ClearAtoms();
  UpdateGrid();
}

/**
 * @brief User elected to undo.
 *
 * @param event
 */
void CrosswordApp::OnUndo(wxCommandEvent &) {
  if (is_searching)
    return;

  if (crossword.Undo()) {
    UpdateGrid();
  }
}

/**
 * @brief User elected to redo.
 *
 * @param event
 */
void CrosswordApp::OnRedo(wxCommandEvent &) {
  if (is_searching)
    return;

  if (crossword.Redo()) {
    UpdateGrid();
  }
}

/**
 * @brief Thread function for updating the grid periodically during a search.
 *
 * @param app
 */
void UpdateGridThreadFunc(CrosswordApp *app) {
  int ms = 1000; // frequency of grid update, higher hangs UI less and isn't noticeable?
  // weirdly, the grid updates slowly so that changes are reflected *during* redraw; need to optimize drawing methods
  while (app->is_searching) {
    wxCommandEvent evnt(GRID_REFRESH);
    wxPostEvent(app, evnt);
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
  }
}

/**
 * @brief Thread function responsible for visualizing the progress of the search algorithm.
 *
 * @param app
 */
void AutofillThreadFunc(CrosswordApp *app) {
  AutofillParams params(&app->db);

  wxCommandEvent evnt(SEARCHING);
  wxPostEvent(app, evnt);

  app->crossword.Autofill(params);
  app->is_searching = false;

  wxCommandEvent evnt1(DONE_SEARCHING);
  wxPostEvent(app, evnt1);

  wxCommandEvent evnt2(GRID_REFRESH);
  wxPostEvent(app, evnt2);
}

/**
 * @brief User elected to autofill.
 *
 * @param event
 */
void CrosswordApp::OnAutofill(wxCommandEvent &) {
  // TODO: disable menu bar items

  if (is_searching) {
    ErrorDialog("Already searching.");
    return;
  }
  if (!crossword.IsValidPattern()) {
    ErrorDialog("Must be a valid puzzle in order to autofill.");
    return;
  }

  // Block UI in the case that the database is still loading.
  if (!db.IsFinishedLoading()) {
    db.WaitForLock();
  }
  db.FlushCaches(); // TODO: figure out why we need this here

  auto all_clues = crossword.Clues();
  Solvability current_solvable_status = crossword.IsInvalidPartial(all_clues, db, 1);
  if (current_solvable_status != Solvability::Solvable) {
    if (current_solvable_status == Solvability::Overdetermined)
      ErrorDialog("Cannot solve (overdetermined).");
    else if (current_solvable_status == Solvability::Invalid)
      ErrorDialog("Cannot solve (invalid).");
    else if (current_solvable_status == Solvability::Duplicate)
      ErrorDialog("Cannot solve (duplicate).");
    else
      ErrorDialog("Cannot solve (other).");
    return;
  }

  is_searching = true;

  std::thread autofill(AutofillThreadFunc, this);
  std::thread grid_func(UpdateGridThreadFunc, this);

  autofill.detach();
  grid_func.detach();
}

/**
 * @brief Called when user resizes window.
 *
 */
void CrosswordApp::OnResize(wxSizeEvent &) {
  delete grid;
  InitGrid();
  UpdateGrid();
}

/**
 * @brief Handler for add to dictionary menu item
 *
 * @param event
 */
void CrosswordApp::OnAddToDictionary(wxCommandEvent &) {
  if (is_searching)
    return;

  Clue clue = GetCurrentClue();

  if (!clue.IsFilled()) {
    ErrorDialog("Clue is filled; cannot add to dictionary.");
    return;
  }

  Word word = clue.ToWord();

  if (db.ContainsEntry(word)) {
    ErrorDialog("\"" + word.ToString() + "\" already in the dictionary.");
    return;
  }

  // Make the score very large because we want to assure the solver
  // doesn't miss out on it.
  db.AddEntry(word, 100, 100);

  UpdateGrid();
}

/**
 * @brief Event handler for the custom event GRID_REFRESH.
 *
 * Updates the UI grid.
 *
 * @param event
 */
void CrosswordApp::OnGridRefresh(wxCommandEvent &) {
  UpdateGrid();
}

/**
 * @brief Custom event handler. Represents the begin of a search.
 *
 * @param event
 */
void CrosswordApp::OnSearching(wxCommandEvent &) {
  SetStatusText("Searching");
}

/**
 * @brief Custom event handler. Represents end of a search.
 *
 * @param event
 */
void CrosswordApp::OnDoneSearching(wxCommandEvent &) {
  SetStatusText("Ready");
}

/**
 * @brief Show logging window
 * 
 * TODO: this
 * 
 */
void CrosswordApp::OnShowLogs(wxCommandEvent &) {
  //std::cout << GetLogs() << std::endl;
  //wxMessageDialog *dialog = new wxMessageDialog(this, GetLogs(), "Logs");
  //dialog->ShowModal();
  //dialog->Destroy();
}

/**
 * @brief Handler for word info menu item
 *
 * @param event
 */
void CrosswordApp::OnWordInfo(wxCommandEvent &) {
  if (is_searching)
    return;

  Clue cur = GetCurrentClue();
  if (!cur.IsFilled()) {
    ErrorDialog("Word not filled");
    return;
  }

  Word word = GetCurrentClue().ToWord();
  if (!db.ContainsEntry(word)) {
    ErrorDialog("Word not in dictionary");
    return;
  }
  std::string message =
          word.ToString() + " (" + std::to_string(word.size()) + " letters)\n" + "Frequency score: " +
          std::to_string(db.GetFrequencyScore(word));
  wxMessageDialog *dialog = new wxMessageDialog(this, message, "Word Information");
  dialog->ShowModal();
  dialog->Destroy();
}

/**
 * @brief Handler for lock cell menu item
 *
 * @param event
 */
void CrosswordApp::OnLockCell(wxCommandEvent &) {
  if (is_searching)
    return;

  crossword.LockCell(selected, !crossword.IsLocked(selected));
  UpdateGrid();
}

void CrosswordApp::OnExport(wxCommandEvent &) {
  if (is_searching)
    return;

  wxFileDialog
          openFileDialog(this, _("Open PDF file"), "", "",
                         "PDF files (*.pdf)|*.pdf", wxFD_SAVE);
  if (openFileDialog.ShowModal() == wxID_CANCEL)
    return;

  ExportPDF(openFileDialog.GetPath().ToStdString());
}

void CrosswordApp::OnLoadDatabase(wxCommandEvent &event) {
  if (is_searching)
    return;

  wxFileDialog
          openFileDialog(this, _("Open CSV file"), "", "",
                         "CSV files (*.csv)|*.csv", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
  if (openFileDialog.ShowModal() == wxID_CANCEL)
    return;

  std::string filename = openFileDialog.GetPath().ToStdString();
  LoadDatabaseFromCSV(filename);
}

void CrosswordApp::OnOpen(wxCommandEvent &) {
  if (is_searching)
    return;

  wxFileDialog
          openFileDialog(this, _("Open CROSSWORD file"), "", "",
                         "CROSSWORD files (*.crossword)|*.crossword", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
  if (openFileDialog.ShowModal() == wxID_CANCEL)
    return;

  // proceed loading the file chosen by the user;
  std::vector<std::string> lines;
  lines.reserve(300000);
  std::string line;
  std::string filename = openFileDialog.GetPath().ToStdString();
  std::ifstream db(filename);
  if (db.is_open()) {
    while (std::getline(db, line)) {
      lines.push_back(line);
    }
    crossword.Unserialize(lines);
    open_file = filename;
    UpdateGrid();
    SelectFirstClue();
    ClearGridSelection();
    crossword.logger.Log("Loaded from file \"" + filename + "\"");
  } else {
    crossword.logger.Log("Open failed");
  }
}

void CrosswordApp::OnSave(wxCommandEvent &) {
  if (is_searching)
    return;
  crossword.logger.Log(open_file);
  if (!open_file.empty())
    SaveToFile(open_file);
  else
    crossword.logger.Log("No open file");
}

void CrosswordApp::OnSaveAs(wxCommandEvent &) {
  if (is_searching)
    return;

  wxFileDialog
          saveFileDialog(this, _("Save CROSSWORD file"), "", "",
                         "CROSSWORD files (*.crossword)|*.crossword", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
  if (saveFileDialog.ShowModal() == wxID_CANCEL)
    return;

  // save the current contents in the file
  std::string filename = saveFileDialog.GetPath().ToStdString();
  SaveToFile(filename);
}

void CrosswordApp::OnStopAutofill(wxCommandEvent &) {
  crossword.StopAutofill();
}

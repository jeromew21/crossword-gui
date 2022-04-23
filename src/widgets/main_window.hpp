/**
 * @file main_window.hpp
 * @author Jerome Wei (jeromejwei@gmail.com)
 * @brief Header file for the UI main window.
 * @version 0.1
 * @date 2022-04-01
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef UI_H
#define UI_H

#include <wx/wxprec.h>

#ifndef WX_PRECOMP

#include <wx/wx.h>

#endif

#include <wx/grid.h>

#include "crossword/crossword.hpp"

/**
 * @brief The background color for non-barrier cells.
 *
 */
const wxColor kWHITE = wxColour(255, 255, 255);

/**
 * @brief The background color for barrier cells.
 *
 */
const wxColor kBLACK = wxColour(0, 0, 0);

/**
 * @brief The background color for invalid cells.
 *
 */
const wxColor kRED = wxColour(255, 87, 101);

/**
 * @brief The background color for greyed-out invalid cells.
 *
 */
const wxColor kGRAY = wxColour(100, 100, 100);

/**
 * @brief Mapping from index to color.
 *
 */
const wxColor COLOR_MAP[3] = {kWHITE, kRED, kGRAY};

/**
 * @brief Custom event that we fire when we want the grid to be refreshed.
 *
 */
wxDEFINE_EVENT(GRID_REFRESH, wxCommandEvent);

/**
 * @brief Custom event that represents search beginning.
 *
 */
wxDEFINE_EVENT(SEARCHING, wxCommandEvent);

/**
 * @brief Custom event that represents search completion.
 *
 */
wxDEFINE_EVENT(DONE_SEARCHING, wxCommandEvent);

enum {
  ID_Enforce_Symmetry = 2,
  ID_Resize_Grid = 3,
  ID_Fill_Word = 4,
  ID_Clear_Word = 5,
  ID_Reset = 7,
  ID_Lock_Cell = 8,
  ID_Spellcheck = 9,
  ID_Set_Clue = 10,
  ID_Export = 11,
  ID_Autofill = 12,
  ID_Undo = 13,
  ID_Redo = 14,
  ID_Show_Puzzle_Info = 15,
  ID_Add_To_Dictionary = 16,
  ID_Word_Info = 17,
  ID_Save = 18,
  ID_Save_As = 19,
  ID_Open = 20,
  ID_Show_Logs = 21,
  ID_Stop_Autofill = 22,
  ID_Load_Database = 23
};

/**
 * @brief Command line options are abstracted into this structure.
 *
 */
struct MainWindowOptions {
  bool db;
  std::string db_filename;
  bool silent;

  MainWindowOptions() : db{false}, silent{false} {};
};

/**
 * @brief The main wxFrame, containing state and API of the app.
 *
 */
class CrosswordApp : public wxFrame {
public:
  /**
   * @brief Current crossword puzzle state.
   *
   */
  crossword_backend::Crossword crossword;

  /**
   * @brief Current puzzle database.
   *
   */
  crossword_backend::WordDatabase db;

  /**
   * @brief Current default grid colors.
   *
   */
  std::array<std::array<int, crossword_backend::kMAX_DIM>, crossword_backend::kMAX_DIM> grid_colors;

  /**
   * @brief True iff the app is currently searching.
   *
   */
  std::atomic<bool> is_searching;

  int CellSize();

  CrosswordApp(MainWindowOptions const &options);

private:
  /**
   * @brief Grid UI element.
   *
   */
  wxGrid *grid;

  /**
   * @brief Currently open file.
   *
   */
  std::string open_file;

  /**
   * @brief Currently selected clue.
   *
   */
  crossword_backend::Clue current_clue;

  /**
   * @brief Currently selected grid coordinate.
   *
   * Should be a sub-coordinate of current_clue, but no
   * guarantee.
   *
   */
  crossword_backend::Coord selected;

  /**
   * @brief Represents the user's on screen selection, of multiple grid blocks.
   *
   * For now, remains simply a boolean.
   *
   */
  bool user_selection;

  /**
   * @brief Edit menu.
   *
   * Needed to retrieve checkbox items.
   *
   */
  wxMenu *menuEdit;

  /**
   * @brief Menu bar
   *
   */
  wxMenuBar *menuBar;

  void ExportPDF(std::string const &filename);

  void SaveToFile(std::string const &filename);

  void UpdateGrid();

  bool GetRotationalSymmetry();

  void LoadDatabaseFromCSV(std::string const &filename);

  bool GetSpellcheck();

  crossword_backend::Clue &GetCurrentClue();

  crossword_backend::Coord GetGridCursor();

  /* Initialization */
  void InitMenuBar();

  void InitStatusBar();

  void InitGrid();

  void InitBindings();

  void ResetGridColors();

  /* Message methods */
  static void ErrorDialog(std::string const &message);

  /* Grid methods */
  void ResizeGrid(int height, int width);

  void SetGridCursor(crossword_backend::Coord c);

  void ClearGridSelection();

  void SelectWordBlock(crossword_backend::Coord c0, crossword_backend::Coord c1);

  void ClickCell(crossword_backend::Coord click_coords);

  void SelectFirstClue();

  void DeleteOne();

  void EnterCharacter(char c);

  /* Menu events */
  void OnExit(wxCommandEvent &event);

  void OnAbout(wxCommandEvent &event);

  void OnResizeGrid(wxCommandEvent &event);

  void OnFillWord(wxCommandEvent &event);

  void OnSuggestWord(wxCommandEvent &event);

  void OnClearWord(wxCommandEvent &event);

  void OnReset(wxCommandEvent &event);

  void OnSetClue(wxCommandEvent &event);

  void OnFillOne(wxCommandEvent &event);

  void OnAutofill(wxCommandEvent &event);

  void OnAddToDictionary(wxCommandEvent &event);

  void OnUndo(wxCommandEvent &event);

  void OnRedo(wxCommandEvent &event);

  void OnGridRefresh(wxCommandEvent &event);

  void OnSearching(wxCommandEvent &event);

  void OnDoneSearching(wxCommandEvent &event);

  void OnShowLogs(wxCommandEvent &event);

  void OnWordInfo(wxCommandEvent &event);

  void OnLockCell(wxCommandEvent &event);

  void OnExport(wxCommandEvent &event);

  void OnOpen(wxCommandEvent &event);

  void OnSave(wxCommandEvent &event);

  void OnSaveAs(wxCommandEvent &event);

  void OnStopAutofill(wxCommandEvent &event);

  void OnLoadDatabase(wxCommandEvent &event);

  /* Generic events */
  void OnGridCellClick(wxGridEvent &event);

  void OnGridCellLeftClick(wxGridEvent &event);

  void OnGridCellRightClick(wxGridEvent &event);

  void OnGridRangeSelect(wxGridRangeSelectEvent &event);

  void OnGridDrag(wxMouseEvent &event);

  void OnKeyDown(wxKeyEvent &event);

  void OnResize(wxSizeEvent &event);
};

/**
 * @brief Renders a grid cell on the screen.
 *
 */
class CellRenderer : public wxGridCellRenderer {
public:
  /**
   * @brief Parent window.
   *
   */
  CrosswordApp *app;

  wxFont cell_number_font;
  int cell_number_font_size;

  /**
   * @brief Base class that provides default render behavior.
   *
   */
  wxGridCellStringRenderer base_cell_renderer;

  void
  Draw(wxGrid &grid, wxGridCellAttr &attr, wxDC &dc, const wxRect &rect, int row, int col, bool isSelected) override;

  wxGridCellRenderer *Clone() const override;

  wxSize GetBestSize(wxGrid &grid, wxGridCellAttr &attr, wxDC &dc, int row, int col) override;

  /**
   * @brief Create new cell renderer object.
   *
   * @param app
   */
  CellRenderer(CrosswordApp *app, wxFont font, int cell_number_font_size) : app(app), cell_number_font(font),
                                                                            cell_number_font_size(
                                                                                    cell_number_font_size) {};

protected:
  ~CellRenderer() = default;
};

#endif

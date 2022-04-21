/**
 * @file dialog.hpp
 * @author Jerome Wei (jeromejwei@gmail.com) 
 * @brief Header file for custom UI dialogs.
 * @version 0.1
 * @date 2022-04-04
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef DIALOG_H
#define DIALOG_H

#include <wx/wxprec.h>

#ifndef WX_PRECOMP

#include <wx/wx.h>

#endif

#include "crossword/word.hpp"
#include "crossword/database.hpp"

#include <vector>

/**
 * @brief Dialog presented to the user for resizing the crossword grid.
 * 
 */
class ResizeGridDialog : public wxDialog {
public:
  int GetWidth();

  int GetHeight();

  ResizeGridDialog(int width, int height);

private:
  wxTextCtrl *tc_h;
  wxTextCtrl *tc_w;
};

/**
 * @brief Dialog presented to the user for selecting a word from a list.
 * 
 */
class WordListDialog : public wxDialog {
public:
  int GetSelection();

  WordListDialog(std::vector<crossword_backend::DatabaseEntry> words);

private:
  wxListBox *lb;
};

#endif

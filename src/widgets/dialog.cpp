/**
 * @file dialog.cpp
 * @author Jerome Wei (jeromejwei@gmail.com)
 * @brief Custom dialog implementation
 * @version 0.1
 * @date 2022-04-11
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "widgets/dialog.hpp"

using namespace crossword_backend;

static const std::size_t MAX_CHOICES = 2000;

int ResizeGridDialog::GetHeight() {
  return std::stoi(tc_h->GetValue().ToStdString());
}

int ResizeGridDialog::GetWidth() {
  return std::stoi(tc_w->GetValue().ToStdString());
}

ResizeGridDialog::ResizeGridDialog(int width, int height)
        : wxDialog(
        NULL,
        -1,
        "Change grid dimensions",
        wxDefaultPosition,
        wxSize(250, 230)) {

  wxPanel *panel = new wxPanel(this, -1);

  wxBoxSizer *vbox = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer *hbox = new wxBoxSizer(wxHORIZONTAL);

  wxStaticBox *st = new wxStaticBox(panel,
                                    -1, wxT("Dimensions"),
                                    wxPoint(5, 5), wxSize(240, 150));

  tc_h = new wxTextCtrl(panel, -1, std::to_string(height),
                        wxPoint(15, 105));

  tc_w = new wxTextCtrl(panel, -1, std::to_string(width),
                        wxPoint(95, 105));

  wxSizer *button_sizer = CreateButtonSizer(wxOK | wxCANCEL);

  vbox->Add(panel, 1);
  vbox->Add(hbox, 0, wxALIGN_CENTER | wxTOP | wxBOTTOM, 10);
  vbox->Add(button_sizer, 1);

  SetSizer(vbox);

  // Centre();
}

/**
 * @brief Get the selected word.
 * 
 * @return int 
 */
int WordListDialog::GetSelection() {
  return lb->GetSelection();
}

/**
 * @brief Displays to the user the first n=min(MAX_CHOICES, words.size()) elements of input vector.
 * 
 * @param words vector of DatabaseEntry objects.
 */
WordListDialog::WordListDialog(std::vector<Word> words)
        : wxDialog(
        NULL,
        -1,
        "Select word",
        wxDefaultPosition,
        wxSize(250, 330)) {
  wxPanel *panel = new wxPanel(this, -1);

  wxBoxSizer *vbox = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer *hbox = new wxBoxSizer(wxHORIZONTAL);

  wxString choices[MAX_CHOICES];
  std::size_t list_size = std::min(MAX_CHOICES, words.size());

  for (std::size_t i = 0; i < list_size; i++) {
    choices[i] = words[i].ToString();
  }

  lb = new wxListBox(panel, -1,
                     wxPoint(5, 5), wxSize(240, 150),
                     list_size, choices);
  lb->SetSelection(0);

  wxSizer *button_sizer = CreateButtonSizer(wxOK | wxCANCEL);

  vbox->Add(panel, 1);
  vbox->Add(hbox, 0, wxALIGN_CENTER | wxTOP | wxBOTTOM, 10);
  vbox->Add(button_sizer, 1);

  SetSizer(vbox);
}
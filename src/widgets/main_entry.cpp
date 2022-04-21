/**
 * @file main.cpp
 * @author Jerome Wei (jeromejwei@gmail.com)
 * @brief Main entry into app
 * @version 0.1
 * @date 2022-04-11
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "widgets/main_window.hpp"
#include <wx/cmdline.h>

/**
 * @brief The main wxWidgets app.
 *
 */
class App : public wxApp {
public:
  MainWindowOptions cmd_line_options_;

  int OnExit() override;

  bool OnInit() override;

  void OnInitCmdLine(wxCmdLineParser &parser) override;

  bool OnCmdLineParsed(wxCmdLineParser &parser) override;
};

static const wxCmdLineEntryDesc g_cmdLineDesc[] =
        {
                {wxCMD_LINE_SWITCH, "h", "help",     "displays help on the command line parameters",
                        wxCMD_LINE_VAL_NONE, wxCMD_LINE_OPTION_HELP},
                {wxCMD_LINE_SWITCH, "s", "silent",   "does not output logs to stdout"},

                {wxCMD_LINE_OPTION, "d", "database", "database to load initially", wxCMD_LINE_VAL_STRING},
                {wxCMD_LINE_NONE}
        };

/**
 * @brief Exiting the app.
 *
 * @return int
 */
int App::OnExit() {
  return 0;
}

/**
 * @brief Initialization of the main window.
 * 
 */
bool App::OnInit() {
  if (!wxApp::OnInit())
    return false;

  auto *main_window = new CrosswordApp(cmd_line_options_);
  main_window->Show(true);
  return true;
}

void App::OnInitCmdLine(wxCmdLineParser &parser) {
  parser.SetDesc(g_cmdLineDesc);
  // must refuse '/' as parameter starter or cannot use "/path" style paths
  parser.SetSwitchChars(wxT("-"));
}

bool App::OnCmdLineParsed(wxCmdLineParser &parser) {
  bool silent_mode = parser.Found(wxT("s"));
  cmd_line_options_.silent = silent_mode;

  wxString *out_db = new wxString("");
  bool db = parser.Found("d", out_db);
  if (db) {
    cmd_line_options_.db = true;
    cmd_line_options_.db_filename = out_db->ToStdString();
  }

  return true;
}

wxIMPLEMENT_APP(App); // main macro

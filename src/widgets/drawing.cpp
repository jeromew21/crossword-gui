/**
 * @file crossword_drawing.cpp
 * @author Jerome Wei (jeromejwei@gmail.com)
 * @brief Draw PDF of crossword puzzle with Cairo.
 * @version 0.1
 * @date 2022-04-13
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "widgets/main_window.hpp"

#include <cairo-pdf.h>

#include <algorithm>

#if defined(__gnu_linux__)
#define _CRT_ASSERT 0

int _CrtSetReportMode(int, int) {
  return 0;
}

#define _strdup strdup
#else
#endif

using namespace crossword_backend;

/**
 * @brief Write out a PDF to file at filename.
 *
 * TODO: create some line-breaking abstractions and height/width calculations
 *
 * @param filename
 */
void CrosswordApp::ExportPDF(std::string const &filename) {
  // Disable annoying assertions in Windows
  int iPrev = _CrtSetReportMode(_CRT_ASSERT, 0);

  auto clues = crossword.Clues();

  int height_ = crossword.GetHeight();
  int width_ = crossword.GetWidth();
  double width_in = 8.5;
  double height_in = 11;

  double width_pt = width_in * 72.;
  double height_pt = height_in * 72.;

  cairo_surface_t *surface = cairo_pdf_surface_create(filename.c_str(), width_pt, height_pt);
  cairo_t *cr = cairo_create(surface);

  cairo_set_source_rgb(cr, 0, 0, 0);

  double puzzle_width = width_pt * .66;
  double paddingX = 10;
  double paddingY = paddingX;
  double sq_width = puzzle_width / (double) width_;

  double line_width = sq_width / 60.;
  cairo_set_line_width(cr, line_width);

  for (int x = 0; x < width_; ++x) {
    for (int y = 0; y < height_; ++y) {
      double rx = paddingX + x * sq_width;
      double ry = paddingY + y * sq_width;
      cairo_rectangle(cr, rx, ry, sq_width, sq_width);
      if (crossword.Get(Coord(y, x)).IsBarrier()) {
        cairo_fill_preserve(cr);
      }
      cairo_stroke(cr);
    }
  }

  // Draw little numbers
  double font_size = sq_width * 0.2;
  cairo_set_font_size(cr, font_size);
  for (int x = 0; x < width_; ++x) {
    for (int y = 0; y < height_; ++y) {
      double rx = paddingX + x * sq_width;
      double ry = paddingY + y * sq_width;
      int num = crossword.GetClueNumber(Coord(y, x));
      if (num != kNO_NUMBER) {
        char *val = _strdup(std::to_string(num).c_str());
        cairo_text_extents_t extents;
        cairo_text_extents(cr, val, &extents);
        double pad = font_size / 5;
        cairo_move_to(cr, rx + pad, ry + extents.height + pad);
        cairo_show_text(cr, val);
        free(val);
      }
    }
  }

  std::vector<int> clue_nums_across;
  std::vector<int> clue_nums_down;
  for (auto it = std::begin(clues); it != std::end(clues); ++it) {
    if (it->GetDirection() == kACROSS) {
      clue_nums_across.push_back(it->GetNumber());
    } else {
      clue_nums_down.push_back(it->GetNumber());
    }
  }
  std::sort(clue_nums_across.begin(), clue_nums_across.end());
  std::sort(clue_nums_down.begin(), clue_nums_down.end());

  font_size = (height_pt - (paddingY + sq_width * height_)) /
              ((double) std::max(clue_nums_across.size(), clue_nums_down.size()) + 1.);
  font_size = font_size * 0.6;
  cairo_set_font_size(cr, font_size);
  double yt = 5 * paddingY + sq_width * height_;
  double xt = paddingX;
  cairo_move_to(cr, paddingX, yt);
  cairo_show_text(cr, "Across");
  int i = 1;
  for (auto it = std::begin(clue_nums_across); it != std::end(clue_nums_across); ++it) {
    cairo_move_to(cr, xt, yt + font_size * i);
    char *val = _strdup(std::string(std::to_string(*it) + ". " + crossword.GetHint(*it, kACROSS)).c_str());
    cairo_show_text(cr, val);
    i++;
    free(val);
  }

  xt = paddingX + width_pt / 2.;
  cairo_move_to(cr, xt, yt);
  cairo_show_text(cr, "Down");
  i = 1;
  for (auto it = std::begin(clue_nums_down); it != std::end(clue_nums_down); ++it) {
    cairo_move_to(cr, xt, yt + font_size * i);
    char *val = _strdup(std::string(std::to_string(*it) + ". " + crossword.GetHint(*it, kDOWN)).c_str());
    cairo_show_text(cr, val);
    i++;
    free(val);
  }

  cairo_destroy(cr);
  cairo_surface_destroy(surface);

  crossword.logger.Log("Drew current puzzle to file \"" + filename + "\"");

  _CrtSetReportMode(_CRT_ASSERT, iPrev);
}

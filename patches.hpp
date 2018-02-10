#ifndef PATCHES_HPP
#define PATCHES_HPP

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <array>
#include <vector>

#define MAXLINES 10000
#define ORIGIN MAXLINES

/*
 * document
 * patch
 * patch : document -> document, (x -> y)
 * patch^-1 : document -> document, (y -> x)
 * patch*patch^-1 : id_x
 * patch^-1*patch : id_y
 * patch^-1^-1 = patch
 * Communation: PQ = Q'P'
 * Merging:
 *   - Commute A^-1B, B'(A^-1)', drop (A^-1)'
 *   - A + B = AB'
 *
 */

enum edit_ops { INSERT, DELETE };

class Edit {
  friend bool edit_comp(Edit x, Edit y);

public:
  Edit(edit_ops new_op = INSERT, int first_line = 0, int second_line = 0, std::string v = "") {
    op = new_op;
    line1 = first_line;
    line2 = second_line;
    val = v;
  };

  void set_op(edit_ops new_op) { op = new_op; }
  void set_line1(int new_line) { line1 = new_line; }
  void set_line2(int new_line) { line2 = new_line; }
  void set_val(std::string v) { val = v; }

  edit_ops get_op() { return op; }
  int get_line1() { return line1; }
  int get_line2() { return line2; }
  std::string get_val() { return val; }

  friend std::ostream& operator<< (std::ostream& stream, const Edit &e) {
    if (e.op == INSERT) {
      stream << "+" << e.line1 << " " << e.val;
    }
    else {
      stream << "-" << e.line1 << " " << e.val;
    }

    return stream;
  }


protected:
  edit_ops op;
  int line1;
  int line2;
  std::string val;
};


bool edit_comp(Edit x, Edit y) { return x.line1 < y.line1; }

void edit_sort(std::vector<Edit> &vec) {
  std::sort(vec.begin(), vec.end(), edit_comp);
}

class Document;
class Patch;
class CompositePatch;
class CompositeDocument;

class Patch {
  /*
   * patch(document) - patch: x -> x
   * patch[document] - patch: x -> y, create CompositeDocument
   * ! - patch^-1
   * * - patch*patch
   * , - patch, patch -> composite patch', patch' - commute
   */

public:

  Patch() { edits = {}; }
  Patch(std::vector<Edit> l) { edits = l; }

  void apply(Document &doc) const {}
  void operator()(Document &doc) { this->apply(doc); }

  Patch invert() const { return *this; }
  Patch operator!() { return this->invert(); }

  Edit get_last() { return this->edits.back(); }

  void commute(Patch p2) {}

  Patch invert() {
    std::vector<Edit> ret;

    for (auto& e : edits) {

      if (e.get_op() == INSERT) {
        ret.push_back(Edit(DELETE, e.get_line1(), e.get_line2(), e.get_val()));
      }
      else {
        ret.push_back(Edit(INSERT, e.get_line1(), e.get_line2(), e.get_val()));
      }

    }

    return Patch(ret);
  }

  bool is_empty() { return edits.size() == 0; }

  friend std::ostream& operator<< (std::ostream& stream, const Patch& p) {
    for (auto i = p.edits.begin(); i != p.edits.end(); i++)
      stream << *i << std::endl;

    return stream;
  }

private:
  std::vector<Edit> edits;
};


class CompositePatch : public Patch {
  /*
   * Composite * patch -> Composite
   * Composite ,
   */
};

class Document {
  /*
   * + - merge
   * - - document - document = patch
   */
public:

  Document(std::ifstream &file) {
    std::string tmp;

    while (!(file.eof())) {
      std::getline(file, tmp);

      val.push_back(tmp);
    }
  }

  Document merge(const Document &doc) const { return doc; }
  Document operator+(const Document &doc) const { return this->merge(doc); }

  std::vector<std::string> to_vec() const { return val; }

  Patch diff(const Document &doc) const {
    using namespace std;

    int max_d, m, n, lower, upper, d, k, row, col;

    max_d = 2 * MAXLINES;

    array<int, 2 * MAXLINES + 1> last_d;
    array<vector<Edit>, 2 * MAXLINES + 1> script;

    vector<string> B = doc.to_vec();

    m = val.size();
    n = B.size();

    for (row = 0; row < m && row < n && val[row] == B[row]; ++row)
      ;

    last_d[ORIGIN] = row;
    script[ORIGIN] = {};
    lower = (row == m) ? ORIGIN + 1 : ORIGIN - 1;
    upper = (row == n) ? ORIGIN - 1 : ORIGIN + 1;

    if (lower > upper) {
      return Patch();
    }

    for (d = 1; d <= max_d; ++d) {
      for (k = lower; k <= upper; k += 2) {
        vector<Edit> old_list;
        Edit new_edit = Edit();

        if (k == ORIGIN - d ||
            k != ORIGIN + d && last_d[k + 1] >= last_d[k - 1]) {
          row = last_d[k + 1] + 1;
          old_list = script[k + 1];
          new_edit.set_op(DELETE);
          new_edit.set_val(val[row-1]);
        } else {
          row = last_d[k - 1];
          old_list = script[k - 1];
          new_edit.set_op(INSERT);
          new_edit.set_val(B[(col = row + k - ORIGIN)-1]);
        }


        new_edit.set_line1(row+1);
        new_edit.set_line2(col+1);
        old_list.push_back(new_edit);
        script[k] = old_list;

        while (row < n && col < n && val[row] == B[col]) {
          ++row;
          ++col;
        }
        last_d[k] = row;

        if (row == m && col == n) {
          edit_sort(script[k]);
          return Patch(script[k]);
        }

        if (row == m)
          lower = k + 2;

        if (col == n)
          upper = k - 2;
      }

      --lower;
      ++upper;
    }
  }
  Patch operator-(const Document &doc) const { return this->diff(doc); }

private:
  std::vector<std::string> val;
};

class CompositeDocument : public Patch, public Document {};

#endif

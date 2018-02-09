#ifndef PATCHES_HPP
#define PATCHES_HPP
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

    void apply(Document& doc) const {  }
    void operator()(Document& doc) { this->apply(doc); }

    Patch invert() const { return *this; }
    Patch operator!() { return this->invert(); }



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

    Document merge(const Document& doc) const { return doc; }
    Document operator+(const Document& doc) const { return this->merge(doc); }

    Patch diff(const Document& doc) const { return Patch(); }
    Patch operator-(const Document& doc) const { return this->diff(doc); }


};

class CompositeDocument : public Patch, public Document {};

#endif

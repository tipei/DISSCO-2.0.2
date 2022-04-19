#include "LASSIE.h"

#ifndef _PARTIALWINDOW_H
#define _PARTIALWINDOW_H

class PartialWindow : public Gtk::Dialog {

public:
    PartialWindow(std::string _originalString, ModifierType _type);
    ~PartialWindow();
    void AddNodeButtonClicked();
    void FunButtonClicked();
    void textChanged();
    std::string getResultString();
    std::string getFunctionString(DOMElement* _thisFunctionElement);

private:
    Glib::RefPtr<Gtk::Builder> attributesRefBuilder;
    ModifierType type;
    
    // We dedicate a class to each "sub alignment" -- each partial row
    class PartialSubAlignment:public Gtk::Alignment {
    public:
        PartialSubAlignment(PartialWindow* _parent, int partialNum);
        ~PartialSubAlignment();
        void appendNewNode(PartialSubAlignment* _newNode);
        void clear();
        void setProbEntry(std::string _string);
        void setAmpValueEntry(std::string _string);
        void setWidthEntry(std::string _string);
        void setRateValueEntry(std::string _string);
        std::string toString();
        PartialSubAlignment* prev;
        PartialSubAlignment* next;
    private:
        Glib::RefPtr<Gtk::Builder> attributesRefBuilder;
        PartialWindow* parent;
        int partialNum;

        void removeButtonClicked();
        void textChanged();
    };

    void partialRemoveNode(PartialSubAlignment * _remove);
    PartialSubAlignment* partialSubAlignments;
    int partialNumOfNodes;
};

#endif
#include "LASSIE.h"

#ifndef _PARTIALWINDOW_H
#define _PARTIALWINDOW_H

class PartialWindow : public Gtk::Dialog {

public:
    PartialWindow();
    ~PartialWindow();
    void AddNodeButtonClicked();
    void FunButtonClicked();
    void textChanged();

private:
    Glib::RefPtr<Gtk::Builder> attributesRefBuilder;
    
    // We dedicate a class to each "sub alignment" -- each partial row
    class PartialSubAlignment:public Gtk::Alignment {
    public:
        PartialSubAlignment(PartialWindow* _parent, int partialNum);
        ~PartialSubAlignment();
        void appendNewNode(PartialSubAlignment* _newNode);
        void clear();
        // void setFunctionsEntry(std::string _string);
        // void setMinEntry(std::string _string);
        // void setMaxEntry(std::string _string);
        // void setDistEntry(std::string _string);
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
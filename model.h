#ifndef ___MODEL___H
#define ___MODEL___H

#include "view.h"
#include "controller.h"
#include <memory>
#include "action.h"
using namespace std;

class Model
{


    protected:
        unique_ptr<Controller> control;
        unique_ptr<View> view;
        void addControl(unique_ptr<Controller> c);
        void addView(unique_ptr<View> c);
        Action getAction();
    public:
        void displayView();
        void updateView(Action action); //maybe pass in state here, and decide what state should include.
        void updateView(int c);
        virtual ~Model() {}
        //add contrroler
        //add view
};
#endif
#include "view.h"
#include "controller.h"
#include <memory>
#include "model.h"
#include "action.h"

void Model::addControl(unique_ptr<Controller> c)
{
    control = std::move(c);
}

void Model::addView(unique_ptr<View> c){
    view = std::move(c);
}

std::pair<int,Action> Model::getAction(){
    return control->getAction();
}

void Model::displayView(){
    view->displayView(); 
}

void Model::updateView(int c){
    view->updateView(c);
}

void Model::updateView(std::pair<int,Action> data){
    view->updateView(data); 
}


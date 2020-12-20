#include "event.h"
#include "vm.h"
void Event::setContent(std::vector<std::vector<char>> content,std::pair<int,int> data, int pasteLN)
{
    this->content = content;
    this->data = data;
    this->pasteLN = pasteLN;
}
void Event::contentPushback(std::vector<char> pushback){
    content.push_back(pushback);
}
std::vector<int> Event::undo()
{
    int line = cursorY + firstDisplayLine;
    std::vector<std::vector<char>> &buffer = vm.getBuffer();
    std::vector<int> viewInfo{cursorY, firstDisplayLine, cursorX, maxH};
    switch (action)
    {
    case joinThisAndNextWithSpace:
        //means we joined line, (cursorY + firstdisplayline) together
        //check if line+1 = maxH
        /*  if the above is true, do nothing
            else I must have combined line and line+1
        */
        if (line+1 < maxH){ //something must have merged
            auto ite = buffer.begin() + data.first; //get iterator to the line of target
            ite =  buffer.erase(ite); //erased the merged line
            ite = buffer.insert(ite, content.back()); //insert in the line1 line
            buffer.insert(ite, content.front());     //insert first line
        }
        break;
    case deleteLine:
        /*
        if maxH = 1;
            I cleared line, so I should insert back to first line
        else
            I am going to insert line at ite
        */
        if (maxH == 1){ //if it was cleared, i just need to move the firs tine back
            buffer.erase(buffer.begin());
            buffer.push_back(content.front());
        }
        else
        {
            buffer.insert(buffer.begin() + data.first, content.front());
        }
        break;
    case deleteLeft:
        //if this is called, an erase MUST have happenend, and we can just replace the line with the old version
        buffer.erase(buffer.begin()+ data.first);
        buffer.insert(buffer.begin()+data.first, content.front());
        break;
    case deleteRight:
        buffer.erase(buffer.begin()+ data.first);
        buffer.insert(buffer.begin()+data.first, content.front());
        break;
    case deleteUp:
        buffer.insert(buffer.begin() + data.first, content.back());
        buffer.insert(buffer.begin()+data.first, content.front());
        break;
    case deleteDown:
        buffer.insert(buffer.begin() + data.first-1, content.front());
        buffer.insert(buffer.begin()+data.first-1, content.back());
        break;
    case deleteLeftNC:
        buffer.erase(buffer.begin()+ data.first);
        buffer.insert(buffer.begin()+data.first, content.front());
        break;
    case deleteRightNC:
        buffer.erase(buffer.begin()+ data.first);
        buffer.insert(buffer.begin()+data.first, content.front());
        break;
    case deleteUpNC:
        buffer.insert(buffer.begin() + data.first, content.back());
        buffer.insert(buffer.begin()+data.first, content.front());
        break;
    case deleteDownNC:
        buffer.insert(buffer.begin() + data.first-1, content.front());
        buffer.insert(buffer.begin()+data.first-1, content.back());
        break;
    case clearLine:
        buffer.erase(buffer.begin() + data.first);
        buffer.insert(buffer.begin() + data.first, content.front());
        break;
    case deleteChar:
        buffer.erase(buffer.begin()+ data.first);
        buffer.insert(buffer.begin()+data.first, content.front());
        break;
    case deleteCharThenInsert:
        buffer.erase(buffer.begin()+ data.first);
        buffer.insert(buffer.begin()+data.first, content.front());
        break;
    case replaceCharWith:
        buffer.erase(buffer.begin() + data.first);
        buffer.insert(buffer.begin() + data.first, content.front());
        break;
    case pasteAfterCursor:
        /*
            2 cases here:
                1) inline pasted = 0
                        we save the line we pasted to

                        so now we erase that line and replace it with the old one.
                2) NOt inlined pasted!= 0
                        remove that number of lines after data.first

        */
        if (pasteLN == 0){ //inline paste
            buffer.erase(buffer.begin() + data.first);
            buffer.insert(buffer.begin() + data.first, content.front());
        }
        else
        {
            for (int i = 0; i < pasteLN;i++){
                buffer.erase(buffer.begin() + data.first + 1);
            }
        }
        break;
    case pasteBeforeCursor:
        if (pasteLN == 0){ //inline paste
            buffer.erase(buffer.begin() + data.first);
            buffer.insert(buffer.begin() + data.first, content.front());
        }
        else
        {
            for (int i = 0; i < pasteLN;i++){
                buffer.erase(buffer.begin() + data.first);
            }
        }
        break;
    case insertNLAboveAndInsert:
        buffer.erase(buffer.begin() + data.first);
        break;
    case insertNLUnderAndInsert:
        buffer.erase(buffer.begin() + data.first+1);
        break;
    case toInsert: //case where we need to STORE EVERYTHING IN THE FILE GRRR
        buffer.swap(content);
        break;
    default:
        break;
    }
    return viewInfo;
}


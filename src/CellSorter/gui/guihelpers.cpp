#include "guihelpers.h"

int GetIndexOfItem(const QComboBox &box, const QVariant &item){
    int index = 0;

    while (index < box.count()) {
        if (box.itemData(index) == item) {
            return index;
        }
        index++;
    }

    return 0;
}

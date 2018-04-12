#ifndef GUIHELPERS_H
#define GUIHELPERS_H

#include <QComboBox>

int GetIndexOfItem(const QComboBox& box, const QVariant& item) {
    int index = 0;

    while (index < box.count()) {
        if (box.itemData(index) == item) {
            return index;
        }
        index++;
    }

    return 0;
}

#endif  // GUIHELPERS_H

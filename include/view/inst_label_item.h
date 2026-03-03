/**
 * @file inst_label_item.h
 * @brief Text label items for Inst name and module name.
 * @author
 * @date
 */

#ifndef SCHEMATIC_VIEWER_VIEW_INST_LABEL_ITEM_H
#define SCHEMATIC_VIEWER_VIEW_INST_LABEL_ITEM_H

#include <QGraphicsTextItem>
#include <QFont>

/**
 * @brief InstLabelItem displays inst name or module name as separate graphics item.
 */
class InstLabelItem : public QGraphicsTextItem {
public:
    /**
     * @brief Constructor.
     * @param text Label text.
     * @param parent Parent item.
     */
    explicit InstLabelItem(const QString& text, QGraphicsItem* parent = nullptr);

    /**
     * @brief Set the label text.
     * @param text New text.
     */
    void setLabelText(const QString& text);

    /**
     * @brief Get the label text size.
     * @return Size of the text bounding rect.
     */
    QSizeF getTextSize() const;

    /**
     * @brief Set font for the label.
     * @param font Font to use.
     */
    void setLabelFont(const QFont& font);

    /**
     * @brief Update position to center above or below the box.
     * @param centerX Center X position of the inst box.
     * @param boxTop Top Y of the inst box.
     * @param boxBottom Bottom Y of the inst box.
     * @param isName true for name (above), false for module name (below).
     */
    void updatePosition(qreal centerX, qreal boxTop, qreal boxBottom, bool isName);

private:
    QFont labelFont_;
};

#endif  // SCHEMATIC_VIEWER_VIEW_INST_LABEL_ITEM_H

/**
 * @file text_item.h
 * @brief Generic text item for schematic labels.
 * @author
 * @date
 */

#ifndef SCHEMATIC_VIEWER_VIEW_TEXT_ITEM_H
#define SCHEMATIC_VIEWER_VIEW_TEXT_ITEM_H

#include <QGraphicsTextItem>
#include <QFont>

/**
 * @brief TextItem displays text at a specific position.
 */
class TextItem : public QGraphicsTextItem {
public:
    /**
     * @brief Constructor.
     * @param text Initial text.
     * @param parent Parent item.
     */
    explicit TextItem(const QString& text = QString(), QGraphicsItem* parent = nullptr);

    /**
     * @brief Set the text content.
     * @param text New text.
     */
    void setText(const QString& text);

    /**
     * @brief Get the text content.
     * @return Current text.
     */
    QString getText() const { return toPlainText(); }

    /**
     * @brief Get the text bounding size.
     * @return Size of the text bounding rect.
     */
    QSizeF getTextSize() const;

    /**
     * @brief Set font for the text.
     * @param font Font to use.
     */
    void setTextFont(const QFont& font);

    /**
     * @brief Update position to center horizontally at given Y.
     * @param centerX Center X position.
     * @param y Y position.
     */
    void updatePositionCentered(qreal centerX, qreal y);

    /**
     * @brief Update position with offset from a reference point.
     * @param refX Reference X position.
     * @param refY Reference Y position.
     * @param offsetX Horizontal offset.
     * @param offsetY Vertical offset.
     */
    void updatePosition(qreal refX, qreal refY, qreal offsetX, qreal offsetY);

private:
    QFont textFont_;
};

#endif  // SCHEMATIC_VIEWER_VIEW_TEXT_ITEM_H

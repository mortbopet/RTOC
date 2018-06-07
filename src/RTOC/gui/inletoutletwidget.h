#ifndef INLETOUTLETWIDGET_H
#define INLETOUTLETWIDGET_H

#include <QDialog>
#include <QGraphicsScene>

namespace Ui {
class InletOutletWidget;
}
enum class State { idle, setInlet, setOutlet };

class InletOutletScene : public QGraphicsScene {
    Q_OBJECT
public:
    explicit InletOutletScene(QObject* parent = 0) {}

    void setState(State state) { m_state = state; }

    void load(State state, const QPointF& p);

signals:
    void unlockButtons();
    void pointMoved(const QPointF& p, bool inletOrOutlet);

protected:
    void setPoint(QGraphicsEllipseItem** i, const QPointF& p, Qt::GlobalColor c);
    QPointF keepInRect(QPointF event);
    QPointF removeOffset(QPointF p);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event);

    const int pointWidth = 5;

    QPoint inlet;
    QPoint outlet;

    QGraphicsEllipseItem* inletPoint = nullptr;
    QGraphicsEllipseItem* outletPoint = nullptr;
    QGraphicsEllipseItem* mousePoint = nullptr;

    QGraphicsEllipseItem* createPoint(Qt::GlobalColor color);

    State m_state = State::idle;
};

class InletOutletWidget : public QDialog {
    Q_OBJECT

public:
    explicit InletOutletWidget(const QImage& image, QWidget* parent = 0);
    ~InletOutletWidget();

    int inletX, inletY, outletX, outletY;

    void load(const std::pair<int, int>& p, bool inletOrOutlet);

private slots:
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();
    void unlockButtons();

    void setPointText(const QPointF& p, bool inletOrOutlet);

private:
    Ui::InletOutletWidget* ui;

    void setPositionOf(bool inletOrOutlet);

    InletOutletScene* m_scene;
};

#endif  // INLETOUTLETWIDGET_H

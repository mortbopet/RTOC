#include "inletoutletwidget.h"
#include "ui_inletoutletwidget.h"

#include <QMessageBox>

#include <QGraphicsEllipseItem>
#include <QGraphicsSceneMouseEvent>

QPointF InletOutletScene::keepInRect(QPointF pos) {
    auto rect = sceneRect();
    if (pos.x() < rect.left()) {
        pos.setX(rect.left());
    }

    if (pos.x() > rect.right()) {
        pos.setX(rect.right());
    }

    if (pos.y() < rect.top()) {
        pos.setY(rect.top());
    }

    if (pos.y() > rect.bottom()) {
        pos.setY(rect.bottom());
    }

    pos -= QPointF(pointWidth / 2, pointWidth / 2);

    return pos;
}

QPointF InletOutletScene::removeOffset(QPointF p) {
    p += QPointF(pointWidth / 2, pointWidth / 2);
    return p;
}

QGraphicsEllipseItem* InletOutletScene::createPoint(Qt::GlobalColor color) {
    auto p = new QGraphicsEllipseItem(0, 0, pointWidth, pointWidth);
    p->setPen(QPen(color, 3, Qt::SolidLine));
    p->setBrush(color);
    return p;
}

void InletOutletScene::load(State state, const QPointF& p) {
    auto pos = keepInRect(p);
    m_state = state;
    if (m_state == State::setInlet) {
        setPoint(&inletPoint, pos, Qt::green);
    } else if (m_state == State::setOutlet) {
        setPoint(&outletPoint, pos, Qt::red);
    }
    m_state = State::idle;
}

void InletOutletScene::setPoint(QGraphicsEllipseItem** i, const QPointF& p, Qt::GlobalColor c) {
    if (*i == nullptr) {
        *i = createPoint(c);
        (*i)->setPos(p);
        addItem(*i);
    } else {
        (*i)->setVisible(true);
        (*i)->setPos(p);
    }

    if (m_state == State::setInlet || m_state == State::setOutlet) {
        bool inletOrOutlet = m_state == State::setInlet ? true : false;
        emit pointMoved(removeOffset((*i)->pos()), inletOrOutlet);
    }
}

void InletOutletScene::mouseMoveEvent(QGraphicsSceneMouseEvent* event) {
    auto pos = keepInRect(event->scenePos());

    if (m_state == State::setInlet || m_state == State::setOutlet) {
        setPoint(&mousePoint, pos, Qt::yellow);
    } else {
        if (mousePoint)
            mousePoint->setVisible(false);
        QGraphicsScene::mouseMoveEvent(event);
    }
}

void InletOutletScene::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
    auto pos = keepInRect(event->scenePos());
    if (m_state == State::setInlet) {
        setPoint(&inletPoint, pos, Qt::green);
    } else if (m_state == State::setOutlet) {
        setPoint(&outletPoint, pos, Qt::red);
    }
    m_state = State::idle;
    emit unlockButtons();
}

// --------------------------------------------------------------------------

InletOutletWidget::InletOutletWidget(const QImage& image, QWidget* parent)
    : QDialog(parent), ui(new Ui::InletOutletWidget) {
    ui->setupUi(this);

    setWindowTitle("Set inlet and outlet");

    connect(ui->setInlet, &QPushButton::clicked, this, [=] { setPositionOf(true); });
    connect(ui->setOutlet, &QPushButton::clicked, this, [=] { setPositionOf(false); });

    // create scene
    m_scene = new InletOutletScene();
    ui->graphicsView->setScene(m_scene);
    ui->graphicsView->setMouseTracking(true);

    m_scene->addPixmap(QPixmap::fromImage(image));
    m_scene->setSceneRect(image.rect());

    ui->graphicsView->setMinimumSize(image.size());

    connect(m_scene, &InletOutletScene::pointMoved, this, &InletOutletWidget::setPointText);
    connect(m_scene, &InletOutletScene::unlockButtons, this, &InletOutletWidget::unlockButtons);

    unlockButtons();
}

void InletOutletWidget::load(const std::pair<int, int>& p, bool inletOrOutlet) {
    auto state = inletOrOutlet ? State::setInlet : State::setOutlet;
    m_scene->load(state, QPointF(p.first, p.second));
}

InletOutletWidget::~InletOutletWidget() {
    delete ui;
}

void InletOutletWidget::on_buttonBox_accepted() {
    inletX = ui->inletX->text().toInt();
    inletY = ui->inletY->text().toInt();
    outletX = ui->outletX->text().toInt();
    outletY = ui->outletY->text().toInt();
    QDialog::accept();
}

void InletOutletWidget::on_buttonBox_rejected() {
    QDialog::reject();
}

void InletOutletWidget::setPositionOf(bool inletOrOutlet) {
    ui->setInlet->setDisabled(true);
    ui->setOutlet->setDisabled(true);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
    if (inletOrOutlet) {
        m_scene->setState(State::setInlet);
    } else {
        m_scene->setState(State::setOutlet);
    }
}

void InletOutletWidget::setPointText(const QPointF& p, bool inletOrOutlet) {
    if (inletOrOutlet) {
        ui->inletX->setText(QString::number(p.x()));
        ui->inletY->setText(QString::number(p.y()));
    } else {
        ui->outletX->setText(QString::number(p.x()));
        ui->outletY->setText(QString::number(p.y()));
    }
}

void InletOutletWidget::unlockButtons() {
    ui->setInlet->setEnabled(true);
    ui->setOutlet->setEnabled(true);

    // simple check to verify that both have been set
    if (ui->inletX->text().isEmpty() || ui->outletX->text().isEmpty()) {
        // Disable accepted
        ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
    } else {
        ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
    }
}

#ifndef LAMBOWIDGET_H
#define LAMBOWIDGET_H

#include <QWidget>
#include <Qt3DExtras/Qt3DWindow>
#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <Qt3DRender/QSceneLoader>
#include <QTimer>
#include <QMouseEvent>   // Fare eventleri için ekle

class LamboWidget : public QWidget
{
    Q_OBJECT
public:
    explicit LamboWidget(QWidget *parent = nullptr);

protected:

    bool eventFilter(QObject *obj, QEvent *event) override;

private slots:
    void onSceneStatusChanged(Qt3DRender::QSceneLoader::Status status);
    void updateRoadAnimation();
    void setupCamera();
    void setupLight(Qt3DCore::QEntity *parent);
    void createRoad(Qt3DCore::QEntity *parent);
    void loadCarModel(Qt3DCore::QEntity *parent);

private:


    Qt3DExtras::Qt3DWindow *view;
    QWidget *viewContainer;
    Qt3DCore::QEntity *sceneRoot;

    Qt3DCore::QTransform *roadTransform;
    QTimer *roadTimer;

    Qt3DCore::QEntity *carEntity;
    Qt3DRender::QSceneLoader *sceneLoader;

    // Yeni: araba ve yol entitylerinin ortak root'u
    Qt3DCore::QEntity *carAndRoadRoot;
    Qt3DCore::QTransform *carAndRoadTransform;

    // Fare takibi
    QPoint lastMousePos;
    bool mousePressed;

    // Kamera açıları
    float azimuth = 0.0f;    // Yatay açı
    float elevation = 20.0f; // Dikey açı, biraz yukarıya bakacak şekilde başlat
};

#endif // LAMBOWIDGET_H

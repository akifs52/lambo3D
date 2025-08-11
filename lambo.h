#ifndef LAMBO_H
#define LAMBO_H

#include <QWidget>
#include <QTimer>
#include <Qt3DCore/QEntity>
#include <Qt3DRender/QCamera>
#include <Qt3DRender/QSceneLoader>
#include <Qt3DExtras/Qt3DExtras>
QT_BEGIN_NAMESPACE

QT_END_NAMESPACE

class LamboWidget : public QWidget
{
    Q_OBJECT
public:
    explicit LamboWidget(QWidget *parent = nullptr);
    ~LamboWidget();

private slots:
    void updateRoadAnimation();
    void onSceneStatusChanged(Qt3DRender::QSceneLoader::Status status);

private:
    void setupCamera();
    void setupLight(Qt3DCore::QEntity *parent);
    void createRoad(Qt3DCore::QEntity *parent);
    void loadCarModel(Qt3DCore::QEntity *parent);
    void createScene();



private:

    Qt3DExtras::Qt3DWindow *view;
    QWidget *viewContainer;
    Qt3DCore::QEntity *sceneRoot;

    Qt3DCore::QEntity *carEntity;
    Qt3DRender::QSceneLoader *sceneLoader;

    Qt3DRender::QCamera *camera;

    Qt3DCore::QTransform *roadTransform;
    QTimer *roadTimer;


};

#endif // LAMBO_H

#include "lambo.h"

#include <Qt3DRender/QCamera>
#include <Qt3DRender/QPointLight>
#include <Qt3DExtras/QCuboidMesh>
#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DExtras/QForwardRenderer>
#include <Qt3DCore/QTransform>
#include <QHBoxLayout>
#include <QUrl>
#include <QDebug>
#include <QMouseEvent>
#include <QtMath>

LamboWidget::LamboWidget(QWidget *parent)
    : QWidget(parent),
    view(new Qt3DExtras::Qt3DWindow()),
    sceneRoot(new Qt3DCore::QEntity()),
    roadTransform(nullptr),
    roadTimer(new QTimer(this)),
    carEntity(nullptr),
    sceneLoader(nullptr),
    carAndRoadRoot(new Qt3DCore::QEntity(sceneRoot)),
    carAndRoadTransform(new Qt3DCore::QTransform()),
    mousePressed(false),
    azimuth(0.0f),
    elevation(20.0f)
{
    view->defaultFrameGraph()->setClearColor(QColor("#1c2331"));

    viewContainer = QWidget::createWindowContainer(view, this);
    viewContainer->setMinimumSize(400, 300);
    viewContainer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // Burada event filter ekliyoruz
    viewContainer->installEventFilter(this);

    auto *lay = new QHBoxLayout(this);
    lay->setContentsMargins(0,0,0,0);
    lay->addWidget(viewContainer);

    setupCamera();
    setupLight(sceneRoot);

    carAndRoadRoot->addComponent(carAndRoadTransform);

    createRoad(carAndRoadRoot);
    loadCarModel(carAndRoadRoot);

    view->setRootEntity(sceneRoot);

    connect(roadTimer, &QTimer::timeout, this, &LamboWidget::updateRoadAnimation);
    roadTimer->start(16);
}

void LamboWidget::setupCamera()
{
    auto *camera = view->camera();
    camera->lens()->setPerspectiveProjection(45.0f, 16.0f/9.0f, 0.1f, 1000.0f);

    float distance = 12.0f;
    float radAzimuth = qDegreesToRadians(azimuth);
    float radElevation = qDegreesToRadians(elevation);

    float x = distance * cos(radElevation) * sin(radAzimuth);
    float y = distance * sin(radElevation);
    float z = distance * cos(radElevation) * cos(radAzimuth);

    camera->setPosition(QVector3D(x, y, z));
    camera->setViewCenter(QVector3D(0.0f, 0.5f, 0.0f));
}

void LamboWidget::setupLight(Qt3DCore::QEntity *parent)
{
    Qt3DCore::QEntity *lightEntity = new Qt3DCore::QEntity(parent);
    auto *light = new Qt3DRender::QPointLight(lightEntity);
    light->setColor("white");
    light->setIntensity(1.0f);

    auto *lightTransform = new Qt3DCore::QTransform();
    lightTransform->setTranslation(QVector3D(0.0f, 10.0f, 10.0f));

    lightEntity->addComponent(light);
    lightEntity->addComponent(lightTransform);
}

void LamboWidget::createRoad(Qt3DCore::QEntity *parent)
{
    Qt3DCore::QEntity *roadEntity = new Qt3DCore::QEntity(parent);

    auto *roadMesh = new Qt3DExtras::QCuboidMesh();
    roadMesh->setXExtent(10.0f);
    roadMesh->setYExtent(0.1f);
    roadMesh->setZExtent(100.0f);

    roadTransform = new Qt3DCore::QTransform();
    roadTransform->setTranslation(QVector3D(0.0f, -0.05f, 0.0f));

    auto *roadMaterial = new Qt3DExtras::QPhongMaterial();
    roadMaterial->setDiffuse(QColor(50,50,50));

    roadEntity->addComponent(roadMesh);
    roadEntity->addComponent(roadTransform);
    roadEntity->addComponent(roadMaterial);

    Qt3DCore::QEntity *lineEntity = new Qt3DCore::QEntity(parent);
    auto *lineMesh = new Qt3DExtras::QCuboidMesh();
    lineMesh->setXExtent(0.3f);
    lineMesh->setYExtent(0.06f);
    lineMesh->setZExtent(100.0f);

    auto *lineTransform = new Qt3DCore::QTransform();
    lineTransform->setTranslation(QVector3D(0.0f, 0.02f, 0.0f));

    auto *lineMat = new Qt3DExtras::QPhongMaterial();
    lineMat->setDiffuse(QColor(Qt::yellow));

    lineEntity->addComponent(lineMesh);
    lineEntity->addComponent(lineTransform);
    lineEntity->addComponent(lineMat);
}

void LamboWidget::loadCarModel(Qt3DCore::QEntity *parent)
{
    carEntity = new Qt3DCore::QEntity(parent);

    sceneLoader = new Qt3DRender::QSceneLoader(carEntity);
    connect(sceneLoader, &Qt3DRender::QSceneLoader::statusChanged,
            this, &LamboWidget::onSceneStatusChanged);

    QUrl modelUrl("qrc:/sources/sources/lamborghini.obj");
    sceneLoader->setSource(modelUrl);

    auto *carTransform = new Qt3DCore::QTransform();
    carTransform->setScale(1.0f);
    carTransform->setTranslation(QVector3D(0.0f, 0.0f, 0.0f));

    carEntity->addComponent(sceneLoader);
    carEntity->addComponent(carTransform);
}

void LamboWidget::onSceneStatusChanged(Qt3DRender::QSceneLoader::Status status)
{
    if (status == Qt3DRender::QSceneLoader::Ready) {
        qDebug() << "Model yüklendi.";
    } else if (status == Qt3DRender::QSceneLoader::Loading) {
        qDebug() << "Model yükleniyor...";
    } else if (status == Qt3DRender::QSceneLoader::Error) {
        qDebug() << "Model yüklenemedi!";
    }
}

void LamboWidget::updateRoadAnimation()
{
    if (!roadTransform || !carAndRoadTransform) return;

    static float offset = 0.0f;
    static float rotationAngle = 0.0f;

    offset += 0.3f;
    if (offset > 50.0f) offset = 0.0f;
    roadTransform->setTranslation(QVector3D(0.0f, -0.05f, offset));

    rotationAngle += 0.3f;
    if (rotationAngle > 360.0f) rotationAngle -= 360.0f;

    carAndRoadTransform->setRotation(QQuaternion::fromEulerAngles(0.0f, rotationAngle, 0.0f));
}

bool LamboWidget::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == viewContainer) {
        if (event->type() == QEvent::MouseButtonPress) {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            if (mouseEvent->button() == Qt::LeftButton) {
                mousePressed = true;
                lastMousePos = mouseEvent->pos();
                return true;
            }
        }
        else if (event->type() == QEvent::MouseMove) {
            if (mousePressed) {
                QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
                QPoint delta = mouseEvent->pos() - lastMousePos;
                lastMousePos = mouseEvent->pos();

                azimuth += delta.x() * 0.5f;
                elevation += -delta.y() * 0.5f;

                if (elevation > 89.0f) elevation = 89.0f;
                if (elevation < -89.0f) elevation = -89.0f;

                auto *camera = view->camera();
                float distance = 12.0f;
                float radAzimuth = qDegreesToRadians(azimuth);
                float radElevation = qDegreesToRadians(elevation);

                float x = distance * cos(radElevation) * sin(radAzimuth);
                float y = distance * sin(radElevation);
                float z = distance * cos(radElevation) * cos(radAzimuth);

                camera->setPosition(QVector3D(x, y, z));
                camera->setViewCenter(QVector3D(0.0f, 0.5f, 0.0f));

                return true;
            }
        }
        else if (event->type() == QEvent::MouseButtonRelease) {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            if (mouseEvent->button() == Qt::LeftButton) {
                mousePressed = false;
                return true;
            }
        }
    }

    return QWidget::eventFilter(obj, event);
}

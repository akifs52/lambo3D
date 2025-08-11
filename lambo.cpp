#include "lambo.h"

#include <Qt3DExtras/Qt3DWindow>
#include <Qt3DRender/QPointLight>
#include <Qt3DExtras/QCuboidMesh>
#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DCore/QTransform>
#include <Qt3DExtras/QOrbitCameraController>
#include <QHBoxLayout>
#include <QDebug>

LamboWidget::LamboWidget(QWidget *parent)
    : QWidget(parent),
    view(new Qt3DExtras::Qt3DWindow()),
    viewContainer(nullptr),
    sceneRoot(nullptr),
    carEntity(nullptr),
    sceneLoader(nullptr),
    camera(nullptr),
    roadTransform(nullptr),
    roadTimer(new QTimer(this))

{
    view->defaultFrameGraph()->setClearColor(QColor("#1c2331"));

    viewContainer = QWidget::createWindowContainer(view, this);
    viewContainer->setMinimumSize(480, 320);
    viewContainer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    auto *lay = new QHBoxLayout(this);
    lay->setContentsMargins(0,0,0,0);
    lay->addWidget(viewContainer);

    // Root entity
    sceneRoot = new Qt3DCore::QEntity();

    // Kamera ve ışık ayarları
    setupCamera();
    setupLight(sceneRoot);

    // Sahneyi oluştur
    createScene();

    // Root entity'i görüntüye bağla
    view->setRootEntity(sceneRoot);

    // Kamera kontrolcüyü kur (mouse ile orbit, pan ve zoom)
    auto *camController = new Qt3DExtras::QOrbitCameraController(sceneRoot);
    camController->setCamera(camera);
    camController->setLinearSpeed(50.0f);
    camController->setLookSpeed(180.0f);

    // Yol animasyonu timer
    connect(roadTimer, &QTimer::timeout, this, &LamboWidget::updateRoadAnimation);
    roadTimer->start(16);
}

LamboWidget::~LamboWidget()
{
    if (roadTimer) roadTimer->stop();

}

void LamboWidget::setupCamera()
{
    camera = view->camera();
    camera->lens()->setPerspectiveProjection(45.0f, 16.0f/9.0f, 0.1f, 1000.0f);
    camera->setPosition(QVector3D(0.0f, 5.0f, 12.0f));
    camera->setViewCenter(QVector3D(0.0f, 0.5f, 0.0f));
}

void LamboWidget::setupLight(Qt3DCore::QEntity *parent)
{
    auto *lightEntity = new Qt3DCore::QEntity(parent);
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
    auto *roadEntity = new Qt3DCore::QEntity(parent);
    auto *roadMesh = new Qt3DExtras::QCuboidMesh();
    roadMesh->setXExtent(10.0f);
    roadMesh->setYExtent(0.1f);
    roadMesh->setZExtent(100.0f);

    roadTransform = new Qt3DCore::QTransform();
    roadTransform->setTranslation(QVector3D(0.0f, -0.05f, 0.0f));

    auto *roadMaterial = new Qt3DExtras::QPhongMaterial();
    roadMaterial->setDiffuse(QColor(50, 50, 50));

    roadEntity->addComponent(roadMesh);
    roadEntity->addComponent(roadTransform);
    roadEntity->addComponent(roadMaterial);

    // Orta çizgi
    auto *lineEntity = new Qt3DCore::QEntity(parent);
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

    carEntity->addComponent(sceneLoader);

    auto *carTransform = new Qt3DCore::QTransform();
    carTransform->setScale(1.0f);
    carTransform->setTranslation(QVector3D(0.0f, 0.0f, 0.0f));
    carEntity->addComponent(carTransform);
}

void LamboWidget::createScene()
{
    auto *roadRoot = new Qt3DCore::QEntity(sceneRoot);
    createRoad(roadRoot);

    auto *carRoot = new Qt3DCore::QEntity(sceneRoot);
    loadCarModel(carRoot);
}

void LamboWidget::onSceneStatusChanged(Qt3DRender::QSceneLoader::Status status)
{
    if (status == Qt3DRender::QSceneLoader::Ready) {
        qDebug() << "[Lambo] Model yüklendi.";
    } else if (status == Qt3DRender::QSceneLoader::Loading) {
        qDebug() << "[Lambo] Model yükleniyor...";
    } else if (status == Qt3DRender::QSceneLoader::Error) {
        qDebug() << "[Lambo] Model yüklenemedi! (QSceneLoader Error)";
    }
}

void LamboWidget::updateRoadAnimation()
{
    if (!roadTransform) return;
    static float offset = 0.0f;
    offset += 0.3f;
    if (offset > 50.0f) offset = 0.0f;
    roadTransform->setTranslation(QVector3D(0.0f, -0.05f, offset));
}


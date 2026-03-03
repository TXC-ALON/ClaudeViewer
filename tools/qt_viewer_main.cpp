/**
 * @file qt_viewer_main.cpp
 * @brief Main entry point for Qt viewer.
 * @author
 * @date
 */

#include "layout/layout_manager.h"
#include "model/object_manager.h"
#include "parser/command_dispatcher.h"
#include "view/schematic_scene.h"

#include <QApplication>
#include <QGraphicsView>
#include <QMainWindow>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QVBoxLayout>
#include <QWidget>
#include <QLabel>
#include <QScrollArea>
#include <iostream>
#include <fstream>
#include <string>

/**
 * @brief Main window for the schematic viewer.
 */
class SchematicViewerWindow : public QMainWindow {
public:
    SchematicViewerWindow()
        : objMgr_()
        , layoutMgr_(objMgr_)
        , scene_(objMgr_, layoutMgr_)
        , dispatcher_(objMgr_)
    {
        setupUi();

        // Run initial layout
        scene_.runAutoLayout();
    }

    /**
     * @brief Process a JSON command.
     * @param jsonStr JSON command string.
     * @return true if successful.
     */
    bool processCommand(const std::string& jsonStr) {
        bool success = dispatcher_.dispatch(jsonStr);
        if (success) {
            scene_.runAutoLayout();
        }
        return success;
    }

    /**
     * @brief Load schematic from JSON commands.
     * @param commands Vector of JSON command strings.
     */
    void loadSchematic(const std::vector<std::string>& commands) {
        for (const auto& cmd : commands) {
            dispatcher_.dispatch(cmd);
        }
        scene_.runAutoLayout();
    }

private:
    void setupUi() {
        setWindowTitle("Schematic Viewer");
        resize(1024, 768);

        // Create central widget
        QWidget* centralWidget = new QWidget;
        setCentralWidget(centralWidget);

        // Create layout
        QVBoxLayout* layout = new QVBoxLayout(centralWidget);

        // Create graphics view
        QGraphicsView* view = new QGraphicsView(&scene_);
        view->setRenderHint(QPainter::Antialiasing);
        view->setDragMode(QGraphicsView::ScrollHandDrag);
        view->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
        view->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
        view->setResizeAnchor(QGraphicsView::AnchorUnderMouse);

        layout->addWidget(view);

        // Create menu bar
        QMenuBar* menuBar = new QMenuBar(this);
        setMenuBar(menuBar);

        // File menu
        QMenu* fileMenu = menuBar->addMenu("&File");

        QAction* exitAction = new QAction("E&xit", this);
        exitAction->setShortcut(QKeySequence::Quit);
        connect(exitAction, &QAction::triggered, this, &QMainWindow::close);
        fileMenu->addAction(exitAction);

        // View menu
        QMenu* viewMenu = menuBar->addMenu("&View");

        QAction* zoomInAction = new QAction("Zoom &In", this);
        zoomInAction->setShortcut(QKeySequence::ZoomIn);
        connect(zoomInAction, &QAction::triggered, [view]() { view->scale(1.2, 1.2); });
        viewMenu->addAction(zoomInAction);

        QAction* zoomOutAction = new QAction("Zoom &Out", this);
        zoomOutAction->setShortcut(QKeySequence::ZoomOut);
        connect(zoomOutAction, &QAction::triggered, [view]() { view->scale(1.0/1.2, 1.0/1.2); });
        viewMenu->addAction(zoomOutAction);

        QAction* resetZoomAction = new QAction("&Reset Zoom", this);
        connect(resetZoomAction, &QAction::triggered, view, &QGraphicsView::resetTransform);
        viewMenu->addAction(resetZoomAction);
    }

    ObjectManager objMgr_;
    LayoutManager layoutMgr_;
    SchematicScene scene_;
    CommandDispatcher dispatcher_;
};

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    SchematicViewerWindow window;

    // Load sample schematic if no arguments provided
    if (argc == 1) {
        std::vector<std::string> sampleCommands = {
            R"({"action":"load","type":"inst","id":"1","name":"pic0","module_name":"top","parent_id":"1"})",
            R"({"action":"load","type":"inst","id":"2","name":"pic1","module_name":"buf","parent_id":"1"})",
            R"({"action":"load","type":"pin","id":"101","name":"input","parent_id":"1","direction":"input"})",
            R"({"action":"load","type":"pin","id":"102","name":"output","parent_id":"1","direction":"output"})",
            R"({"action":"load","type":"pin","id":"201","name":"a","parent_id":"2","direction":"input"})",
            R"({"action":"load","type":"pin","id":"202","name":"y","parent_id":"2","direction":"output"})",
            R"({"action":"attribute","type":"pin","id":"101","connectionnum":1})",
            R"({"action":"attribute","type":"pin","id":"102","connectionnum":1})",
            R"({"action":"attribute","type":"pin","id":"201","connectionnum":1})",
            R"({"action":"attribute","type":"pin","id":"202","connectionnum":1})",
        };
        window.loadSchematic(sampleCommands);
    } else {
        // Load commands from file
        std::string filename = argv[1];
        std::ifstream file(filename);
        if (file.is_open()) {
            std::vector<std::string> commands;
            std::string line;
            while (std::getline(file, line)) {
                if (!line.empty()) {
                    commands.push_back(line);
                }
            }
            window.loadSchematic(commands);
        } else {
            std::cerr << "Cannot open file: " << filename << std::endl;
        }
    }

    window.show();

    return app.exec();
}
